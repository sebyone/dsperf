#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <time.h>

#include "../dperf_local.hpp"

//DaaS Headers
#ifdef WITH_DAAS
#include "daas.hpp"
#include "daas_types.hpp"
#endif

void run_underlay_bandwidth_client(program_args_t *test, const char *server_ip, int server_port);
void run_underlay_bandwidth_server(int port);
void run_underlay_bandwidth_client_udp(program_args_t *args);
void run_underlay_bandwidth_server_udp(int port);

#ifdef WITH_DAAS
void run_overlay_bandwidth_client(daas_setup_t *setup, program_args_t *test);
void run_overlay_bandwidth_server(daas_setup_t *setup);
#endif
