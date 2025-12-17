#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "connections.h"

int ping(){
  const char basic_req[]= "GET / HTTP/1.1\r\nHost: sam-bonnekamp.com\r\nConnection: close\r\n";
  char res[1024];
  int sock = connect_to_service("sam-bonnekamp.com", "80");
  if(sock < 0){
    perror("couldn't connect to server");
    return -1;
  }
  ssize_t bytes_written = write(sock, basic_req, sizeof(basic_req));
  if(bytes_written < 0){
    perror("couldn't write");
    return -1;
  }
  ssize_t bytes_read = read(sock, res, 1023);
  if(bytes_read < 0){
    perror("couldn't read");
    return -1;
  }
  res[bytes_read] = 0;
  fputs(res, stdout);
  fflush(stdout);
  return 0;
}
