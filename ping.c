#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "connections.h"

int ping(char *fqdn){
  const char basic_req[]= "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n";
  char res[1024];
  int bytes_printed = snprintf(res, 1024, basic_req, fqdn);
  ssize_t bytes_written, bytes_read;
  int sock = connect_to_service(fqdn, "80");
  
  if(sock < 0){
    perror("couldn't connect to server targeted");
    return -1;
  }
  
  bytes_written = write(sock, res, bytes_printed);
  if(bytes_written < 0){
    perror("couldn't write to targeted server");
    return -1;
  }
  
  bytes_read = read(sock, res, 1023);
  if(bytes_read < 0){
    perror("couldn't read from targeted server");
    return -1;
  }
  /*
  res[bytes_read] = 0;
  fputs(res, stdout);
  fflush(stdout);
  */
  return 0;
}
