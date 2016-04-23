#include <stdio>

#include "alice.h"
#include "../channel/channel_wrapper.h"
#include "../lib/utils.h"

int main(){

	// Initialize variables
	int port = 9999;

	// Host channel
	ChannelWrapper channel;

	if(channel.start_server(port)){
	  	while (1){
	    	
	    	// Someone has connected 
	  	}
	}else{
		printf("Error starting server on port %d\n", port);
	}

	return 0;
}