
#include "tester_ipv4tcp.h"

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

const model_info_t info = { // TESTER:
    _PROTO_IPV4,                   // protocol
    __Capacity,                    // model
    0,                             // variant
    "IPv4/TCP",                    // title
    "Throughput/Bandwidth v.01a",  // description
    "developers@sebyone.it"};      // author

// ipv4_tcp defines
#define PACKET_BUFFER_MAX_SIZE 2048

#define IPV4_MIN_MSS 256
#define IPV4_MAX_MSS 1460

#define IPV4_MIN_SPORT 0
#define IPV4_MAX_SPORT 65535
#define IPV4_DEF_SPORT 3000

// TESTER SETTINGS AND VARS
// -------------------------------------------------------------------------------------------------------- !

typedef struct // Specific ipv4 model settings
{
    // Parameters
    // hrole_t mode;
    unsigned repeat_n;    // test repetittion
    in_port_t port;       // IPv4 Port
    sockaddr_in local_ip; // Local hwif/IP
    unsigned local_if;
    sockaddr remote_ip; // Client mode only
    unsigned mss;       // mss size
    double bandwidth;
    //
} env_settings_ipv4_t;

static env_settings_ipv4_t env;

capacity_vars_t vars; // capacity_vars
// -------------------------------------------------------------------------------------------------------- !

void setDefaultEnv()
{
    memset(&env, 0, sizeof(env_settings_ipv4_t));

    // env.mode = _ROLE_NONE;                     // run mode
    env.repeat_n = 0;                          // run counter
    env.port = IPV4_DEF_SPORT;                 // default service port
    env.local_if = NULL;                       // uses specified interface
    env.local_ip.sin_family = AF_INET;         // Posix IP struct
    env.local_ip.sin_addr.s_addr = INADDR_ANY; // Set all locals
    env.local_ip.sin_port = htons(env.port);   // default port setted
    env.remote_ip.sa_family = AF_INET;         // Posix IP struct
    env.mss = 0;                               // =0 negotiate >0 fixed
    env.bandwidth = 0;                         // nominal Speed: 50000Mb/s
    env.mss = 0;
}

// -------------------------------------------------------------------------------------------------------- !
ret_t set_env_ipv4tcp(options_t &ops_)
{
    resetVars(vars);
    setDefaultEnv();

    if (ops_.service_num > 0)
    {
        env.port = atoi(ops_.local_addr);
    }
    else
    {
        env.port = IPV4_DEF_SPORT;
    }

    if (ops_.run_mode == _ROLE_SERVER)
    {

        // env.local_ip.sin_addr = 0; // convert from options in ipv4 structure
        // sockaddr_in local_addr;
        // memset(&local_addr, 0, sizeof(sockaddr_in));
        // env.local_ip.sin_family = AF_INET;

        // if interface specified set: local hwif
        env.local_ip.sin_addr.s_addr = INADDR_ANY; // Set interface local_addr

        if (inet_pton(env.local_ip.sin_family, ops_.local_addr, &env.local_ip.sin_addr) != 0)
        {
            pverbose("ipv4tcp: invalid local IP !\n"); // Error in adress !
            return rtErr;
        }

        // bind
        // testing parameters
        // run mode
    }
    else // _ROLE_CLIENT
    {

        //
        sockaddr server_addr;
        env.remote_ip.sa_family = AF_INET;
        inet_pton(env.remote_ip.sa_family, ops_.remote_addr, &env.remote_ip.sa_data);

        /*
        if (inet_network(ops_.local_addr) <= 0) // (https://man7.org/linux/man-pages/man3/getaddrinfo.3.html)
        {
            pverbose("ipv4tcp: invalid IPv4 address: '%s'\n", ops_.local_addr);
            return rtErr;
        }
        */
    }
    if (!(env.port < IPV4_MIN_SPORT && env.port > IPV4_MAX_SPORT))
    {
        pverbose("ipv4tcp: invalid port number: '%d' !\n", env.port);
        return rtErr;
    }

    env.local_ip.sin_port = htons(env.port);

    // Globals
    vars.blocksize = ops_.tst_block_size; // block-size mode
    vars.timeslot = ops_.tst_time_slot;   // timed mode
    vars.bandwidth = ops_.bandwidth;      // REference bandwidth
}

// -------------------------------------------------------------------------------------------------------- !
// LOOPBACK SERVER
// -------------------------------------------------------------------------------------------------------- !
ret_t run_server_ipv4tcp()
{
    char *buffer = (char *)malloc(PACKET_BUFFER_MAX_SIZE);
    int peer_sock;

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        pverbose("ipv4tcp: can't allocate Socket !\n");
        return rtErr;
    }

    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(server_sock, (const sockaddr *)&env.local_ip, sizeof(sockaddr_in)) < 0)
    {
        pverbose("ipv4tcp: can't bind local address \n"); // %s:%d !", env.local_ip, env.port); // convert ip to string
        close(server_sock);
        return rtErr;
    }
    if (listen(server_sock, 1) < 0)
    {
        pverbose("ipv4tcp: listen socket fault !\n");
        close(server_sock);
        return rtErr;
    }
    pverbose("ipv4tcp:Listen on ipv4 \n"); // [%s:%d]...\n" env.local_ip, env.port); !!!!!!!!!!!!!!!!!!!!!!!!!!!

    sockaddr peer_addr;
    socklen_t peer_addr_len = sizeof(peer_addr);

    while (1)
    {
        peer_sock = accept(server_sock, &peer_addr, &peer_addr_len); // (https://man7.org/linux/man-pages/man2/accept.2.html)
        if (peer_sock < 0)
        {
            pverbose("ipv4tcp: incoming connection from [%s] refused !\n", peer_addr);
            continue;
        }

        pverbose("ipv4tcp: incoming connection from [%s] accepted. \n", peer_addr.sa_data);
        if (env.mss > 0)
        {
            int curr_mss = 0;
            socklen_t curr_mss_len = sizeof(curr_mss);
            if (getsockopt(peer_sock, IPPROTO_TCP, TCP_MAXSEG, &curr_mss, &curr_mss_len) != 0)
            {
                pverbose("ipv4tcp: can't negotiate mss \n");
                continue;
            }
            pverbose("ipv4tcp: negotiated mss to [%d] bytes. \n", curr_mss);
        }

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
        pverbose("report: Time %.6f [ms] | Data %d [Bytes] | Throughput %.3f [Mbps]\n", elapsed, total_received, throughput);

        free(buffer);
        close(peer_sock); // If data waiting to be transmitted, close tries to complete this transmission (SO_LINGER).
    }
    shutdown(server_sock, 2); // 2 = stop both reception and transmission.
    return rtOk;
}

// -------------------------------------------------------------------------------------------------------- !
// TESTER
// -------------------------------------------------------------------------------------------------------- !
ret_t run_client_ipv4tcp()
{
    char *packet;
    int sock = socket(AF_INET, SOCK_STREAM, 0); // Create socket IPv4/TCP (SOCK_STREAM)

    if (sock < 0)
    {
        pverbose("ipv4tcp: can't CREATE socket !");
        return (rtErr);
    }

    if (env.mss > 0)
    {
        if (setsockopt(sock, IPPROTO_TCP, TCP_MAXSEG, &env.mss, sizeof(env.mss)) != 0)
        {
            pverbose("ipv4tcp: can't set MSS!");
            return (rtErr);
        }
    }

    if (connect(sock, &env.remote_ip, sizeof(env.remote_ip)) < 0)
    {
        pverbose("ipv4tcp can't CONNECT socket !");
        close(sock);
        return (rtErr);
    }

    pverbose("ipv4tcp: client mode, loopback at %s:%d \n", env.remote_ip, env.port); // with %s size %d\n", ip, service_port, "block", test->block_size);

    // Packet length settings (socket MSS negotiation !!!)
    /*
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
    */

    socklen_t curr_mss_len = sizeof(vars.pktpayload);
    if (getsockopt(sock, IPPROTO_TCP, TCP_MAXSEG, &vars.pktpayload, &curr_mss_len) < 0) // Read back the MSS value.
    {
        pverbose("ipv4tcp: can't read mss value !");
        return (rtErr);
    }
    pverbose("mss: %d \n", (size_t)vars.pktpayload);

    /// computes number of packets will be sended and protocol efficiency
    if (vars.pktpayload >= vars.blocksize)
    {
        vars.pktefficiency = 1;
    }
    else
    {
        vars.pktstosend = trunc(vars.blocksize / vars.pktpayload) + ((size_t)vars.blocksize % (size_t)vars.pktpayload);
    }
    vars.pktefficiency = vars.pktpayload / (vars.pktpayload + vars.pktheader) * 100.0;

    // Allocates a packet's size buffer
    const size_t buffersize = (size_t)(vars.pktpayload + vars.pktheader);
    packet = (char *)malloc(buffersize);
    memset(packet, 'A', buffersize);

    // Performs one or many tests...
    if (1) // (Options.csv_format && !Options.csv_no_header)
    {
        report_capacity(info, vars, _OUTS_CSV_HEADER);
    }

    vars.tstcounter = 0;

    while (vars.tstcounter++ < env.repeat_n)
    {
        vars.pktssent = 0;
        ssize_t bytes2send = (ssize_t)vars.blocksize;

        // double start_time = now_millis();
        vars.totaltime = now_millis();

        while (bytes2send)
        {
            if (bytes2send >= (ssize_t)vars.pktpayload)
            {
                bytes2send -= send(sock, packet, (size_t)(vars.pktpayload), 0); // (https://man7.org/linux/man-pages/man2/send.2.html)
            }
            else
            {
                bytes2send -= send(sock, packet, (size_t)bytes2send, 0);
            }
            vars.pktssent++;
        }

        // Results vars
        vars.totaltime = (now_millis() - vars.totaltime); // ms VERIFICARE !!!!!!!!!!!
        vars.datasent = vars.blocksize - bytes2send;
        vars.throughput = _Byte2Megabits(vars.datasent) / (vars.totaltime * 1000); // [Mbps]
        // Bandwidth ?????????????????????????

        // Prints out test vars
        report_capacity(info, vars, _OUTS_SUMMARY); // Options.csv_format ? _OUTS_SUMMARY : _OUTS_CSV_ROW);
    }

    free(packet);
    close(sock);
    return rtOk;
}
