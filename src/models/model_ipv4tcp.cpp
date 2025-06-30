
#include "model_ipv4tcp.h"

#if defined(__linux__) || defined(__RASP__) || defined(__MINGW64__)

#include <unistd.h>      // Posix API ( fopen, close etc.etc. )
#include <sys/ioctl.h>   // does not declare the ioctl function on some platforms: AIX 5.1, Solaris 11.4, Haiku 2017
#include <sys/types.h>   // defines a collection of typedef symbols and structures
#include <sys/socket.h>  // Macros and structures to use sockets
#include <netinet/in.h>  // Definitions for the internet protocol family
#include <netinet/tcp.h> // defines macros for use as a socket option
#include <arpa/inet.h>

#elif defined(__windows__)

#include <winsock.h>
#include <winsock2.h>
#include <sys/types.h>

#endif

#include <math.h>
#include <string.h>
#include <stdio.h>

#include "../helpers/timers.h"
#include "../helpers/datetime.h"
#include "../hwinterfaces.h"

// extern options_t Options; // options_t Options;

// ipv4_tcp
#define PACKET_BUFFER_MAX_SIZE 2048
//
#define MAX_LINE_LEN 256
#define MAX_LINKS 5
#define MAX_REMOTE_LINKS 64
#define LINK_MAX_VAL 6

#define _MODEL_IPV4 1

#define IPV4_MIN_MSS 256
#define IPV4_MAX_MSS 1460

#define IPV4_MIN_SPORT 0
#define IPV4_MAX_SPORT 65535
#define IPV4_DEF_SPORT 3000

#define _MODEL_DAAS 2

#define MIN_BLOCK_SIZE 1
#define MAX_BLOCK_SIZE _1GB // 1GB

static model_info_t ipv4tcp = {   // Model defines
    _PROTO_IPV4,                  // protocol
    __Capacity,                   // model class
    2,                            // model ID
    "IPv4/TCP",                   // Title
    "Throughput/Bandwidth v.01a", // Description
    "developers@sebyone.it"};     // Authors

capacity_vars_t vars = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // values for enumerated capacity_keys

typedef struct // Specific model settings
{

    model_info_t *pmodel;
    phwif_t hwi;      // name as OS reported "eth", "wlan", ...
    double bandwidth; // nominal Speed: 50000Mb/s

    hrole_t mode; // run mode

    unsigned port;
    sockaddr_in local_ip;
    sockaddr_in remote_ip; // Client mode only

    unsigned pkt_mss; // mss size

} settings_ipv4_t;

static settings_ipv4_t env;

void setDefaultEnv()
{
    memset(&env, 0, sizeof(settings_ipv4_t));

    env.pmodel = &ipv4tcp;                          // Model defines
    env.hwi = NULL;                                 // name as OS reported "eth", "wlan", ...
    env.bandwidth = 0;                              // nominal Speed: 50000Mb/s
    env.mode = _ROLE_NONE;                          // run mode
    env.port = IPV4_DEF_SPORT;                      // default service port
    memset(&env.local_ip, 0, sizeof(sockaddr_in));  // Posix IP struct
    memset(&env.remote_ip, 0, sizeof(sockaddr_in)); // Posix IP struct
    env.local_ip.sin_port = env.port;               // default port setted
    env.remote_ip.sin_port = env.port;              // default port setted
    env.pkt_mss = 0;                                // negotiation disabled
    sockaddr remote_ip;
}

// -------------------------------------------------------------------------------------------------------- !

ret_t set_env_ipv4tcp(options_t &ops_)
{
    setDefaultEnv();

    // if service num is 
    env.port = atoi(ops_.local_addr);
    if (!(env.port < IPV4_MIN_SPORT && env.port > IPV4_MAX_SPORT))
    {
        pverbose("error: invalid port number !\n");
        return rtExit;
    }

    if (ops_.host_role == _ROLE_SERVER)
    {
        // local if.
        env.local_ip = 0; // convert from options in ipv4 structure
        // sockaddr_in local_addr;
        // memset(&local_addr, 0, sizeof(sockaddr_in));
        env.local_ip.sin_family = AF_INET;
        env.local_ip.sin_port = htons(env.port);
        env.local_ip.sin_addr.s_addr = INADDR_ANY; // Set interface local_addr

        // bind

        // testing parameters
        // run mode
    }
    else // _ROLE_CLIENT
    {
        // remote if. ?
        // local if. ?
    };
}

// -------------------------------------------------------------------------------------------------------- !
// SERVER LOOPBACK
// -------------------------------------------------------------------------------------------------------- !
ret_t run_server_ipv4tcp(tclass_t tcls)
{
    char *buffer = (char *)malloc(PACKET_BUFFER_MAX_SIZE);
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (server_sock < 0)
    {
        pverbose("error: can't allocate Socket !\n");
        return rtErr;
    }

    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // sockaddr_in local_addr;
    // memset(&local_addr, 0, sizeof(sockaddr_in));
    // env.local_ip.sin_family = AF_INET;
    // env.local_ip.sin_port = htons(env.port);
    // env.local_ip.sin_addr.s_addr = INADDR_ANY; // Set interface local_addr

    if (bind(server_sock, (const sockaddr *)&env.local_ip, sizeof(sockaddr_in)) < 0)
    {
        pverbose("error: can't bind local address %s:%d !", env.local_ip, env.port); // convert ip to string
        close(server_sock);
        return rtErr;
    }

    if (listen(server_sock, 1) < 0) // Blocking mode
    {
        pverbose("error: fault listen socket !\n");
        close(server_sock);
        return rtErr;
    }

    pverbose("[TCP/IP] Listen on ipv4 port [%d]...\n", env.port);

    sockaddr peer_addr;
    socklen_t peer_addr_len = sizeof(peer_addr);

    int peer_sock;

    while (1)
    {
        peer_sock = accept(server_sock, &peer_addr, &peer_addr_len); // (https://man7.org/linux/man-pages/man2/accept.2.html)
        if (peer_sock < 0)
        {
            pverbose("error: refused incoming connection !\n");
            continue;
        }

        pverbose("event: accepted incoming connection [%s]\n", peer_addr.sa_data);

        if (env.)
            int curr_mss = 0;
        socklen_t curr_mss_len = sizeof(curr_mss);
        if (getsockopt(peer_sock, IPPROTO_TCP, TCP_MAXSEG, &curr_mss, &curr_mss_len) != 0)
        {
            pverbose("[TCP/IP] Error can't negotiate mss \n");
            return rtErr;
        }
        pverbose("[TCP/IP] Negotiated mss to [%d] bytes \n", curr_mss);

        ssize_t total_received = 0;
        double start = now_millis();

        while (1) // Until testing_time-out or connection closed !!!!!!!!!!!!!
        {
            ssize_t recvd = recv(peer_sock, buffer, sizeof(buffer), 0); // (https://man7.org/linux/man-pages/man2/recv.2.html)
            if (recvd <= 0)
                break; // when receive a packet with empty payload !!!!!!!!!!!!
            total_received += recvd;
        }

        double elapsed = now_millis() - start;
        double throughput = (elapsed > 0) ? (_Byte2Megabits(total_received) / (elapsed / 1000.0)) : 0;
        pverbose("Time %.6f [ms] | Data %d [Bytes] | Throughput %.3f [Mbps]\n", elapsed, total_received, throughput);

        free(buffer);
        shutdown(peer_sock, 2); //  2 - Stop both reception and transmission.
        // close(peer_sock);  //  If data waiting to be transmitted, close tries to complete this transmission (SO_LINGER).
    }
    shutdown(server_sock, 2); //  2 - Stop both reception and transmission.
                              // close(server_sock);
    return rtOk;
}

// -------------------------------------------------------------------------------------------------------- !
// CLIENT
// -------------------------------------------------------------------------------------------------------- !
ret_t run_client_ipv4tcp(tclass_t tcls)
{
    if (strlen(Options.local_addr) > 0) // (https://man7.org/linux/man-pages/man3/getaddrinfo.3.html)
    {
        if (inet_network(Options.local_addr) <= 0)
        {
            pverbose("[CLIENT] Invalid IPv4 address: '%s'\n", Options.local_addr);
            return rtErr;
        }
    }

    if (Options.service_num < IPV4_MIN_SPORT || Options.service_num > IPV4_MAX_SPORT)
    {
        pverbose("[CLIENT] Invalid Service Port number: '%d'\n", Options.service_num);
        return rtErr;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0); // Create socket IPv4/TCP (SOCK_STREAM)
    if (sock < 0)
    {
        pverbose("Can't CREATE socket error!");
        return (rtErr);
    }

#if (0) // --- Set mss
    mss = 576;
    if (setsockopt(sock, IPPROTO_TCP, TCP_MAXSEG, &mss, sizeof(mss)) != 0)
    {
        pverbose("Can't get MSS!");
        return (rtErr);
    }

    // Read back the MSS value.
    int curr_mss;
    socklen_t curr_mss_len = sizeof(curr_mss);
    if (getsockopt(lsock, IPPROTO_TCP, TCP_MAXSEG, &curr_mss, &curr_mss_len) < 0)
    {
        perror(0);
        return 1;
    }

    printf("mss: %d\n", curr_mss);
#endif // ---

    sockaddr server_addr;
    server_addr.sa_family = AF_INET;
    inet_pton(server_addr.sa_family, Options.local_addr, &server_addr.sa_data);
    if (connect(sock, &server_addr, sizeof(sockaddr)) < 0)
    {
        pverbose("Can't CONNECT socket error!");
        close(sock);
        return (rtErr);
    }

    if (!Options.csv_format) // verbose mode options !!!
    {
        pverbose("dsperf started in client mode, loopback at  %s:%d \n", Options.local_addr, Options.service_num); // with %s size %d\n", ip, service_port, "block", test->block_size);
    }

    // Packet length settings (socket MSS negotiation !!!)

    socklen_t optlen = vars[_pktpayload]; // mss = PAYLOAD !!!!!!!!!!!!!
    // ip_opts optval;

    if (!Options.pkt_payload > 0) // .mss_specified
    {
        getsockopt(sock, IPPROTO_TCP, TCP_MAXSEG, &vars[_pktpayload], &optlen);
        vars[_pktheader] = 40;
    }
    else
    {
        // Force MSS negotiation !!!!!!!!!!!
        vars[_pktpayload] = 1400; // DA SISTEMARE !!!!!!!!!!!!!!!!!!!!
        vars[_pktheader] = 40;
    }

    /// computes number of packets will be sended and protocol efficiency
    if ((size_t)vars[_pktpayload] >= Options.block_size)
    {
        vars[_pktstosend] = 1;
    }
    else
    {
        vars[_pktstosend] = trunc(Options.block_size / (size_t)vars[_pktpayload]) + (double)(Options.block_size % (size_t)vars[_pktpayload]);
    }
    vars[_efficiency] = vars[_pktpayload] / (vars[_pktpayload] + vars[_pktheader]) * 100.0;

    // Allocates a packet's size buffer
    const size_t buffersize = (size_t)vars[_pktpayload];
    char *packet = (char *)malloc(buffersize);
    memset(packet, 'A', buffersize);

    // Performs one or many tests...
    if (Options.csv_format && !Options.csv_no_header)
    {
        report_capacity(env.pmodel, vars, _OUTS_CSV_HEADER);
    }

    vars[_tstcounter] = 0;

    while (vars[_tstcounter]++ < Options.repetitions)
    {
        vars[_pktssent] = 0;
        ssize_t bytes2send = (ssize_t)vars[_blocksize];

        // double start_time = get_time_microseconds();
        double start_time = now_millis();
        while (bytes2send)
        {
            if (bytes2send >= (ssize_t)vars[_pktpayload])
            {
                bytes2send -= send(sock, packet, (size_t)(vars[_pktpayload]), 0); // (https://man7.org/linux/man-pages/man2/send.2.html)
            }
            else
            {
                bytes2send -= send(sock, packet, (size_t)bytes2send, 0);
            }
            vars[_pktssent]++;
        }

        // Results vars
        // vars[_ttime] = (get_time_microseconds() - start_time) / 1000; // ms
        vars[_ttime] = (now_millis() - start_time); // ms
        vars[_datasent] = vars[_blocksize];
        vars[_throughput] = _Byte2Megabits(vars[_datasent]) / (vars[_ttime] * 1000); // [Mbps]

        // Bandwidth ?????????????????????????
        // const uint64_t target_bitrate_bps = 10 * 1024 * 1024 * 8; // 10 Mbps (Megabits) _10Mps

        // Prints out vars
        report_capacity(env.pmodel, vars, Options.csv_format ? _OUTS_SUMMARY : _OUTS_CSV_ROW);
    }

    free(packet);
    close(sock);
    return rtOk;
}
