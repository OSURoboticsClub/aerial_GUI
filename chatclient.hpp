#ifndef CHATCLIENT_HPP
#define CHATCLIENT_HPP


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1000

/*****************************************************
* Function: get_in_addr
* Description: returns sockaddr, works for both IPv4 or Ipv6
* Taken from Beej's guide
******************************************************/
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/*****************************************************
* Function: send_all()
* Description: funcion used to send all data to socket
******************************************************/
bool send_all(int socket, void *buffer, size_t length){
    char *ptr = (char*) buffer;
    // printf("SERVER: send_all(): length: %d\n", length);
    while (length > 0){
      int i = send(socket, ptr, length, 0);
      // printf("SERVER: sent %d bytes\n", i);
      if (i < 1) return false;
      ptr += i;
      length -= i;
    }
    return true;
}


/*****************************************************
* Function: receive_all()
* Description: funcion used to receive all data from socket
******************************************************/
bool receive_all(int socket, void* buffer){
  char* ptr = (char*) buffer;
  size_t len = sizeof(buffer);
  ssize_t count;
  while (len > 0 && (count = recv(socket, ptr, len, 0)) > 0){
    ptr += count;
    len = len- (size_t)count;
    printf("stuck on loop\n");
  }
  if (len < 0 || count < 0){
    perror("SERVER: error in recv()\n");
  }
}



#endif // CHATCLIENT_HPP
