#ifndef FT_TRACEROUTE_H
#define FT_TRACEROUTE_H

#include "../libft/include/libft.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct s_trace {
  char *hostname;
  char host[1046];
  char *packet;
  char ip[100];

  int alloc;
  int packetSize;
  int seqRecv;
  int sockfd;
  int ttl;
  int verose;
  uint16_t seq;

  struct icmp *icmp_header;
  struct sockaddr_in dest_addr;
  struct timeval tv;

  void (*help)(char *);
  void (*free)(struct s_trace *);
  int (*getname)(struct s_trace *);
  void (*getinfo)(struct s_trace *, char *ip);
  void (*header)(struct s_trace *);
  int8_t (*connection)(struct s_trace *);
  void (*run)(struct s_trace *);
  int8_t (*send)(struct s_trace *);
  int8_t (*recv)(struct s_trace *);
  int8_t (*set_ttl)(struct s_trace *);
} t_trace;

t_trace *initTrace(int ac, char **av);

#endif
