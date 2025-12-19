#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>

#include "formatting.h"
#include "config.h"
#define BUFFER_SIZE 1024

int open_config(config *cfg, char *filename){
  char buffer[BUFFER_SIZE];
  int file_fd = open(filename, O_RDONLY);
  ssize_t bytes_read = 0;

  if(file_fd < 0)
    return file_fd;

  bytes_read = read(file_fd, buffer, BUFFER_SIZE-1);

  if(bytes_read < 0)
    return bytes_read;

  buffer[bytes_read] = 0;

  //split each line
  char *tok = strtok(buffer, "\n");
  while(tok){
    //find '=' delimeter (non destructive)
    char *delim = tok;
    while(*delim != 0 && *delim != '=')
      delim++;

    if(*delim == 0){
      printf("invalid token \"%s\"\n", tok);
      tok = strtok(NULL, "\n");
      continue;
    }

    //DRY final boss
    *delim = 0;
    if(strcmp(tok, "my_domain")==0){
      cfg->my_domain = malloc(strlen(++delim));
      strcpy(cfg->my_domain, delim);
    }
    else if(strcmp(tok, "from")==0){
      cfg->from = malloc(strlen(++delim));
      strcpy(cfg->from, delim);
    }
    else if(strcmp(tok, "to")==0){
      cfg->to = malloc(strlen(++delim));
      strcpy(cfg->to, delim);
    }
    else if(strcmp(tok, "peer_domain")==0){
      cfg->peer_domain = malloc(strlen(++delim));
      strcpy(cfg->peer_domain, delim);
    }
    else if(strcmp(tok, "port")==0){
      cfg->port = malloc(strlen(++delim));
      strcpy(cfg->port, delim);
    }
    else{
      printf("unrecognised directive \"%s\"\n", tok);
    }

    tok = strtok(NULL, "\n");
  }
  return 0;
}
