#ifndef TCP_CPP
#define TCP_CPP

#include "common.h"
#include "tcp.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

bool TCP::connect_to_server(int port){
  if (socketfd<0){
    socketfd=socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd<0){
      return false;
    }
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    return connect(socketfd, (sockaddr *)&server, sizeof(server));
    //TODO key exchange
    //take argument saying whether to do key exchange so that the proxy can still use the function
  }else{
    return false;
  }
}

bool TCP::start_server(int port){
  if (socketfd<0){
    socketfd=socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd<0){
      return false;
    }
    struct sockaddr_in server;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if(bind(socketfd, (sockaddr *)&server, sizeof(server))==0){
      listen(socketfd, 5);
      return true;
    }
  }
  return false;
}

 TCP TCP::accept_client(){
  int connfd = accept(socketfd, NULL, NULL);
  //TODO key exchange
  return TCP(connfd);
}

bool TCP::connected(){
  return socketfd>=0;
}

void TCP::disconnect(){
  if (socketfd>=0){
    close(socketfd);
    socketfd = -1;
  }
}

void TCP::set_keys(std::string aes_key, std::string aes_iv, std::string hmac_password, std::string init_nonce){
  memcpy(this->aes_key, aes_key.c_str(), 16);
  memcpy(this->aes_iv, aes_iv.c_str(), 16);
  this->hmac_password = hmac_password;
  this->my_nonce = init_nonce;
  this->their_nonce = init_nonce;
}

/* Nonce stuff */
//generates random nonce
void TCP::generateMyNonce(){
  std::string random = str2hexstr(readRand(16));
  my_nonce = random;
}
//retrieves random nonce
std::string TCP::getMyNonce(){
  return my_nonce;
}
//sets the stored random nonce
void TCP::setTheirNonce(std::string n){
  their_nonce = n;
}
//gets the stored random nonce
std::string TCP::getTheirNonce(){
  return their_nonce;
}

bool TCP::send_data(std::string data){
  if(socketfd>=0){
    if (send(socketfd, data.c_str(), data.length(), 0)>=0){
      return true;
    }else{
      disconnect();
    }
  }
  return false;
}

bool TCP::send_hmac(std::string data){
  data += "|"+hmac_hash(hmac_password, data);
  return send_data(data);
}

bool TCP::send_aes(std::string data){
  return send_hmac(str2hexstr(aesEncrypt(data, aes_key, aes_iv)));
}

bool TCP::send_nonce(std::string data){
  generateMyNonce();
  return send_aes(str2hexstr(data+"|"+my_nonce+"|"+their_nonce));
}

bool TCP::send_encrypted(std::string data){
  data.resize(512,0);
  return send_nonce(str2hexstr(data));
}

std::string TCP::recv_data(int num_bytes){
  char buffer[num_bytes+1];
  if (socketfd>=0){
    int l = recv(socketfd, buffer, num_bytes, 0);
    if (l>0){
      buffer[l]=0;
      return std::string(buffer);
    }else if (l==0){
      disconnect();
    }
  }
  return "";
}

std::string TCP::recv_hmac(int num_bytes){
  std::vector<std::string> data;
  split(recv_data(num_bytes), "|", data);

  if(data.size()>=2 && hmac_hash(hmac_password, data[0])==data[1]){
    return data[0];
  }else{
    return "";
  }

}

std::string TCP::recv_aes(int num_bytes){
  std::string data = hexstr2str(recv_hmac(num_bytes));
  if (data!=""){
    data = aesDecrypt(data, aes_key, aes_iv);
    return data.substr(0,data.size()-1);
  }else{
    return data;
  }

}

std::string TCP::recv_nonce(int num_bytes){
  std::vector<std::string> data;
  split(hexstr2str(recv_aes(num_bytes)), "|", data);
  if (data.size()>=3 && data[2]==my_nonce){
    their_nonce = data[1];
    return data[0];
  }else{
    return "";
  }

}

std::string TCP::recv_encrypted(int num_bytes){
  return hexstr2str(recv_nonce(num_bytes));
}

#endif
