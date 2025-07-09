  
    AF_INET is è la famiglia di indirizzi per IPv4:
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



/*
// Scambio RTT
char ping_buf[5] = {0};
ssize_t ping_len = recv(client_sock, ping_buf, sizeof(ping_buf), 0);
if (ping_len == sizeof(ping_buf) && strcmp(ping_buf, "PING") == 0)
{
    if (send(client_sock, "PONG", 5, 0) != 5)
    {
        perror("[SERVER] Errore invio PONG");
        free(buffer);
        close(client_sock);
        continue;
    }
}
else
{
    fprintf(stderr, "[SERVER] Ping non ricevuto correttamente\n");
    free(buffer);
    close(client_sock);
    continue;
}
*/

/*

    int to_send;

    if (__flag_time_defined) // Runs in time-mode
    {
        // curr_time < end_time_def
        while ((curr_time < end_time_def))
        {
            to_send = pkt_payload_size;
            if (send(sock, pkt_payload, to_send, 0) > 0)
                break;
            curr_time = get_time_microseconds();
        }
    }
    else // runs in block-size mode
    {

        while ((bytes_sent < block_size))
        {
            if (!dsperf_timer_wait_tick(timer)) // Semaphore
            {
                break;
            }
            to_send = block_size - bytes_sent;
            if (to_send > pkt_payload_size)
                to_send = pkt_payload_size;

            // to_send = pkt_payload_size;
            if ((sent = send(sock, pkt_payload, pkt_payload_size, 0)) <= 0)
                break;
            bytes_sent += sent;
            pkts_counter++;
        }
    }

    double end_time = get_time_microseconds();

    dsperf_timer_stop(timer);
    dsperf_timer_destroy(timer);

    double elapsed_ms = (end_time - start_time) / 1000; // us -> ms
    double elapsed_s = elapsed_ms / 1000;

    // Dopo aver effettuato il test, posso ottenere il numero di pacchetti persi soltanto ricevendo il numero effettivo che ha raggiunto il server
    // Ovvero il server risponde con il numero effettivo di pacchetti che ha ricevuto!!!
    (per testare protocolli stateless ad esempio UDP, nel caso di TCP senza accedere al raw socket !!!)

        double out_throughput_Mbytes = (_Byte2Megabyte(bytes_sent) / (double)elapsed_s); // / (elapsed_ms / (double)1000); // Ottengo Mega byte per secondo
    double out_throughput_Mbits = out_throughput_Mbytes * (double)8;                     // Ottengo Mega bit per secondo
    double out_throughput_pps = (double)pkts_counter / elapsed_s;

    int out_bytes_to_send = block_size; // Total byte to send
    int out_bytes_lost = out_bytes_to_send - bytes_sent;

    double out_bytes_lost_percents = out_bytes_to_send > 0 ? ((double)out_bytes_lost / out_bytes_to_send) * 100.0 : 0.0;

    /*
    if (csv_enabled == 1)
        fclose(csv);
    */
// usleep(1000); // TODO: utilizzare opzione per attivare il ritardo

/* Solo se flag selezionato !!!!!!!
FILE *csv = fopen(csv_path, i == 0 ? "w" : "a");
int csv_enabled = 1;
if (!csv)
{
    csv_enabled = 0;
}

if (i == 0 && csv_enabled == 1)
    fprintf(csv, "Timestamp_ms,Num Pkt,Dimensione Pkt,Throughput_MBps,RTT_ms\n");

char ping_msg[] = "PING";
char pong_msg[5] = {0};

double rtt_ms = 0;
double ping_start = get_time_microseconds();
if (send(sock, ping_msg, sizeof(ping_msg), 0) != sizeof(ping_msg))
{
    perror("[CLIENT] Error sending PING");
}
else
{
    ssize_t r = recv(sock, pong_msg, sizeof(pong_msg), 0);  // Bloccante !!!!!!!!!!

    if (r != sizeof(pong_msg) || strcmp(pong_msg, "PONG") != 0)
    {
        fprintf(stderr, "[CLIENT] Invalid PONG response\n");
    }
    else
    {
        double ping_end = get_time_microseconds();
        rtt_ms = (ping_end - ping_start) / 1000.0;
        // printf("[CLIENT] RTT misurato: %.3f ms\n", rtt_ms);
    }
}
*/
/*

   int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            perror("socket");
            return;
        }

        struct sockaddr_in serv_addr = {
            .sin_family = AF_INET,
            .sin_port = htons(server_port),
        };
        inet_pton(AF_INET, server_ip, &serv_addr.sin_addr);

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            perror("connect");
            close(sock);
            continue;
        }

        // Get MSS
        int mss = 0;
        socklen_t optlen = sizeof(mss);
        if (getsockopt(sock, IPPROTO_TCP, TCP_MAXSEG, &mss, &optlen) == 0)
        {
            // printf("[CLIENT] MSS negotiated: %d bytes\n", mss);
        }
        /*
        else {
            perror("getsockopt");
            mss = 1448;
        }
        if (mss > 1448) {
            printf("[CLIENT] Warning: MSS (%d) > 1448; using 1448 for safety.\n", mss);
            mss = 1448;
        }
        */

        int chunk_size = (mss < block_size) ? mss : block_size;
        char *buffer = malloc(chunk_size);
        memset(buffer, 'A', chunk_size);

        FILE *csv = fopen(csv_path, i == 0 ? "w" : "a");
        int csv_enabled = 1;
        if (!csv)
        {
            csv_enabled = 0;
        }
        if (i == 0 && csv_enabled == 1)
            fprintf(csv, "Timestamp_ms,Num Pkt,Dimensione Pkt,Throughput_MBps,RTT_ms\n");

        char ping_msg[] = "PING";
        char pong_msg[5] = {0};
        double rtt_ms = 0;

        double ping_start = get_time_microseconds();
        if (send(sock, ping_msg, sizeof(ping_msg), 0) != sizeof(ping_msg))
        {
            perror("[CLIENT] Error sending PING");
        }
        else
        {
            ssize_t r = recv(sock, pong_msg, sizeof(pong_msg), 0);
            if (r != sizeof(pong_msg) || strcmp(pong_msg, "PONG") != 0)
            {
                fprintf(stderr, "[CLIENT] Invalid PONG response\n");
            }
            else
            {
                double ping_end = get_time_microseconds();
                rtt_ms = (ping_end - ping_start) / 1000.0;
                // printf("[CLIENT] RTT misurato: %.3f ms\n", rtt_ms);
            }
        }

        int packet_length = chunk_size; //!!!!!!! negoziato!!!!!!!
        int header_bytes = 40;          // TCP/IP standard header
        int payload_bytes = chunk_size;
        double efficiency = ((double)payload_bytes / (payload_bytes + header_bytes)) * 100.0;
        double packet_to_send = (double)block_size / (double)packet_length;

        if (!formatting_output_csv)
        {
            printf("\n[SUMMARY RUN %d/%d]\n", i + 1, repetitions);
            printf("  Data Block:         %.3f MB\n", block_size / 1.024e6);
            printf("  Protocol:           IPv4\n");
            printf("  Packet Length:      %d bytes\n", packet_length);
            printf("  Header:             %d bytes\n", header_bytes);
            printf("  Efficiency:         %.3f %%\n", efficiency);
            printf("  Pkts to send:     %.3f\n", packet_to_send); // numero pacchetti da inviare
        }

        // printf("[CLIENT] Sending %ld bytes in chunks of %d bytes...\n", block_size, chunk_size);

        int packet_num = 0;
        int total_sent = 0;
        double start_time = get_time_microseconds();
        // double last_time = start_time;

        while (total_sent < block_size)
        {
            int to_send = block_size - total_sent;
            if (to_send > chunk_size)
                to_send = chunk_size;

            ssize_t sent = send(sock, buffer, to_send, 0);
            if (sent <= 0)
                break;

            // double now = get_time_microseconds();
            // double delta_us = now - last_time;
            // double delta_sec = delta_us / 1e6;
            // double throughput_MBps = (sent / 1e6) / (delta_sec > 0 ? delta_sec : 1e-9);
            // printf("[+%.6f s] Sent %zd bytes (%.3f MB/s)\n", (now - start_time) / 1e6, sent, throughput_MBps);
            //  Salvare i dati in memoria e salvare sul file al termine del processo di trasferimento
            /*
            if(csv_enabled==1){
                fprintf(csv, "%.3f,%d,%zd,%.4f,%.6f\n", (now - start_time) / 1000, packet_num, sent, throughput_MBps, rtt_ms);
            }
            */
            total_sent += sent;
            // last_time = now;
            packet_num++;
        }

        double end_time = get_time_microseconds();
        double elapsed_ms = (end_time - start_time) / 1000; // us -> ms

        // Dopo aver effettuato il test, posso ottenere il numero di pacchetti persi soltanto ricevendo il numero effettivo che ha raggiunto il server
        // Ovvero il server risponde con il numero effettivo di pacchetti che ha ricevuto!!! ( per testare protocolli stateless ad esempio UDP, nel caso di TCP senza accedere al raw socket!!!)

        double avg_throughput_MBps = (total_sent / 1.024e6) / (elapsed_ms / 1000); // Ottengo Mega byte per secondo
        double avg_throughput_Mbps = avg_throughput_MBps * 8;                      // Ottengo Mega bit per secondo

        double throughput_pps = packet_num / (elapsed_ms / 1000);


*/