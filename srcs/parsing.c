#include "../includes/ft_traceroute.h"

static void ft_getopt(char *av, char *flag, int *opt) {
  const char *optchar;

  if (av[0] != '-') {
    *opt = -1;
    return;
  } else if (av[0] == '-' && av[2] != '\0') {
    *opt = -2;
    fprintf(stderr, "ft_ping: invalid arguments: '%s'\n", av);
    return;
  }
  *opt = av[1];
  optchar = ft_strchr(flag, *opt);
  if (optchar == NULL) {
    *opt = '?';
    return;
  }
}

static int ft_check(t_trace *trace, int *i, char **av, char opt, int ac) {
  trace->flag.verbose.ok = 0;
  switch (opt) {
  case '?':
    trace->help(av[0]);
    trace->free();
    exit(0);
  case 'h':
    trace->help(av[0]);
    trace->free();
    exit(0);
  case 'I':
    trace->proto = IPPROTO_ICMP;
    break;
  case 'f':
    if (*i + 1 >= ac) {
      fprintf(stderr, "ft_trace: option requires an argument -- 'f'\n");
      return EXIT_FAILURE;
    }
    int tll = ft_atoi(av[*i + 1]);
    if (tll <= 0 || tll > MAX_HOPS) {
      if (tll == 0) {
        fprintf(stderr, "ft_trace: ttl cannot be 0\n");
      } else {
        fprintf(stderr, "ft_trace: ttl cannot be more than %d\n", MAX_HOPS);
      }
      return EXIT_FAILURE;
    }
    trace->ttl = tll;
    *i += 1;
    break;
  case 'm':
    if (*i + 1 >= ac) {
      fprintf(stderr, "ft_trace: option requires an argument -- 'm'\n");
      return EXIT_FAILURE;
    }
    int maxttl = ft_atoi(av[*i + 1]);
    if (maxttl <= 0 || maxttl > MAX_HOPS) {
      if (maxttl == 0) {
        fprintf(stderr, "ft_trace: max hops cannot be 0\n");
      } else {
        fprintf(stderr, "ft_trace: max hops cannot be more than %d\n",
                MAX_HOPS);
      }
      return EXIT_FAILURE;
    }
    trace->hopmax = maxttl;
    *i += 1;
    break;
  case 'p':
    if (*i + 1 >= ac) {
      fprintf(stderr, "ft_trace: option requires an argument -- 'p'\n");
      return EXIT_FAILURE;
    }
    int port = ft_atoi(av[*i + 1]);
    if (port <= 0 || port > MAXPORT) {
      if (port == 0) {
        fprintf(stderr, "ft_trace: port cannot be 0\n");
      } else {
        fprintf(stderr, "ft_trace: port cannot be more than %d\n", MAXPORT);
      }
      return EXIT_FAILURE;
    }
    trace->port = port;
    break;
  case 'q':
    if (*i + 1 >= ac) {
      fprintf(stderr, "ft_trace: option requires an argument -- 'q'\n");
      return EXIT_FAILURE;
    }
    int val = ft_atoi(av[*i + 1]);
    if (val <= 0 || val > MAXQUERY) {
      if (val == 0) {
        fprintf(stderr, "ft_trace: query cannot be 0\n");
      } else {
        fprintf(stderr, "ft_trace: query cannot be more than %d\n", MAXQUERY);
      }
      return EXIT_FAILURE;
    }
    trace->query = val;
    *i += 1;

    break;
  }
  return EXIT_SUCCESS;
}

int parse(int ac, char **av) {
  t_trace *trace = ft_trace(NULL);
  int opt;
  int i;

  opt = 0;
  i = 1;
  while (i < ac) {
    if (ft_strcmp(av[i], "--icmp") == 0) {
      trace->proto = IPPROTO_ICMP;
    } else if (ft_strcmp(av[i], "--help") == 0) {
      trace->help(av[0]);
      trace->free();
      exit(0);
    } else {
      ft_getopt(av[i], "I?hqmfp", &opt);
      if (opt == -2) {
        return EXIT_FAILURE;
      }
      if (ft_check(trace, &i, av, opt, ac) == EXIT_FAILURE) {
        return EXIT_FAILURE;
      } else if (opt == -1) {
        if (trace->hostname == NULL) {
          trace->hostname = av[i];
        }
      }
    }
    ++i;
  }
  return EXIT_SUCCESS;
}
