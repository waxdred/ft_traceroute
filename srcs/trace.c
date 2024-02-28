#include "../includes/ft_traceroute.h"

void help(char *s) {
  fprintf(stderr, "Usage: %s, ip_adress\n", s);
  return;
}

int8_t set_ttl() {
  // set ttl
  t_trace *trace = ft_trace(NULL);
  if (setsockopt(trace->sockfd, IPPROTO_IP, IP_TTL, &trace->ttl,
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
  bzero(&hint, sizeof(hint));
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
    strcpy(trace->ip, host);
  }
  freeaddrinfo(servinfo);
  return EXIT_SUCCESS;
}

void get_name_ip(char *ip) {
  t_trace *trace = ft_trace(NULL);
  struct sockaddr_in sa;

  memset(&sa, 0, sizeof(sa));
  memset(&trace->host, 0, sizeof(trace->host));
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = inet_addr(ip);
  if (getnameinfo((struct sockaddr *)&sa, sizeof(sa), trace->host,
                  sizeof(trace->host), NULL, 0, 0)) {
    fprintf(stderr, "Could not resolve hostname\n");
    exit(EXIT_FAILURE);
  }
}

int8_t openSocket() {
  t_trace *trace = ft_trace(NULL);
  struct timeval timeout = {1, 0};
  socklen_t len = sizeof(timeout);
  trace->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (trace->sockfd == -1) {
    fprintf(stderr, "ft_trace: error creation socket: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }
  trace->packetSize = sizeof(struct icmp) + sizeof(struct timeval);
  trace->packet = (char *)malloc(trace->packetSize);
  if (!trace->packet) {
    fprintf(stderr, "ft_trace: error allocation: %s\n", strerror(errno));
    return 2;
  }
  ft_memset(trace->packet, 0, sizeof(struct icmp));
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
  trace->dest_addr.sin_family = AF_INET;
  trace->dest_addr.sin_addr.s_addr = inet_addr(trace->ip);
  trace->icmp_header = (struct icmp *)trace->packet;
  trace->icmp_header->icmp_id = getpid() & 0xFFFF;
  trace->header(trace);
  return EXIT_SUCCESS;
}
