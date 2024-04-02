#include "../includes/ft_traceroute.h"
#include <netinet/udp.h>

void help(char *s) {
  fprintf(stderr, "Usage: %s, ip_adress\n", s);
  fprintf(stderr, "  -m maxttl\n");
  fprintf(stderr, "  -p port\n");
  fprintf(stderr, "  -q query\n");
  fprintf(stderr, "  -f ttl\n");
  fprintf(stderr, "  -I icmp\n");
}

int8_t set_ttl() {
  // set ttl
  t_trace *trace = ft_trace(NULL);
  if (setsockopt(trace->sock, IPPROTO_IP, IP_TTL, &trace->ttl,
                 sizeof(trace->ttl)) == -1) {
    fprintf(stderr, "ft_trace: error set setsockopt ttl: %s\n",
            strerror(errno));
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int host_to_ip() {
  t_trace *trace = ft_trace(NULL);
  char host[100];
  struct addrinfo hint;
  struct addrinfo *servinfo, *tmp;
  ft_bzero(&hint, sizeof(hint));
  hint.ai_family = AF_INET;

  int recv = getaddrinfo(trace->hostname, NULL, &hint, &servinfo);
  if (recv < 0) {
    perror("ft_trace: error getaddrinfo");
    return EXIT_FAILURE;
  } else if (servinfo == NULL) {
    fprintf(stderr, "ft_trace: cannot resolve: %s: Unknown host\n",
            trace->hostname);
    return EXIT_FAILURE;
  }
  for (tmp = servinfo; tmp != NULL; tmp = tmp->ai_next) {
    getnameinfo(tmp->ai_addr, tmp->ai_addrlen, host, sizeof(host), NULL, 0,
                NI_NUMERICHOST);
    ft_strcpy(trace->ip, host);
  }
  freeaddrinfo(servinfo);
  return EXIT_SUCCESS;
}

void get_name_ip(char *ip) {
  t_trace *trace = ft_trace(NULL);
  struct sockaddr_in sa;

  ft_memset(&sa, 0, sizeof(sa));
  ft_memset(&trace->host, 0, sizeof(trace->host));
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = inet_addr(ip);
  if (getnameinfo((struct sockaddr *)&sa, sizeof(sa), trace->host,
                  sizeof(trace->host), NULL, 0, 0)) {
    fprintf(stderr, "Could not resolve hostname\n");
    exit(EXIT_FAILURE);
  }
}

void SetPortUdp(t_trace *trace, int port) {
  trace->dest_addr_udp.sin_port = htons(port);
}

int8_t openSocket() {
  t_trace *trace = ft_trace(NULL);
  struct timeval timeout = {1, 0};
  socklen_t len = sizeof(timeout);
  trace->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  trace->sockfd_udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (trace->sockfd == -1 || trace->sockfd_udp == -1) {
    fprintf(stderr, "ft_trace: error creation socket: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }
  switch (trace->proto) {
  case IPPROTO_ICMP:
    trace->packetSize = sizeof(struct icmp) + sizeof(struct timeval);
    trace->packetSize = PACKET_SIZE;
    trace->sock = trace->sockfd;
    break;
  case IPPROTO_UDP:
    trace->packetSize = sizeof(struct udphdr) + sizeof(struct timeval);
    trace->packetSize = PACKET_SIZE;
    trace->sock = trace->sockfd_udp;
    break;
  }
  trace->packet = (char *)malloc(PACKET_SIZE);
  if (!trace->packet) {
    fprintf(stderr, "ft_trace: error allocation: %s\n", strerror(errno));
    return 2;
  }
  ft_memset(trace->packet, 0, trace->packetSize);
  trace->alloc = 1;
  if (trace->getname()) {
    return EXIT_FAILURE;
  }
  // set time out for receive
  if (setsockopt(trace->sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, len) < 0) {
    fprintf(stderr, "ft_trace: error set setsockopt timeout: %s\n",
            strerror(errno));
    return EXIT_FAILURE;
  }
  switch (trace->proto) {
  case IPPROTO_ICMP:
    trace->dest_addr.sin_family = AF_INET;
    trace->dest_addr.sin_addr.s_addr = inet_addr(trace->ip);
    trace->icmp_header = (struct icmp *)trace->packet;
    trace->icmp_header->icmp_id = getpid() & 0xFFFF;
    break;
  case IPPROTO_UDP:
    // set dest for udp
    trace->dest_addr_udp.sin_family = AF_INET;
    SetPortUdp(trace, DEST_PORT);
    trace->dest_addr_udp.sin_addr.s_addr = inet_addr(trace->ip);
    break;
  }
  if (trace->proto == IPPROTO_ICMP)
    trace->header();
  return EXIT_SUCCESS;
}
