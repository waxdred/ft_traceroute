#include "../includes/ft_traceroute.h"

int main(int ac, char *av[]) {
  t_trace *trace;

  trace = initTrace(ac, av);
  if (!trace)
    return EXIT_FAILURE;
  if (ac != 2) {
    trace->help(av[0]);
    trace->free(trace);
    exit(EXIT_FAILURE);
  }
  int err = 0;
  if ((err = trace->connection(trace)) != 0) {
    trace->free(trace);
    return EXIT_FAILURE;
  }
  trace->run(trace);
  return (0);
}
