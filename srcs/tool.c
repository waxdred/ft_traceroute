#include "../includes/ft_traceroute.h"

unsigned short calculate_checksum(void *addr, size_t count) {
  unsigned short *ptr;
  unsigned long sum;

  ptr = addr;
  for (sum = 0; count > 1; count -= 2)
    sum += *ptr++;
  if (count > 0)
    sum += *(unsigned char *)ptr;
  while (sum >> 16)
    sum = (sum & 0xffff) + (sum >> 16);
  return (~sum);
}

void fill_seq_icmp() {
  t_trace *trace = ft_trace(NULL);
  trace->icmp_header->icmp_type = ICMP_ECHO;
  trace->icmp_header->icmp_code = 0;
  trace->icmp_header->icmp_id = getpid() & 0xFFFF;
  trace->icmp_header->icmp_seq = htons(trace->seq);
  ft_memset(trace->icmp_header->icmp_data, '*', trace->packetSize);
  trace->icmp_header->icmp_cksum = 0;
  trace->icmp_header->icmp_cksum = calculate_checksum(
      (unsigned short *)trace->icmp_header, trace->packetSize);
}
