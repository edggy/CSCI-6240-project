#include <cstdlib>			// atoi
#include <sys/socket.h>		// socket
#include <arpa/inet.h>		// inet
#include <stdio.h>			// printf
#include <unistd.h>			// read
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h> 
#include <string.h>			// string and bzero

#include "../lib/utils.h"	// crypto dependencies

#define MAX_MSG_SIZE 4096

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
	struct sockaddr_in serv_addr;
	struct hostent *server;

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
	server = gethostbyname("127.0.0.1");
	serv_addr.sin_family = AF_INET;
	bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);

	// Bind to port
	if(connect(socket_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0){
		printf("Error connecting to socket\n");
		return -1;
	}

	printf("Connected to Alice. Beginning key exchange\n");

	// Begin key exchange
	// Generate Bob's half of the key and encrypt it
	std::string bob_key_half = readRand(16);
	std::string encrypted_bob_key_half = encryptWithBothKeys("alice.pub", "bob.key", bob_key_half);

	// Send encrypted key to Alice
	if(send(socket_fd, encrypted_bob_key_half.c_str(), encrypted_bob_key_half.length(), 0) < 0){
		printf("Error sending encrypted key\n");
		return -1;
	}

	printf("Sent Bob's half of key to Alice.\n");

	// Receive Alice's encrypted key
	return_call = recv(socket_fd, read_buffer, MAX_MSG_SIZE, 0);
	if(return_call < 0){
    	printf("Error reading encrypted key\n");
    	return -1;
    }

    // Determine end of message and store in string
    read_buffer[return_call] = 0;
    std::string encrypted_alice_key_half(read_buffer);
    encrypted_alice_key_half = hexstr2str(encrypted_alice_key_half);

    // Decrypt Alice's half of the key
    std::string alice_key_half = decryptWithBothKeys("alice.pub", "bob.key", encrypted_alice_key_half);

    printf("Key received from Alice. Key agreed\n");

    // Determine session key and split it into aes_key, aes_iv, hmac_password, and init_nonce
    std::string session_key = session_hash(bob_key_half, alice_key_half);
    std::string aes_key = hexstr2str(session_key.substr(0,32));
    std::string aes_iv = hexstr2str(session_key.substr(32,32));
    std::string hmac_password = session_key.substr(64,32);
    std::string init_nonce = session_key.substr(96,32);

    printf("Here is the nonce: %s\n", init_nonce.c_str());

	return 0;
}