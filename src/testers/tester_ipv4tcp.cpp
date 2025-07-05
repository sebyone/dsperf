
#include "tester_ipv4tcp.h"

#if defined(__linux__) || defined(__RASP__) || defined(__MINGW64__)

#include <unistd.h>      // Posix API ( fopen, close etc.etc. )
#include <sys/ioctl.h>   // does not declare the ioctl function on some platforms: AIX 5.1, Solaris 11.4, Haiku 2017
#include <sys/types.h>   // defines a collection of typedef symbols and structures
#include <sys/socket.h>  // Macros and structures to use sockets
#include <netinet/in.h>  // Definitions for the internet protocol family
#include <netinet/tcp.h> // defines macros for use as a socket option
#include <arpa/inet.h>
#include <netinet/in.h>

#elif defined(__windows__)

#include <winsock.h>
#include <winsock2.h>
#include <sys/types.h>

#endif

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

#include "../helpers/timers.h"
#include "../helpers/datetime.h"
#include "../hwinterfaces.h"

const model_info_t info = {       // TESTER:
    _PROTO_IPV4,                  // protocol
    __Capacity,                   // model
    0,                            // variant
    "IPv4/TCP",                   // title
    "Throughput/Bandwidth v.01a", // description
    "developers@sebyone.it"};     // author

// -------------------------------------------------------------------------------------------------------- !
static struct // Specific ipv4 parameters
{
    int repeat_n;       // test repetittion
    short int port;     // IPv4 Port
    sockaddr_in local;  // server mode local hwif/IP
    sockaddr rpeer;     // connected remote
    int local_hwif;     // local Interface !!!!!!!!!!!!!!!!!!
    sockaddr_in remote; // Client mode only
    unsigned mss;       // mss size
    double bandwidth;
    //
    bool _ocsv;
    bool _ocsvheader;
    bool _opkts;
} env; // env_settings_ipv4_t;

// static env_settings_ipv4_t env;

capacity_vars_t vars; // capacity_vars
// -------------------------------------------------------------------------------------------------------- !

void setDefaultEnv()
{
    memset(&env, 0, sizeof(env));
    // env.mode = _ROLE_NONE;               // run mode
    env.repeat_n = 1;                       // run counter
    env.port = IPV4_DEF_SPORT;              // default service port
    env.local_hwif = 0;                     // uses specified hardware
    env.local.sin_family = AF_INET;         // IP family
    env.local.sin_addr.s_addr = INADDR_ANY; // Set all locals
    env.local.sin_port = htons(env.port);   // default port setted
    env.rpeer.sa_family = AF_INET;          // IP family "AF_UNSPEC"
    env.rpeer.sa_data[0] = '\0';            // IP family

    env.remote.sin_family = AF_INET;         // IP family
    env.remote.sin_addr.s_addr = INADDR_ANY; // Set all locals
    env.remote.sin_port = htons(env.port);   // default port setted

    env.mss = 0;             // =0 negotiate >0 fixed
    env.bandwidth = 0;       // nominal Speed: 50000Mb/s
    env.mss = 0;             // packet payload
    env._ocsv = false;       // enable csv
    env._ocsvheader = false; // csv header
    env._opkts = false;      // output packet contents
}

// -------------------------------------------------------------------------------------------------------- !
rt_t set_env_ipv4tcp(options_t &ops_) // Set Tester Parameter
{
    resetVars(vars); // Reset Tester vars and Enviroment
    setDefaultEnv();

    if (ops_.service_num > 0) // Check range and Set IPv4 port number
    {
        if ((ops_.service_num < IPV4_MIN_SPORT) || (ops_.service_num > IPV4_MAX_SPORT))
        {
            pverbose("ipv4tcp: invalid port number: '%d' !\n", ops_.service_num);
            return rtErr;
        }
        env.port = ops_.service_num;
    }
    else // set default
    {
        pverbose("ipv4tcp: uses default service port: '%d' !\n", env.port);
    }

    // env.repeat_n
    env.local.sin_port = htons(env.port); // Service
    env.remote.sin_port = htons(env.port);

    vars.blocksize = (double)ops_.tst_block_size; // block-size mode
    vars.timeslot = (double)ops_.tst_time_slot;   // timed mode

    vars.bandwidth = (double)ops_.bandwidth; // Reference to bandwidth !!!!!!!

    env._ocsv = ops_.csv_enabled;
    env._ocsvheader = !ops_.csv_no_header;
    env._opkts = ops_.pktverbose; // Verbose mode

    in_addr_t ip_addr = 0;

    if (ops_.run_mode == _ROLE_SERVER)
    {
        if (strchr(ops_.local_addr, '*')) // Set default local addresses/interfaces
        {
            // TODO: get local address !
            // inet_pton(env.local_ip.sin_family, ops_.local_addr, &env.local_ip.sin_addr)
            pverbose("ipv4tcp: bind local default IPv4 addresses !\n");
        }
        else // Listen on specified address/interface
        {
            if ((ip_addr = inet_addr(ops_.local_addr)) == 0) // dot-notations to IP network address
            {
                pverbose("ipv4tcp: invalid IP %s !\n", ops_.local_addr); // Error in address !
                return rtErr;
            }
            env.local.sin_addr.s_addr = ip_addr; // pverbose("ipv4tcp: ready to start SERVER (%s:%d)\n", inet_ntoa(env.local.sin_addr), env.port);
        }
    }
    else // _ROLE_CLIENT
    {
        /* TODO: resolve hostname !!!!!!!!!!
        ------------------------------------
        ip_addr = inet_addr(&ops_.remote_addr[0]); // converts 'binary format' in  'dot-notation'
        char tmp_str[_MAX_STR_LEN];
        sprintf(tmp_str, "%s:%d", ops_.remote_addr, env.port);
        pverbose("tmp_str: '%s'\n", tmp_str);
        pverbose("ip_addr: '%s'\n", inet_ntoa(*((struct in_addr *)&ip_addr)));
        ip_addr = inet_addr(&tmp_str[0]); // converts 'dot-notation' in 'binary format'
        */

        if (inet_pton(AF_INET, ops_.remote_addr, &env.remote.sin_addr) == 0) // converts 'dot-notation' in 'binary format' (https://man7.org/linux/man-pages/man3/inet.3.html)
        {
            pverbose("ipv4tcp: invalid remote IP %s !\n", ops_.remote_addr); // Error in address !
            return rtErr;
        }
        // pverbose("ipv4tcp: ready to start CLIENT to (%s:%d)\n", inet_ntoa(env.remote.sin_addr), env.port);
    }
    return rtOk;
}

// -------------------------------------------------------------------------------------------------------- !
// LOOPBACK SERVER
// -------------------------------------------------------------------------------------------------------- !
rt_t run_server_ipv4tcp()
{
    ssize_t recvd, total_received;
    double start, elapsed, throughput;
    char *buffer;
    int opt, remote_sk = 0, local_sk = socket(env.local.sin_family, SOCK_STREAM, 0);

    if (local_sk < 0)
    {
        pverbose("ipv4tcp: can't allocate AF_INET, SOCK_STREAM !\n");
        return rtErr;
    }

    size_t curr_mss = 0;
    socklen_t len = sizeof(curr_mss);
    if (env.mss > 0) // Set mss
    {
        curr_mss = (size_t)env.mss;
        if (getsockopt(local_sk, IPPROTO_TCP, TCP_MAXSEG, &curr_mss, &len) != 0) // set mss !!!!!!!!!!!!!
        {
            pverbose("ipv4tcp: negotited mss socket option (TCP_MAXSEG) error [%d] !\n", errno);
        }
    }
    if (getsockopt(local_sk, IPPROTO_TCP, TCP_MAXSEG, &curr_mss, &len) == 0)
    {
        pverbose("ipv4tcp: packet payload (mss) %d [bytes] !\n", curr_mss);
    }
    else
    {
        pverbose("ipv4tcp: negotited mss socket option (TCP_MAXSEG) error [%d] !\n", errno);
    }

    if (setsockopt(local_sk, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != 0) // set reusable socket
    {
        pverbose("ipv4tcp: socket option (SO_REUSEADDR) error [%d]\n!", errno);
    }

    if (bind(local_sk, (struct sockaddr *)&env.local, sizeof(sockaddr)) < 0)
    {
        pverbose("ipv4tcp: can't bind local address \n"); // %s:%d !", env.local_ip, env.port); // convert ip to string
        close(local_sk);
        return rtErr;
    }

    if (listen(local_sk, 1) < 0)
    {
        pverbose("ipv4tcp: listen socket fault !\n");
        close(local_sk);
        return rtErr;
    }

    // -----------
    pverbose("ipv4tcp: listen on (%s:%d)\n", inet_ntoa(env.local.sin_addr), env.port);
    buffer = (char *)malloc(PACKET_BUFFER_MAX_SIZE);
    socklen_t peer_addr_len = sizeof(env.rpeer);
    while (1)
    {
        remote_sk = accept(local_sk, (struct sockaddr *)&env.remote, &peer_addr_len); // (https://man7.org/linux/man-pages/man2/accept.2.html)
        if (remote_sk < 0)
        {
            pverbose("ipv4tcp: incoming connection from [%s] refused !\n", env.rpeer.sa_data);
            continue;
        }
        pverbose("ipv4tcp: open connection remote [%s] \n", inet_ntoa(env.remote.sin_addr)); // inet_ntoa() ???

        // TODO: implement test protocol like fresbee !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        total_received = 0;

        start = now_millis();
        while (1) // TODO: until testing_time-out or connection closed !!!!!!!!!!!!!
        {
            recvd = recv(remote_sk, buffer, sizeof(buffer), MSG_DONTWAIT); // (https://man7.org/linux/man-pages/man2/recv.2.html)
            if (recvd > 0)
            {
                if (env._opkts) // verbose mode (https://man7.org/linux/man-pages/man3/cmsg.3.html)
                {
                    printf("New pkt header");
                    printf(buffer);
                    printf("\n");
                }
            }
            else
            {
                pverbose("ipv4tcp: socket read error [%d]\n!", errno); // error EAGAIN or EWOULDBLOCK.
                break;
            }
            total_received += recvd;
        }
        elapsed = now_millis() - start;

        throughput = (elapsed > 0) ? (_Byte2Megabits(total_received) / (elapsed / 1000.0)) : 0;
        close(remote_sk); // tries to complete pending transmission and close (SO_LINGER).
        pverbose("ipv4tcp: socket closed, connection time %.6f [ms], Bytes recvd %d [Bytes], throughput %.3f [Mbps] \n", elapsed, total_received, throughput);
    }
    shutdown(local_sk, 2); // 2 = stop both reception and transmission.
    free(buffer);
    // -----------

    return rtOk;
}

// -------------------------------------------------------------------------------------------------------- !
// CLIENT TESTER
// -------------------------------------------------------------------------------------------------------- !
rt_t run_client_ipv4tcp()
{
    char *packet;
    int rsk;
    ssize_t bytes2send;

    rsk = socket(env.rpeer.sa_family, SOCK_STREAM, 0); // Create socket IPv4/TCP (SOCK_STREAM) // AF_UNSPEC
    if (rsk < 0)
    {
        pverbose("ipv4tcp: can't CREATE socket !");
        return (rtErr);
    }

    /*
    if (env.mss > 0)
    {
        if (setsockopt(rsk, IPPROTO_TCP, TCP_MAXSEG, &env.mss, sizeof(env.mss)) != 0)
        {
            pverbose("ipv4tcp: can't set MSS!");
            return (rtErr);
        }
    }
    */

    // Retries............
    if (connect(rsk, (struct sockaddr *)&env.remote, sizeof(env.rpeer)) < 0) // (https://man7.org/linux/man-pages/man2/connect.2.html)
    {
        pverbose("ipv4tcp: socket connection error [%d]\n!", errno); // error EAGAIN or EWOULDBLOCK.
        close(rsk);
        return (rtErr);
    }

    pverbose("ipv4tcp: Tester client at loopback [%s:%d]\n", inet_ntoa(env.remote.sin_addr), env.port);

    // Packet length settings (socket MSS negotiation !!!)
    size_t curr_mss = 536;
    socklen_t len = sizeof(curr_mss);
    /*
    if (env.mss > 0) // Set mss
    {
        curr_mss = (size_t)env.mss;
        if (getsockopt(rsk, IPPROTO_TCP, TCP_MAXSEG, &curr_mss, &len) != 0) // set mss !!!!!!!!!!!!!
        {
            pverbose("ipv4tcp: negotited mss socket option (TCP_MAXSEG) error [%d] !\n", errno);
        }
    }

    if (getsockopt(rsk, IPPROTO_TCP, TCP_MAXSEG, &curr_mss, &len) == 0)
    {
        pverbose("ipv4tcp: packet payload (mss) %d [bytes] !\n", curr_mss);
    }
    else
    {
        pverbose("ipv4tcp: negotited mss socket option (TCP_MAXSEG) error [%d] !\n", errno);
    }

    */

    vars.pktpayload = 536;
    pverbose("ipv4tcp: use mss: %.0f \n", vars.pktpayload);

    if (vars.pktpayload >= vars.blocksize) /// computes number of packets will be sended and protocol efficiency
    {
        vars.pktstosend = 1;
    }
    else
    {
        vars.pktstosend = trunc(vars.blocksize / vars.pktpayload) + (long)vars.blocksize % curr_mss;
    }
    vars.pktefficiency = vars.pktpayload / (vars.pktpayload + vars.pktheader) * 100.0;

    if (env._ocsv && env._ocsvheader) // output csv
    {
        report_capacity(info, vars, _OUTS_CSV_HEADER);
    }

    // ------------- allocates a packet's size buffer
    const size_t buffersize = (size_t)(PACKET_BUFFER_MAX_SIZE); // (vars.pktpayload + vars.pktheader);
    packet = (char *)malloc(buffersize);
    memset(packet, 'A', buffersize);
    // report_capacity(info, vars, _OUTS_SUMMARY);

    vars.tstcounter = 0;
    ssize_t sent;
    while (vars.tstcounter++ < env.repeat_n) // Performs one or many tests...
    {
        vars.pktssent = 0;
        bytes2send = (ssize_t)vars.blocksize;

        vars.totaltime = now_millis();

        while (bytes2send > 0)
        {
            if (bytes2send >= curr_mss)
            {
                sent = send(rsk, packet, curr_mss, 0); // (https://man7.org/linux/man-pages/man2/send.2.html)
            }
            else
            {
                sent = send(rsk, packet, (size_t)bytes2send, 0);
            }
            if (sent < 0)
            {
                pverbose("ipv4tcp: socket closed  %d \n", sent);
            }
            bytes2send -= sent;
            vars.pktssent++;
        }

        // Update result vars
        vars.totaltime = (now_millis() - vars.totaltime); // ms VERIFICARE !!!!!!!!!!!
        vars.datasent = vars.blocksize - bytes2send;
        vars.throughput = _Byte2Megabits(vars.datasent) / (vars.totaltime * 1000); // [Mbps]
        vars.bandwidth = 0;                                                        // [Mbps]
        vars.saturation = 0;                                                       // [Mbps]
        vars.jitter = 0;

        if (env._ocsv) // Outputs test results
        {
            report_capacity(info, vars, _OUTS_CSV_ROW);
        }
        else
        {
            report_capacity(info, vars, _OUTS_SUMMARY);
        }
    }
    close(rsk);
    free(packet);
    // -------

    return rtOk;
}
