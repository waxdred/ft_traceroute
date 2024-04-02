#include "../includes/ft_traceroute.h"
#include <stdint.h>

int8_t ft_send() {
  t_trace *trace = ft_trace(NULL);
  struct sockaddr_in dest;
  if (trace->proto == IPPROTO_ICMP)
    trace->header();
  switch (trace->proto) {
  case IPPROTO_ICMP:
    dest = trace->dest_addr;
    break;
  case IPPROTO_UDP:
    dest = trace->dest_addr_udp;
    break;
  }
  trace->set_ttl();
  if (sendto(trace->sock, trace->packet, trace->packetSize, 0,
             (struct sockaddr *)&dest, sizeof(dest)) <= 0) {
    fprintf(stderr, "ft_trace: error sending packet: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int8_t ft_receive(t_query *q) {
  t_trace *trace = ft_trace(NULL);
  int8_t ret = 0;
  char buf[1024];
  struct timeval end;
  struct sockaddr_in from;
  socklen_t fromlen = sizeof(from);
  char srcIP[INET_ADDRSTRLEN], dstIP[INET_ADDRSTRLEN];

  ft_bzero(buf, 1024);
  ft_bzero(srcIP, INET_ADDRSTRLEN);
  ft_bzero(dstIP, INET_ADDRSTRLEN);

  ret = recvfrom(trace->sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&from,
                 &fromlen);
  if (ret == -1) {
    if (q->count == 0 && trace->ttl < 10) {
      printf(" %d * ", trace->ttl);
    } else if (q->count == 0 && trace->ttl) {
      printf("%d * ", trace->ttl);
    } else if (q->count > 0 && q->count < q->queryMax) {
      printf(" * ");
    }
    if (q->count == q->queryMax - 1) {
      printf("\n");
    }
    return EXIT_SUCCESS;
  }
  gettimeofday(&end, NULL);
  struct ip *ip = (struct ip *)buf;
  double data = (double)(end.tv_usec - trace->tv.tv_usec) / 1000;

  inet_ntop(AF_INET, &(ip->ip_src), srcIP, INET_ADDRSTRLEN);
  inet_ntop(AF_INET, &(ip->ip_dst), dstIP, INET_ADDRSTRLEN);
  trace->getinfo(srcIP);
  if (q->count == 0) {
    if (trace->ttl < 10)
      printf(" %d %s (%s) %.3lf ms ", trace->ttl, trace->host, srcIP, data);
    else
      printf("%d %s (%s) %.3lf ms ", trace->ttl, trace->host, srcIP, data);
  } else {
    if (q->host != NULL && ft_strcmp(trace->host, q->host) == 0) {
      printf("%.3lf ms ", data);
    } else {
      printf("%s (%s) %.3lf ms ", trace->host, srcIP, data);
    }
  }
  if (q->count == q->queryMax - 1) {
    printf("\n");
  }
  q->host = trace->host;
  if (q->count == trace->query - 1 && ft_strcmp(srcIP, trace->ip) == 0) {
    return EXIT_RECV_DONE;
  }
  trace->seq++;
  trace->seqRecv++;
  return EXIT_SUCCESS;
}

void ft_traceroute() {
  t_trace *trace = ft_trace(NULL);
  int done;
  done = 0;
  trace->signalStop = 1;
  signal(SIGINT, handle_signal);
  fprintf(stdout, "ft_traceroute to %s (%s), %d hops max, %d byte packets\n",
          trace->hostname, trace->ip, trace->hopmax, trace->packetSize);
  for (; trace->signalStop && trace->ttl <= trace->hopmax;) {
    int err = 0;
    t_query info;
    ft_bzero(&info, sizeof(t_query));
    info.ttl = trace->ttl;
    info.queryMax = trace->query;
    for (int i = 0; i < trace->query; i++) {
      if (trace->proto == IPPROTO_UDP) {
        SetPortUdp(trace, trace->port + i);
      }
      usleep(100);
      gettimeofday(&trace->tv, NULL);
      info.count = i;
      if (trace->send()) {
        break;
      }
      err = trace->recv(&info);
      if (err == EXIT_RECV) {
        break;
      } else if (err == EXIT_RECV_DONE) {
        i = trace->query;
        done = 1;
        break;
      }
    }
    trace->ttl++;
    if (done == 1) {
      break;
    }
  }
}
