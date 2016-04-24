#include <cstdlib>			// atoi
#include <sys/socket.h>		// socket
#include <arpa/inet.h>		// inet
#include <stdio.h>			// printf
#include <unistd.h>			// read
#include <string.h>			// string and bzero

#include "../lib/utils.h"	// crypto dependencies

#define MAX_MSG_SIZE 4096

int client_socket_fd;
std::string aes_key;
std::string aes_iv;
std::string hmac_password;
std::string init_nonce;
std::string my_nonce;
std::string their_nonce;

int main(int argc, char* argv[]){

	// Check parameters
	if(argc != 2){
		printf("Error: Need port\n");
		return -1;	
	} 

	// Initialize variables
	int port = atoi(argv[1]);
	char read_buffer[MAX_MSG_SIZE+1];
	std::string write_buffer;
	int return_call = -1;
	int socket_fd;
	struct sockaddr_in server, client;
	socklen_t client_length;

	printf("Binding to port %d\n", port);

	// Open port
	// Declare socket file descripter
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	// Validity check
	if (socket_fd < 0){
		printf("Error creating socket\n");
	    return -1;
	}
	
	// Initialize socket
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    // Bind to port and listen for one connection (Bob)
    if(bind(socket_fd, (sockaddr *)&server, sizeof(server)) == 0){
      	listen(socket_fd, 1);
    }

    printf("Port bound. Waiting for Bob...\n");

    // Bind client when it arrives
    client_length = sizeof(client);
    client_socket_fd = accept(socket_fd, (struct sockaddr*) &client, &client_length);

    // Validity check
    if(client_socket_fd < 0){
    	printf("Error accepting client\n");
    	return -1;
    }

    printf("Bob connected. Beginning key exchange\n");

    // Begin key agreement
    // Read Bob's half of the key (encrypted)
    bzero(read_buffer, MAX_MSG_SIZE+1);
    return_call = recv(client_socket_fd, read_buffer, MAX_MSG_SIZE, 0);
    if(return_call < 0){
    	printf("Error reading Bob's encrypted key\n");
    	return -1;
    }

    // Determine end of message and store in string
    read_buffer[return_call] = 0;
    std::string encrypted_bob_key_half(read_buffer);
    encrypted_bob_key_half = hexstr2str(encrypted_bob_key_half);

    // Determine Bob's half of the key
	std::string bob_key_half = decryptWithBothKeys("bob.pub", "alice.key", encrypted_bob_key_half);
	
	printf("Determined Bob's half of key. Sending Alice's half\n");

	// Generate 128-bit random key for AES and encrypt with RSA
	std::string alice_key_half = readRand(16);
	std::string encrypted_alice_key = encryptWithBothKeys("bob.pub", "alice.key", alice_key_half);

	// Send key half (encrypted) to Bob
	write_buffer = str2hexstr(encrypted_alice_key);
	if(send(client_socket_fd, write_buffer.c_str(), write_buffer.length(), 0) < 0){
		printf("Error sending encrypted key\n");
		return -1;
	}

	printf("Key agreed\n");

	// Determine session key and split it into aes_key, aes_iv, hmac_password, and init_nonce
	std::string session_key = session_hash(bob_key_half, alice_key_half);
	aes_key = hexstr2str(session_key.substr(0,32));
	aes_iv = hexstr2str(session_key.substr(32,32));
	hmac_password = session_key.substr(64,32);
	init_nonce = session_key.substr(96,32);

    printf("Here is the nonce: %s\n", init_nonce.c_str());
    my_nonce = init_nonce;
    their_nonce = init_nonce;

    std::string msg = "Hi Bob!";
    if(send_aes_encrypted(client_socket_fd, msg, aes_key, aes_iv, hmac_password, their_nonce, my_nonce)){
    	printf("Message sent\n");
    }else{
    	printf("Error sending message\n");
    }

    std::string recvd = recv_aes_encrypted(client_socket_fd, hmac_password, aes_key, aes_iv, my_nonce, their_nonce);
    printf("Here is a message: %s\n", recvd.c_str());

	return 0;
}