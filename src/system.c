/*
 * lib/econet.c This file contains an implementation of the Econet
 *              support functions for the net-tools.
 *              (NET-3 base distribution).
 *
 * Version:     $Id: econet.c,v 1.11 2000/05/27 17:36:16 pb Exp $
 *
 * Author:      Philip Blundell <philb@gnu.org>
 *
 * Modified:
 *
 *              This program is free software; you can redistribute it
 *              and/or  modify it under  the terms of  the GNU General
 *              Public  License as  published  by  the  Free  Software
 *              Foundation;  either  version 2 of the License, or  (at
 *              your option) any later version.
 */

#include "system.h"

#if defined(__linux__) || defined(__RASP__)

#include <unistd.h>      // Posix API ( fopen, close etc.etc. )
#include <sys/ioctl.h>   // does not declare the ioctl function on some platforms: AIX 5.1, Solaris 11.4, Haiku 2017
#include <sys/types.h>   // defines a collection of typedef symbols and structures
#include <sys/socket.h>  // Macros and structures to use sockets
#include <netinet/in.h>  // Definitions for the internet protocol family
#include <netinet/tcp.h> // defines macros for use as a socket option
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <ifaddrs.h>       // get interface by address
#include <netdb.h>         // getnameinfo()
#include <linux/if_link.h> // struct rtnl_link_stats

#elif defined(__MINGW64__)

#elif defined(__windows__)

#include <winsock.h>
#include <winsock2.h>
#include <sys/types.h>

#endif

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

#include "helpers/timers.h"
#include "helpers/datetime.h"

#if HAVE_AFUNIX
#include <sys/un.h>
#endif

rt_t ipv4tcp_bandwidth(int ifn, long dband) // computes bandwidth for local interface
{

    return rtOk;
}

rt_t list_interfaces()
{

    int counter_network_interfaces = 0;

    struct if_nameindex *interface_indexes;
    struct if_nameindex *interface;

    interface_indexes = if_nameindex();

    if (interface_indexes == NULL) // Some error occurred during the execution of if_nameindex() function or there is no enough memory available
    {
        perror("If_nameindex");
        printf("Ooops...error while extracting the network interface names.\n");
        // exit(EXIT_FAILURE);
        return rtErr;
    }

    // Loop through the elements of the array of if_nameindex structs

    for (interface = interface_indexes; interface->if_index != 0 && interface->if_name != NULL; interface++)
    {
        counter_network_interfaces++;
        printf("There exists a network interface called \"%s\" with index %d. \n", interface->if_name, interface->if_index);
    }

    printf("In total, there is a number of %d network interfaces on this machine.\n", counter_network_interfaces);

    return rtExit;
}

rt_t list_interfaces2()
{
    // Interfface settings
    // get interface by IP Address (https://man7.org/linux/man-pages/man3/getifaddrs.3.html)
    struct ifaddrs *ifa, *iifa;
    if (getifaddrs(&ifa) == -1)
    {
        perror("getifaddrs");
        return rtErr;
    }
    /* Walk through linked list, maintaining head pointer so we
                  can free list later. */
    int family, s;
    char host[NI_MAXHOST];

    for (iifa = ifa; iifa != NULL; iifa = iifa->ifa_next)
    {
        if (iifa->ifa_addr == NULL)
            continue;

        family = iifa->ifa_addr->sa_family;

        // Display interface name and family (including symbolic form of the latter for the common families).

        printf("%-8s %s (%d)\n", iifa->ifa_name,
               (family == AF_PACKET) ? "AF_PACKET" : (family == AF_INET) ? "AF_INET"
                                                 : (family == AF_INET6)  ? "AF_INET6"
                                                                         : "???",
               family);

        /* For an AF_INET* interface address, display the address. */

        if (family == AF_INET || family == AF_INET6)
        {
            s = getnameinfo(iifa->ifa_addr,
                            (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
                            host, NI_MAXHOST,
                            NULL, 0, NI_NUMERICHOST);
            if (s != 0)
            {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            }

            printf("\t\taddress: <%s>\n", host);
        }
        else if (family == AF_PACKET && iifa->ifa_data != NULL)
        {
            struct rtnl_link_stats *stats = (struct rtnl_link_stats *)&iifa->ifa_data;

            printf("\t\ttx_packets = %10u; rx_packets = %10u\n"
                   "\t\ttx_bytes   = %10u; rx_bytes   = %10u\n",
                   stats->tx_packets, stats->rx_packets,
                   stats->tx_bytes, stats->rx_bytes);
        }
    }
    freeifaddrs(ifa);
    return rtExit;
}

rt_t get_ipv4()
{

    struct aftype **aft;

    struct ifreq ifrx; // (https://man7.org/linux/man-pages/man7/netdevice.7.html)

    // strcpy(ifr->ifr_name, "eth0");

    int skt = socket(AF_INET, SOCK_STREAM, 0);

    if (ioctl(skt, SIOCGIFMTU, &ifrx) != 0) // Gets current mtu value
    {
        char saddr[INET_ADDRSTRLEN];
        //   inet_ntop(AF_INET, &(ifr.ifr_ifru.ifru_addr.sa_data), saddr, INET_ADDRSTRLEN);

        //  pverbose("ipv4tcp: if %s MTU  %lu, addr %s \n", ifr.ifr_ifrn.ifrn_name, ifr.ifr_mtu, saddr);
    }
    /*
    ifr.ifr_mtu = ... // Change value if it needed
        if (!ioctl(sock, SIOCSIFMTU, &ifr))
    {
        // Mtu changed successfully
    }
    */
    return rtExit;
}

#include <sys/utsname.h>

#define KRELEASE(maj, min, patch) ((maj) * 10000 + (min) * 1000 + (patch))
int kernel_version(void)
{
    struct utsname uts;
    int major, minor, patch = 0;

    if (uname(&uts) < 0)
        return rtErr;
    if (sscanf(uts.release, "%d.%d.%d", &major, &minor, &patch) < 2)
        return rtErr;
    return KRELEASE(major, minor, patch);
}

/*

int set_mtu(const char *interface, int mtu)
{
    int fd;
    struct ifreq ifr;

    // Create a socket (required for ioctl)
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
    {
        perror("socket");
#include <unistd.h> // Posix API ( fopen, close etc.etc. )


        return -1;
    }

    // Set interface name
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';

    // Set MTU value
    ifr.ifr_mtu = mtu;

    // Perform ioctl call to set MTU
    if (ioctl(fd, SIOCSIFMTU, &ifr) == -1)
    {
#include <unistd.h> // Posix API ( fopen, close etc.etc. )


        perror("ioctl (SIOCSIFMTU)");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}
*/

int get_mtu(const char *interface, int mtu)
{
    int fd;

    // Create a socket (required for ioctl)
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
    {
        perror("socket");
        return -1;
    }

    // Set interface name
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';

    // Set MTU value
    ifr.ifr_mtu = mtu;

    // Perform ioctl call to set MTU
    if (ioctl(fd, SIOCSIFMTU, &ifr) == -1)
    {
        perror("ioctl (SIOCSIFMTU)");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

/*
int get_mtu(const char *interface_name) {
    int fd;
    struct ifreq ifr;

    // Create a socket (you can also use an existing one)
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        perror("socket creation failed");
        return -1;
    }

    // Zero out the ifreq structure
    memset(&ifr, 0, sizeof(ifr));

    // Copy the interface name into the ifreq structure
    strncpy(ifr.ifr_name, interface_name, IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0'; // Ensure null termination

    // Use ioctl to get the interface MTU
    if (ioctl(fd, SIOCGIFMTU, &ifr) == -1) {
        perror("ioctl SIOCGIFMTU failed");
        close(fd);
        return -1;
    }

    close(fd);
    return ifr.ifr_mtu;
}
    */

/*

 // get Interface name by Address AF_INET (https://man7.org/linux/man-pages/man3/getifaddrs.3.html)
in_addr_t ip_addr = 0;
struct ifaddrs *ifa, *iifa;
int rt;
if ((rt = getifaddrs(&ifa)) < 0) // Network address:  iifa->ifa_addr (struct sockaddr *ifa_addr) Big-endian
{
    pverbose("ipv4tcp: getifaddrs() failed: %s\n", gai_strerror(rt));
    return rtErr;
}

char *host_ip;
iifa = ifa;
while (iifa != NULL)
{
    if (iifa->ifa_addr != NULL && iifa->ifa_addr->sa_family == env.local.sin_family)
    {
        host_ip = inet_ntoa(((struct sockaddr_in *)iifa->ifa_addr)->sin_addr);
        // pverbose("interface: <%s>  <%s>\n", iifa->ifa_name, host_ip); // env.local_if.ifr_ifrn.ifrn_name

        pverbose("'%s' == '%s', cmp %d\n", ops_.local_addr, host_ip, strncmp(ops_.local_addr, host_ip, strlen(ops_.local_addr)));
        if (strncmp(ops_.local_addr, host_ip, strlen(ops_.local_addr)) == 0)
        {

            strncpy(&env.local_if_name[0], iifa->ifa_name, strlen(iifa->ifa_name)); // Copy the interface name into the ifreq structure
            env.local_if_name[IFNAMSIZ - 1] = '\0';
            pverbose("ipv4tcp: uses interface '%s', <%-8s> \n", env.local_if_name, inet_ntoa(((struct sockaddr_in *)iifa->ifa_addr)->sin_addr)); //
        }
    }

    // pverbose("'%s' == '%s', cmp %d\n", ops_.local_addr, *(char *)iifa->ifa_addr, strncmp(ops_.local_addr, iifa->ifa_addr->sa_data, strlen(ops_.local_addr)));

    // if ((ip_addr = inet_addr(struct in_addr)iifa->ifa_addr)) == 0) // dot-notations to IP network address

    // if (strncmp(iifa->ifa_name, &ops_.local_addr[0], strlen(iifa->ifa_name)) == 0)
    // pverbose("Interface <%s> \n", iifa->ifa_name);

    if (iifa->ifa_addr->sa_family == env.local.sin_family)
    {
        rt = getnameinfo(iifa->ifa_addr,
                         (env.local.sin_family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
                         host_ip, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        if (rt != 0)
        {
            pverbose("ipv4tcp: getnameinfo() failed: %s\n", gai_strerror(rt));
            return rtErr;
        }

        // pverbose("'%s' == '%s', cmp %d\n", ops_.local_addr, host_ip, strncmp(ops_.local_addr, host_ip, strlen(ops_.local_addr)));
        if (strncmp(&host_ip[0], &ops_.local_addr[0], strlen(host_ip)) == 0)
        {
            strncpy(&env.local_if.ifr_ifrn.ifrn_name[0], iifa->ifa_name, strlen(iifa->ifa_name)); // Copy the interface name into the ifreq structure
            env.local_if.ifr_name[IFNAMSIZ - 1] = '\0';                                           // ensure null termination
            pverbose("ipv4tcp: uses interface <%s> \n", env.local_if.ifr_name, host_ip);          // %-8s <%s>
        }
    }
    iifa = iifa->ifa_next;
}

freeifaddrs(ifa);

*/