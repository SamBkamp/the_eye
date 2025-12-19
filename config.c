#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>

typedef struct{
  char *my_domain;
  char *from;
  char *to;
  char *peer_domain;
  char *port;
}config;

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

    *delim = 0;
    if(strcmp(tok, "my_domain")==0){
      cfg->my_domain = malloc(strlen(++delim));
      strcpy(cfg->my_domain, delim);
    }
    else if(strcmp(tok, "from")==0){
      cfg->from = malloc(strlen(++delim));
      strcpy(cfg->from, delim);
    }

    tok = strtok(NULL, "\n");
  }

}


int main(int argc, char* argv[]){
  config cfg;
  open_config(&cfg, "config.cfg");
  printf("domain: %s\n", cfg.my_domain);
  printf("from: %s\n", cfg.from);
}
