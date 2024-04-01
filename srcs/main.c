#include "../includes/ft_traceroute.h"

int main(int ac, char *av[]) {
  t_trace *trace;

  trace = initTrace();
  if (!trace)
    return EXIT_FAILURE;
  if (ac < 2) {
    trace->help(av[0]);
    trace->free();
    exit(EXIT_FAILURE);
  }

  if (trace->parse(ac, av)) {
    trace->free();
    return EXIT_FAILURE;
  }
  int err = 0;
  if ((err = trace->connection()) != 0) {
    trace->free();
    return EXIT_FAILURE;
  }
  trace->run();
  trace->free();
  return (0);
}
