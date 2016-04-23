#include "bob.h"

#include <stdio>

#include "../channel/channel_wrapper.h"
#include "../lib/utils.h"

int main(){

	// Initialize variables
	int port = 9999;

	// Host channel
	ChannelWrapper channel;

	if(channel.connect_to_server(port)){

		// TODO FIX THIS FOR BOB INSTEAD OF ALICE
		// Determine keys for each party
		// std::string alice_key = readRand(16);
		// std::string encrypted_alice_key = encryptWithBothKeys("bob.pub", "alice.key", alice_key);
		// server.send_data(str2hexstr(encrypted_alice_key));

		// std::string encrypted_bob_key = hexstr2str(server.recv_data());
		// std::string bob_key = decryptWithBothKeys("bob.pub", "alice.key", encrypted_bob_key);

		// std::string session_key = session_hash(alice_key, bob_key);
		// std::string aes_key = hexstr2str(session_key.substr(0,32));
		// std::string aes_iv = hexstr2str(session_key.substr(32,32));
		// std::string hmac_password = session_key.substr(64,32);
		// std::string init_nonce = session_key.substr(96,32);

		// server.set_keys(aes_key, aes_iv, hmac_password, init_nonce);
	}else{
		printf("Unable to connect to port %d\n", port);
	}


	return 0;
}