
#include "tester_ipv4tcp.h"

#include <stdio.h>  // Standard C
#include <unistd.h> // Posix API ( fopen, close etc.etc. )

#include <string.h>
#include <errno.h>
#include <math.h>

#if defined(__linux__) || defined(__RASP__)

#include <sys/types.h>  // defines a collection of typedef symbols and structures
#include <sys/socket.h> // Macros and structures to use sockets
#include <sys/ioctl.h>  // does not declare the ioctl function on some platforms: AIX 5.1, Solaris 11.4, Haiku 2017
//
#include <netinet/in.h>  // Definitions for the internet protocol family
#include <netinet/tcp.h> // defines macros for use as a socket option
#include <arpa/inet.h>

// #include <net/if.h>       // User space
#include <linux/if.h>      // Kernel space
#include <linux/if_link.h> // struct rtnl_link_stats

#include <ifaddrs.h> // get interface by address
#include <netdb.h>   // getnameinfo()

#define NI_MAXHOST 1025
#define IF_NAMESIZE 16
#define IFNAMSIZ IF_NAMESIZE

#elif defined(__MINGW64__)

#elif defined(__windows__)

#include <sys/types.h>
#include <winsock.h>
#include <winsock2.h>

#endif

#include "../helpers/timers.h"
#include "../helpers/datetime.h"
#include "../system.h"

// -------------------------------------------------------------------------------------------------------- !
static const model_info_t info = { // TESTER:
    _PROTO_IPV4,                   // protocol
    __Capacity,                    // evaluation model
    0,                             // variant
    "IPv4/TCP",                    // name
    "Throughput/Bandwidth v.01a",  // description
    "developers@sebyone.it"};      // author

// -------------------------------------------------------------------------------------------------------- !
static capacity_vars_t vars; // capacity_vars

// -------------------------------------------------------------------------------------------------------- !
static struct // Specific ipv4 parameters
{
    int repeat_n;                 // test repetittion
    short int port;               // IPv4 Port
    struct sockaddr_in local_ip;  // server mode local_ip hwif/IP
    struct sockaddr rpeer;        // connected remote_ip
    struct sockaddr_in remote_ip; // Client mode only
    char local_if[IFNAMSIZ];      // local_ip Interface name !!!!!!!!!!!!!!!!!!
    size_t mss;                   // mss size
    size_t mtu;                   // mss size
    size_t if_bandwidth;          // retrieved from hardware IF
    size_t block_size;
    size_t timeslot; // time to test
    //
    bool _ocsv;
    bool _ocsvheader;
    bool _opkts;
} env; // env_settings_ipv4_t;

// -------------------------------------------------------------------------------------------------------- !
rt_t sktoption(int sk_, int op_, int &val_);
// rt_t getifname(const char *ip_addr_);

// -------------------------------------------------------------------------------------------------------- !
void setDefaultEnv()
{
    memset(&env, 0, sizeof(env));
    // env.mode = _ROLE_NONE;               // run mode
    env.repeat_n = 1;                          // run counter
    env.port = IPV4_DEF_SPORT;                 // default service port
    env.local_if[0] = '\0';                    // uses interface
    env.local_ip.sin_family = AF_INET;         // IP family
    env.local_ip.sin_addr.s_addr = INADDR_ANY; // Set all locals
    env.local_ip.sin_port = htons(env.port);   // default port setted - ntohs()
    env.rpeer.sa_family = AF_INET;             // IP family "AF_UNSPEC"
    env.rpeer.sa_data[0] = '\0';               // IP family

    env.remote_ip.sin_family = AF_INET;         // IP family
    env.remote_ip.sin_addr.s_addr = INADDR_ANY; // Set all locals
    env.remote_ip.sin_port = htons(env.port);   // default port setted
    //
    env.mss = 0; // 0 = use current/negotiate, >0 = force mss
    env.mtu = 0; // 0 = use current, >0 = force mtu

    env.if_bandwidth = 0; // nominal Speed: 50000Mb/s
    env.block_size = 0;   // output packet contents
    //
    env._ocsv = false;       // enable csv
    env._ocsvheader = false; // csv header
    env._opkts = false;      // output packet contents
}

// -------------------------------------------------------------------------------------------------------- !
#include <stdint.h>

uint32_t swap_endian_32(uint32_t num)
{
    return ((num >> 24) & 0xFFu) |      // Move byte 3 to byte 0
           ((num << 8) & 0xFF0000u) |   // Move byte 1 to byte 2
           ((num >> 8) & 0xFF00u) |     // Move byte 2 to byte 1
           ((num << 24) & 0xFF000000u); // Move byte 0 to byte 3
}
// ------------------------
rt_t set_env_ipv4tcp(options_t &ops_) // Set Tester Parameter
{

    setDefaultEnv(); // Tester parameters (enviroment)

    // ---------------- get Interface name by Address AF_INET (https://man7.org/linux/man-pages/man3/getifaddrs.3.html)
    struct ifaddrs *ifa, *iifa;
    int rt;
    if ((rt = getifaddrs(&ifa)) < 0)
    {
        pverbose("ipv4tcp: getifaddrs() failed: %s\n", gai_strerror(rt));
        return rtErr;
    }
    char *host_ip;
    iifa = ifa;
    while (iifa != NULL)
    {
        if (iifa->ifa_addr != NULL && iifa->ifa_addr->sa_family == env.local_ip.sin_family)
        {
            host_ip = inet_ntoa(((struct sockaddr_in *)iifa->ifa_addr)->sin_addr);
            // pdebug("interface: <%s>  <%s>\n", iifa->ifa_name, host_ip); // env.local_if.ifr_ifrn.ifrn_name
            // pdebug("'%s' == '%s', cmp %d\n", ops_.local_addr, host_ip, strncmp(ops_.local_addr, host_ip, strlen(ops_.local_addr)));
            if (strncmp(ops_.local_addr, host_ip, strlen(ops_.local_addr)) == 0)
            {
                strncpy(&env.local_if[0], iifa->ifa_name, strlen(iifa->ifa_name)); // Copy the interface name into the ifreq structure
                env.local_if[IFNAMSIZ - 1] = '\0';
                //------------
                // env.local_ip.sin_addr = ((struct sockaddr_in *)iifa->ifa_addr)->sin_addr;
                // env.local_ip = *((struct sockaddr_in *)iifa->ifa_addr);
                //--------------
                // pverbose("ipv4tcp: uses interface '%s', <%-8s> \n", env.local_if, inet_ntoa(((struct sockaddr_in *)iifa->ifa_addr)->sin_addr)); //
            }
        }
        iifa = iifa->ifa_next;
    }
    freeifaddrs(ifa);
    // ----------------

    // Protocol settings
    env.timeslot = (size_t)ops_.tst_time_slot; // timed mode
    env.mss = ops_.pkt_payload_size;           // packet payload
    env.mtu = ops_.pkt_mtu_size;
    env.if_bandwidth = ops_.bandwidth; // packet payload

    // Performing mode
    env.block_size = (size_t)ops_.tst_block_size; // block-size mod

    // Reporting
    env._ocsv = ops_.csv_enabled;
    env._ocsvheader = !ops_.csv_no_header;
    env._opkts = ops_.pktverbose; // Verbose mode

    // Peers addressing
    if (ops_.service_num > 0) // Check range and Set IPv4 port number
    {
        if ((ops_.service_num < IPV4_MIN_SPORT) || (ops_.service_num > IPV4_MAX_SPORT))
        {
            pverbose("ipv4tcp: invalid port number: '%d' !\n", ops_.service_num);
            return rtErr;
        }
        env.port = ops_.service_num;
    }
    else // set default port
    {
        pverbose("ipv4tcp: uses default service port: '%d' !\n", env.port); // default setted
    }
    env.local_ip.sin_port = htons(env.port); // Service

    if (ops_.run_mode == _ROLE_SERVER)
    {
        if (strchr(ops_.local_addr, '*')) // Set default local_ip addresses/interfaces
        {
            // TODO: get local address !
            // TODO: retrieves bandwidth from hardware
            pverbose("ipv4tcp: bind local default IPv4 addresses !\n");
        }
        else // Listen on specified address/interface
        {
            // in_addr_t ip_addr;
            //  TODO: retrieves bandwidth from hardware
            if ((env.local_ip.sin_addr.s_addr = inet_addr(ops_.local_addr)) == 0) // dot-notations to IP network address
            {
                pverbose("ipv4tcp: invalid IP %s !\n", ops_.local_addr); // Error in address !
                return rtErr;
            }
            // env.local_ip.sin_addr.s_addr = ip_addr; // pverbose("ipv4tcp: ready to start SERVER (%s:%d)\n", inet_ntoa(env.local.sin_addr), env.port);
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

        if (inet_pton(AF_INET, ops_.remote_addr, &env.remote_ip.sin_addr) == 0) // converts 'dot-notation' in 'binary format' (https://man7.org/linux/man-pages/man3/inet.3.html)
        {
            pverbose("ipv4tcp: invalid remote_ip address %s !\n", ops_.remote_addr); // Error in address !
            return rtErr;
        }
    }
    return rtOk;
}

// -------------------------------------------------------------------------------------------------------- !
// LOOPBACK SERVER
// -------------------------------------------------------------------------------------------------------- !
rt_t run_server_ipv4tcp()
{
    ssize_t recvd, total_recv_data, total_recv_pkts;
    double start, elapsed, throughput;
    char *buffer;
    int opt, remote_sk = 0;
    int local_sk = socket(env.local_ip.sin_family, SOCK_STREAM, 0);

    resetVars(vars); // Result variables
    vars.blocksize = (double)env.block_size;
    vars.bandwidth = env.if_bandwidth; // Reference to bandwidth !!!!!!!

    if (local_sk < 0)
    {
        pverbose("ipv4tcp: can't allocate AF_INET, SOCK_STREAM fault #%d %s\n", errno, strerror(errno));
        return rtErr;
    }

    // -- MSS - TCP layer (OSI layer 4)
    size_t curr_set = 0;
    socklen_t len = sizeof(env.mss);
    if (env.mss > 0) // Set mss
    {
        curr_set = env.mss;
        if (setsockopt(local_sk, IPPROTO_TCP, TCP_MAXSEG, &env.mss, len) != 0) // set payload size (mss) !
        {
            pverbose("ipv4tcp: socket set option TCP_MAXSEG fault #%d %s\n", errno, strerror(errno));
        }
    }
    len = sizeof(env.mss);
    if (getsockopt(local_sk, IPPROTO_TCP, TCP_MAXSEG, &env.mss, &len) == 0) // read back payload size (4bytes)
    {

        pverbose("ipv4tcp: uses packet payload size MSS: %d [bytes] \n", env.mss);
        vars.pktpayload = env.mss;
    }
    else
    {
        pverbose("ipv4tcp: can't get mss (TCP_MAXSEG) fault #%d %s\n", errno, strerror(errno));
        close(local_sk);
        return rtErr;
    }

    // -- MTU - IP layer (OSI layer 3)
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, env.local_if, IFNAMSIZ - 1);
    if (env.mtu > 0) // Set mtu
    {
        ifr.ifr_mtu = env.mtu;
        if (ioctl(local_sk, SIOCSIFMTU, &ifr) < 0) // Sets MTU value
        {
            pverbose("ipv4tcp: can't set MTU, ioctl SIOCSIFMTU failed #%d %s\n", errno, strerror(errno));
            close(local_sk);
            return rtErr;
        }
    }

    if (ioctl(local_sk, SIOCGIFMTU, &ifr) < 0) // gets the interface MTU - read back
    {
        pverbose("ipv4tcp: ioctl SIOCGIFMTU failed #%d %s\n", errno, strerror(errno));
        close(local_sk);
        return rtErr;
    }
    else
    {
        env.mtu = ifr.ifr_ifru.ifru_mtu;
        vars.pktmtu = env.mtu;
        pverbose("ipv4tcp: uses interface '%s' with MTU: %d [bytes] !\n", env.local_if, env.mtu);
    }

    // -----------------------

    if (setsockopt(local_sk, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != 0) // set reusable socket
    {
        pverbose("ipv4tcp: socket option (SO_REUSEADDR) fault #%d %s\n", errno, strerror(errno));
    }
    struct timeval tv; //  Timeout in seconds
    tv.tv_usec = IPV4_RCVTIMEO_SEC;
    if (setsockopt(local_sk, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) // set timer for recv_socket
    {
        pverbose("ipv4tcp: socket option (SO_RCVTIMEO) fault #%d %s\n", errno, strerror(errno));
    }

    if (bind(local_sk, (struct sockaddr *)&env.local_ip, sizeof(struct sockaddr)) < 0)
    {
        pverbose("ipv4tcp: can't bind local address #%d %s\n", errno, strerror(errno));

        close(local_sk);
        return rtErr;
    }
    if (listen(local_sk, 1) < 0)
    {
        pverbose("ipv4tcp: listen socket fault !\n");
        close(local_sk);
        return rtErr;
    }

    pverbose("ipv4tcp: listen on (%s:%d)\n", inet_ntoa(env.local_ip.sin_addr), env.port);
    // -----------

    /*
        char *buffer = (char *)malloc(PACKET_BUFFER_MAX_SIZE);

        // struct sockaddr_in rpeer; // connected remote_ip
        // socklen_t rpeer_addr_len = sizeof(struct sockaddr_in);
        // memset(&rpeer, 0, sizeof(struct sockaddr_in));

        struct sockaddr rpeer; // connected remote_ip
        socklen_t rpeer_len = sizeof(rpeer);
        memset(&rpeer, 0, rpeer_len);
        rpeer.sa_family = AF_INET; // IP family "AF_UNSPEC"
        rpeer.sa_data[0] = '\0';   // IP family

        while (1)
        {
            remote_sk = accept(local_sk, (struct sockaddr *)&rpeer, &rpeer_len); // (https://man7.org/linux/man-pages/man2/accept.2.html)
            if (local_sk < 0)
            {
                pverbose("ipv4tcp: incoming connection from [%s] refused !\n", inet_ntoa(((struct sockaddr_in *)&rpeer)->sin_addr));
                continue;
            }
            pverbose("ipv4tcp: open connection remote_ip [%s] \n", rpeer.sa_data); // inet_ntoa(((struct sockaddr_in *)&rpeer)->sin_addr));

            // TODO: implement test protocol like fresbee !
            ....
    */

    buffer = (char *)malloc(PACKET_BUFFER_MAX_SIZE);
    socklen_t rpeer_addr_len = sizeof(env.rpeer);
    while (1)
    {
        remote_sk = accept(local_sk, (struct sockaddr *)&env.rpeer, &rpeer_addr_len); // (https://man7.org/linux/man-pages/man2/accept.2.html)
        if (local_sk < 0)
        {
            pverbose("ipv4tcp: incoming connection from [%s] refused !\n", env.rpeer.sa_data);
            continue;
        }
        pverbose("ipv4tcp: open connection remote_ip [%s] \n", inet_ntoa(*(struct in_addr *)&env.rpeer)); // inet_ntoa() ??? inet_ntoa(((struct sockaddr_in *)iifa->ifa_addr)->sin_addr)

        // TODO: implement test protocol like fresbee !
        total_recv_data = 0;
        total_recv_pkts = 0;

        start = now_millis();
        while (1) // TODO: until testing_time-out or connection closed !!!!!!!!!!!!!
        {
            recvd = recv(remote_sk, buffer, sizeof(buffer), 0); // MSG_DONTWAIT (https://man7.org/linux/man-pages/man2/recv.2.html)
            if (recvd < 0)
            {
                // recvd timeouts 30 !!!!
                pverbose("ipv4tcp: socket read error  #%d %s\n", errno, strerror(errno)); // errors ?
                break;
            }
            total_recv_pkts++;
            if (recvd == 0)
            {
                break;
            }
            total_recv_data += recvd;

#if defined(IPV4_PKT_FRISBEE)
            if (buffer[1] == 'S' && buffer[2] == 'B' && buffer[4] == 'E' && buffer[5] == '>')
            {
                pverbose("frisbee received");
                buffer[5] = '<';
                if (send(remote_sk, buffer, recvd, 0) < 0)
                {
                    pverbose("ipv4tcp: socket send data error  #%d %s\n", errno, strerror(errno)); // errors ?
                    break;
                }
            }
#endif
            if (env._opkts)
            {
                // printout header // verbose mode (https://man7.org/linux/man-pages/man3/cmsg.3.html)
                pverbose("recvd pkt #%d - data size %d [bytes] \n", total_recv_pkts, recvd);
                // printf(buffer);
                // printf("\n");
            }
        }
        elapsed = now_millis() - start;

        throughput = (elapsed > 0) ? (_Bytes2Megabits(total_recv_data) / (elapsed * 1000.0)) : 0;
        close(remote_sk); // tries to complete pending transmission and close (SO_LINGER).
        pverbose("ipv4tcp: closed, connection time %.6f [ms], recvd %ld [Bytes], throughput %.3f [Mbps] \n", elapsed, total_recv_data, throughput);
    }
    shutdown(local_sk, 2); // 2 = stop both reception and transmission.
    free(buffer);
    // -----------
    return rtOk;
}

// -------------------------------------------------------------------------------------------------------- !
// CLIENT TESTER
// a) block-size mode: send all data and computes total time.
// b) time-windowed mode: computes how many data are been sent in the time-windows period.
// -------------------------------------------------------------------------------------------------------- !
rt_t run_client_ipv4tcp()
{
    socklen_t sklen; // Temp for socket options setting
    ssize_t bytes2send = 1;
    char *packet;
    int remote_sk;

    remote_sk = socket(env.remote_ip.sin_family, SOCK_STREAM, IPPROTO_IP); // Create socket IPv4/TCP (SOCK_STREAM) // AF_UNSPEC
    if (remote_sk < 0)
    {
        pverbose("ipv4tcp: can't CREATE socket !");
        return (rtErr);
    }

    // Retries............
    if (connect(remote_sk, (struct sockaddr *)&env.remote_ip, sizeof(env.remote_ip)) < 0) // (https://man7.org/linux/man-pages/man2/connect.2.html)
    {
        pverbose("ipv4tcp: socket connection error  %d : %s\n", errno, strerror(errno));
        close(remote_sk);
        return (rtErr);
    }

    pverbose("ipv4tcp: uses remote_ip loopback at [%s:%d]\n", inet_ntoa(env.remote_ip.sin_addr), env.port);

    // -- MSS - TCP layer (OSI layer 4)
    /*
    if (env.mtu > 0) // Tries to set
    {
        if (setsockopt(local_sk, IPPROTO_TCP, TCP_MAXSEG, &env.mss, sizeof(env.mss)) != 0)
        {
            pverbose("ipv4tcp: can't set MSS!");
            return (rtErr);
        }
    }

    // Tries to negotiate mss
    if (send(local_sk, packet, (size_t)bytes2send, MSG_CONFIRM) < 0)
    {
        pverbose("ipv4tcp: socket connection error  #%d %s\n", errno, strerror(errno)); // error EAGAIN or EWOULDBLOCK.
    }
    */
    env.mtu = IPV4_DEF_MTU;

    sklen = sizeof(env.mss);
    if (getsockopt(remote_sk, IPPROTO_TCP, TCP_MAXSEG, &env.mss, &sklen) == 0) // read payload size
    {
        pverbose("ipv4tcp: uses packet's payload size MSS: %d [bytes] !\n", env.mss);
    }
    else
    {
        pverbose("ipv4tcp: get socket option (TCP_MAXSEG) fault [%d] !\n", errno);
        env.mss = IPV4_DEF_MSS;
        pverbose("ipv4tcp: uses payload size (mss): %.0ld \n", env.mss);
    }

    resetVars(vars); // Result variables
    vars.blocksize = (double)env.block_size;
    vars.timeslot = (double)env.timeslot; // timed mode
    vars.bandwidth = env.if_bandwidth;    // Reference to bandwidth !!!!!!!
    // computes number of packets will be sended
    vars.pktstosend = (env.mss >= env.block_size) ? 1.0 : trunc(env.block_size / env.mss) + env.block_size % env.mss;

    // -- Run mode

    if (env._ocsv && env._ocsvheader) // output csv
    {
        report_capacity(&info, &vars, _OUTS_CSV_HEADER);
    }

    // allocates a packets buffer
    const size_t pkt_buffer_size = (size_t)(PACKET_BUFFER_MAX_SIZE);
    packet = (char *)malloc(pkt_buffer_size);
    memset(packet, 'A', pkt_buffer_size);

#if defined(IPV4_PKT_FRISBEE)
    struct timeval tv; //  Timeout in seconds
    tv.tv_usec = IPV4_RCVTIMEO_SEC;
    if (setsockopt(remote_sk, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) // set time-out for recv_socket
    {
        pverbose("ipv4tcp: socket option (SO_RCVTIMEO) fault  [%d] !\n", errno);
    }
    // Frisbee packet ;
    packet[0] = 'F';
    packet[1] = 'S';
    packet[2] = 'B';
    packet[3] = 'E';
    packet[4] = 'E';
    packet[5] = '>';
    ssize_t recvd = 0;
    ssize_t total_recv_pkts = 0;
    ssize_t total_recv_data = 0;
    double pkt_rtrip_time;
#endif
    int tstcounter = 0;
    ssize_t sent;
    ssize_t total_sent_pkts;
    ssize_t total_sent_data;

    // --- BEGIN: TEST BY DATA-BLOCK
    while (tstcounter++ < env.repeat_n) // Performs one or many tests...
    {
        bytes2send = env.block_size;
        total_sent_pkts = 0;
        total_sent_data = 0;

        vars.trasftime = now_millis();

        while (bytes2send > 0)
        {
            if (bytes2send >= env.mss)
            {
                sent = send(remote_sk, packet, env.mss, 0); // (https://man7.org/linux/man-pages/man2/send.2.html)
            }
            else
            {
                sent = send(remote_sk, packet, (size_t)bytes2send, 0);
            }
            if (sent < 0)
            {
                pverbose("ipv4tcp: socket connection error  #%d %s\n", errno, strerror(errno)); // error EAGAIN or EWOULDBLOCK.
            }
            else
            {

#if defined(IPV4_PKT_FRISBEE)
                pkt_rtrip_time = now_millis();
#endif
                pverbose("ipv4tcp: sended %ld \n", sent);
                bytes2send -= sent;
                total_sent_pkts++;
                total_sent_data += sent;
            }

#if defined(IPV4_PKT_FRISBEE)
            recvd = recv(remote_sk, packet, sizeof(packet), 0); // MSG_DONTWAIT (https://man7.org/linux/man-pages/man2/recv.2.html)
            if (recvd < 0)
            {
                // recvd timeouts !!!!
                pverbose("ipv4tcp: socket read error  #%d %s\n", errno, strerror(errno)); // errors ?
                break;
            }
            else
            {
                if (packet[1] == 'S' && packet[3] == 'E' && packet[5] == '<')
                {
                    pkt_rtrip_time = (now_millis() - pkt_rtrip_time);
                    vars.rtriptime = (vars.rtriptime + pkt_rtrip_time) / 2.0;
                    vars.jitter = (vars.jitter + (pkt_rtrip_time - vars.rtriptime)) / 2.0;
                    vars.latyency += (pkt_rtrip_time / 2.0); // Total
                    vars.pktsrcvd++;
                    total_recv_pkts++;
                    total_recv_data += recvd;
                    packet[5] = '>';
                }
            }
#endif
        }
        // Update vars
        vars.trasftime = (now_millis() - vars.trasftime); // ms VERIFICARE !!!!!!!!!!!
        vars.tstcounter = tstcounter;
        vars.pktssent = (double)total_sent_pkts;
        vars.datasent += (double)total_sent_data;

#if defined(IPV4_PKT_FRISBEE)
        vars.pktsrcvd = (double)total_recv_pkts;
        vars.datarcvd = (double)total_recv_data;
        vars.dataloss = vars.pktssent - vars.pktsrcvd;
        vars.pktsloss = vars.datasent - vars.datarcvd;
        vars.tsterrors = 1.0 - (vars.pktsrcvd / vars.pktssent) * -100.0;
#endif

        vars.throughput = _Bytes2Megabits(vars.datasent) / (vars.trasftime * 1000); // [Mbps]
        vars.bandwidth = 0;                                                         // [Mbps]
        vars.saturation = 0;                                                        // [Mbps]
        vars.jitter = abs(vars.jitter);

        vars.pktpayload = (double)env.mss;
        vars.pktheader = (double)(env.mtu - env.mss);
        vars.pktefficiency = vars.pktpayload / (vars.pktpayload + vars.pktheader) * 100.0;

        if (env._ocsv) // Outputs test results
        {
            report_capacity(&info, &vars, _OUTS_CSV_ROW);
        }
        else
        {
            report_capacity(&info, &vars, _OUTS_SUMMARY);
        }
    }
    close(remote_sk);
    free(packet);

    // --- END: DATA-BLOCK

    /*
    // --- BEGIN: TIME-WINDOWED
    vars.tstcounter = 0;
    ssize_t sent;
    while (vars.tstcounter++ < env.repeat_n) // Performs one or many tests...
    {
        vars.pktssent = 0;
        bytes2send = (ssize_t)vars.blocksize;

        vars.trasftime = now_millis();

        while (bytes2send > 0)
        {
            if (bytes2send >= curr_mss)
            {
                sent = send(local_sk, packet, curr_mss, 0); // (https://man7.org/linux/man-pages/man2/send.2.html)
            }
            else
            {
                sent = send(local_sk, packet, (size_t)bytes2send, 0);
            }
            if (sent < 0)
            {
                pverbose("ipv4tcp: socket connection error  #%d %s\n", errno, strerror(errno)); // error EAGAIN or EWOULDBLOCK.
            }
            else
            {
                pverbose("ipv4tcp: sended %d \n", sent);
                bytes2send -= sent;
                vars.pktssent++;
            }
        }

        // Update result vars
        vars.trasftime = (now_millis() - vars.trasftime); // ms VERIFICARE !!!!!!!!!!!
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
    close(local_sk);
    free(packet);
    // --- END: TIME-WINDOWED
    */

    return rtOk;
}

/*
rt_t sktoption(int sk_, int op_, int &val_)
{
    switch (op_)
    {
    case SO_RCVTIMEO:
        struct timeval tv; //  Timeout in seconds
        tv.tv_usec = IPV4_RCVTIMEO_SEC;
        if (setsockopt(sk_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) // set timer for recv_socket
        {
            pverbose("sktoption: SO_RCVTIMEO fault  [%d] !\n", errno);
        }
        break;
    case SO_REUSEADDR:
        if (setsockopt(local_sk, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) != 0) // set reusable socket
        {
            pverbose("ipv4tcp: socket option (SO_REUSEADDR) fault [%d]\n!", errno);
        }
    }
    return rtOk;
}
    */