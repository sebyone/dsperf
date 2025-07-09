/*
 * dsperf 2024, 2025 (@) Sebyone Srl
 *
 * File: tester_ipv4tcp.h
 *
 * Licence
 * This Source Code Form  is subject to the terms of the Mozilla Public License, v. 2.0.
 * You can obtain a copy of the MPL License at https://mozilla.org/MPL/2.0/.
 * Disclaimer of Warrant
 * Covered Software is provided under this License on an "as is" basis, without warranty
 * of any kind.The entire risk as to the quality and performance of the Covered Software
 * is  with You.  This  disclaimer  of  warranty  constitutes  an essential part of this
 * License. No use of any Covered Software is authorized under this License except under
 * this disclaimer.
 *
 * Contributors:
 * plogiacco@smartlab.it - initial design, implementation and documentation
 * sebastiano.meduri@gmail.com  - initial design, implementation and documentation
 *
 */

/*  dsperf tester routine
    ------------------------------------------
          name: "IPv4/TCP"
          specs: "Throughput/Bandwidth v.01a"
         author: "developers@sebyone.it"
       protocol: _PROTO_IPV4,
          model: __Capacity
        variant: 0


    Overview
    --------
    Trasmission Capacity Tester routine for Transmission Control Protocol (6-TCP)
    Sends a block of data to remote_ip server and print out all test results.

    IPv4 Protocol as defined in IETF RFCs 790-791
    -----+---------------------------------------+----------
      1	  Internet Control Message Protocol	      ICMP
      2	  Internet Group Management Protocol	    IGMP
      6	  Transmission Control Protocol	          TCP ==> RFC 793, RFC 1122 and RFC 2001
     17	  User Datagram Protocol	                UDP
     41	  IPv6 encapsulation	                    ENCAP
     89	  Open Shortest Path First	              OSPF
    132	  Stream Control Transmission Protocol	  SCTP

    IPv4 Network stack
    ------------------
    Linux: https://man7.org/linux/man-pages/man7/socket.7.html   (BSD Posix socket)
    Microsoft (Winsock):
*/

/*
    IPv4 address family type AF_INET:
    -----------------------------------------------
      SOCK_STREAM -Supporta la comunicazione di flusso di byte orientata alla connessione affidabile.
      SOCK_DGRAM - Supporta le comunicazioni datagrammi non affidabili.
      SOCK_RAW - Supporta l'accesso non elaborato al protocollo di trasporto.

    Un'applicazion specifica un tipo di socket quando chiama la funzione per creare un nuovo socket.

    IPPROTO_XXX sono i valori dell'enumerazione di protocollo IPv4:
    ---------------------------------------------------------------
      IPPROTO_IP - Opzioni del protocollo Internet
      IPPROTO_ICMP - Protocollo del messaggio di controllo Internet
      IPPROTO_IGMP - Protocollo di gestione dei gruppi Internet
      IPPROTO_GGP - Gateway al protocollo gateway
      IPPROTO_IPV4 - Incapsulazione IPv4
      IPPROTO_ST - Protocollo di flusso
      IPPROTO_TCP - Protocollo di controllo della trasmissione
      IPPROTO_CBT - Protocollo degli alberi basati su base principale
      IPPROTO_EGP - Protocollo gateway esterno
      IPPROTO_IGP - Protocollo gateway interno privato
      IPPROTO_PUP - Protocollo di pacchetto universale PARC
      IPPROTO_UDP - Protocollo di datagrammi utente
      IPPROTO_IDP - Protocollo internet datagram
      IPPROTO_RDP - Protocollo dati affidabile
      IPPROTO_ND - Protocollo net disk
      IPPROTO_ICLFXBM - Monitoraggio a banda larga
      IPPROTO_PIM - Multicast indipendente dal protocollo
      IPPROTO_PGM - Multicast generale pragmatico
      IPPROTO_L2TP - Protocollo di tunneling livello 2
      IPPROTO_SCTP - Protocollo di trasmissione del controllo di flusso
      IPPROTO_RAW - Pacchetti IP non elaborati

    I protocolli aggiuntivi sono supportati tramite l'uso di socket non elaborati (RAW).
    Un'applicazione specifica un protocollo quando chiama la funzione per creare un nuovo socket.

    Un indirizzo di trasporto IPv4 viene specificato con la struttura SOCKADDR_IN.
    ----------------------------------------------------------------------------------------------------------------------
    La struttura SOCKADDR varia a seconda del protocollo selezionato.
    Ad eccezione del parametro sin*_family, il contenuto di sockaddr viene espresso in ordine di byte di rete.

    struct in_addr {              struct sockaddr {
      union {                       ushort  sa_family;
        struct {                    char    sa_data[14];
          u_char s_b1;            } SOCKADDR, *PSOCKADDR, *LPSOCKADDR;;
          u_char s_b2;
          u_char s_b3;            struct sockaddr_in {
          u_char s_b4;              short   sin_family;
          } S_un_b;                 u_short sin_port;
        struct {                    struct  in_addr sin_addr;
          u_short s_w1;             char    sin_zero[8];
          u_short s_w2;           } SOCKADDR_IN, *PSOCKADDR_IN, *LPSOCKADDR_IN;
          } S_un_w;
        u_long S_addr;
      } S_un;
    } in_addr_t;

    In sintesi, sockaddr è una struttura generica per gli indirizzi di socket, mentre sockaddr_in è una struttura s
    pecifica per gli indirizzi TCP/IP che facilita la gestione dell'indirizzo IP e del numero di porta.

    Inoltre SOCKADDR_STORAGE è una struttura che archivia anche le informazioni sull'indirizzo del socket ed
    è sufficientemente grande per archiviare le informazioni su indirizzi IPv4 o IPv6:

    typedef struct sockaddr_storage {
                        short   ss_family;                // Address family of the socket, such as AF_INET.
                        char    __ss_pad1[_SS_PAD1SIZE];  // Reserved. Defined as a 48-bit pad that ensures achieves 64-bit alignment.
                        __int64 __ss_align;               // Reserved. Used by the compiler to align the structure.
                        char    __ss_pad2[_SS_PAD2SIZE];  // Reserved. Used by the compiler to align the structure.
              } SOCKADDR_STORAGE, *PSOCKADDR_STORAGE;

    L'uso della struttura SOCKADDR_STORAGE promuove l'indipendenza della famiglia di protocolli e della versione del protocollo
    e semplifica lo sviluppo. È consigliabile utilizzare la struttura SOCKADDR_STORAGE al posto della struttura sockaddr.


    inet_addr(...)    The inet_addr function interprets the character string specified by the cp parameter. This string represents
                      a numeric Internet address expressed in the Internet standard ".'' notation. The value returned is a number
                      suitable for use as an Internet address. All Internet addresses are returned in IP's network order (bytes
                      ordered from left to right). If you pass in " " (a space) to the inet_addr function, inet_addr returns zero.

    inet_ntoa(...)    converte un indirizzo di rete Internet (Ipv4) in una stringa ASCII in formato decimale standard di Internet.

    htons(...)        takes a 16-bit number in "host byte order*" and returns a 16-bit number in "network byte order" used in
                      TCP/IP networks (the AF_INET or AF_INET6 address family).


                      *) Host byte order refers to the way a specific computer architecture stores multi-byte data (like
                      integers) in memory,  while network byte order is a standardized big-endian format used in network
                      communication to ensure consistent data interpretation across different systems.
     ----------------------------------------------------------------------------------------------------------------------    
*/

#ifndef MODEL_IPV4TCP_H
#define MODEL_IPV4TCP_H

#include "../models/capacity.h"
#include "../options.h"

// ipv4_tcp settings
#define PACKET_BUFFER_MAX_SIZE 2048

#define IPV4_MIN_MSS 256
#define IPV4_MAX_MSS 1460
#define IPV4_DEF_MSS 536

#define IPV4_MIN_MTU 256
#define IPV4_MAX_MTU 3000
#define IPV4_DEF_MTU 1500
//
#define IPV4_PKT_HEADER_SIZE 20
#define IPV4_GRE_HEADER_SIZE 4
#define IPV4_TCP_HEADER_SIZE 20
//
#define ETHERNET_HEADER_SIZE 14
#define ETHERNET_FCS_SIZE 4

#define IPV4_MIN_SPORT 0
#define IPV4_MAX_SPORT 65535
#define IPV4_DEF_SPORT 3001

#define IPV4_PKT_FRISBEE
#define IPV4_RCVTIMEO_SEC 30

#ifdef __cplusplus
extern "C"
{
#endif

//rt_t sktoption(int sk_, int op_, int &val_);
rt_t set_env_ipv4tcp(options_t &ops_); // set default Options values for model
rt_t run_server_ipv4tcp();             // start loopback server
rt_t run_client_ipv4tcp();             // performs test


#ifdef __cplusplus
}
#endif

#endif // MODEL_IPV4TCP_H
