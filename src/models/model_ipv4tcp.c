
#include "model_ipv4tcp.h"

#if defined(__linux__) || defined(__RASP__)
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#endif

extern Settings; // options_t Settings;
double vars[13]; // computed values for enum ipv4tcp_vars

/*
struct // model_ipv4_tcp report
{
    // char *test_model = TEST_MODEL_NAME; // Setted
    hwif_t interface;    // test
    int repetitions;     // test executions
    long int block_size; // "data to send" traffic block size [MB]

    // char *model_info = TEST_MODEL_NAME; // Setted
    int pkt_Length; //  packet size header included [bytes]
    int pkt_header; // Header [bytes]   - 17 bytes IP header + sub-protocol options 0..34 bytes
    // Efficiency [%]   - ratio: [%] = payload / total_packet_size ( header+payload )

    int pkts_to_send; // Pkts to send   - trunc(blocksize / packet_payload) + (blocksize % packet_payload)
    int pkts_sent;    // Pkt sent       - counter of packet really sended ( check socket buffering settings !!!!!!!)
    int pkts_loss;    // Pkt loss       - ???????????????????????
    int data_sent;    // Data Sent [MB] -

    long int test_time;       // Transfer Time [ms]
    long int test_throughput; // Throughput [MBps]  [Mbps] [pps]
    double err_pkts_p;        //   // Pkt Err.[%]

} model_data;
*/

typedef enum
{
    _OUTS_CSV_HEADER = 1, // header
    _OUTS_CSV_ROW,        // simple line
    _OUTS_SUMMARY,
    _OUTS_SUMMARY_ROW

} frm_stuffs_e;

// -------------------------------------------------------------------------------------------------------- !
ret_t ipv4tcp_bandwidth(int ifn); // computes bandwidth for local interface
{
}

// -------------------------------------------------------------------------------------------------------- !
void print_outs(frm_stuffs_e _switch)
{
    switch (_switch)
    {
    case _OUTS_CSV_HEADER:
        printf("#/#\t");
        printf("Data Block [MB]\t");
        printf("Protocol\t");
        printf("Payload [bytes]\t");
        printf("Header [bytes]\t");
        printf("Efficiency[%%]\t");
        printf("Pkts to send\t");  // numero pacchetti da inviare
        printf("Pkts sent\t");     // numero pacchetti inviati
        printf("Pkts loss\t");     //
        printf("Data sent[MB]\t"); // Mega bytes
        printf("Pkt Err.[%%]\t");
        printf("Transfer time [ms]\t");
        printf("Throughput [MB/s]\t[Mbps]\t[pps]\n");
        break;

    case _OUTS_CSV_ROW:
        printf("'%d/%d\t", vars[_tstcounter]);
        printf("%.3f\t", _Byte2Megabyte(vars[_blocksize]));
        printf("%s\t", TEST_MODEL_NAME);
        printf("%d\t", vars[_pktpayload]);
        printf("%d\t", vars[_pktheader]);
        printf("%.3f\t", vars[_efficiency]);
        printf("%.3f\t", vars[_pktstosend]);               // numero pacchetti da inviare
        printf("%d\t", vars[_pktssent]);                   // numero pacchetti inviati
        printf("%d\t", 0);                                 //
        printf("%.3f\t", _Byte2Megabyte(vars[_datasent])); // Mega bytes
        // printf("  Data Lost:         %d bytes\n", total_lost);
        printf("%.3f\t", vars[_pktsloss] / vars[_pktssent] * 100);
        printf("%.3f\t", vars[_ttime]);
        printf("%.3f\t%.3f\t%.3f\n", vars[_throughput], vars[_throughput] / 8, (vars[_ttime] / 1000) / vars[_pktssent]);
        break;

    case _OUTS_SUMMARY:
        printf("\n[SUMMARY RUN %d/%d]\n", i + 1, vars[_tstcounter]);
        printf("  Data Block:         %.3f MB\n", _Byte2Megabyte(vars[_blocksize]));
        printf("  Protocol:           %s\n", TEST_MODEL_NAME);
        printf("  Packet Length:      %d bytes\n", vars[_pktlength]);
        printf("  Header:             %d bytes\n", vars[_pktheader]);
        printf("  Efficiency:         %.3f %%\n", vars[_efficiency]);
        printf("  Pkts to send:     %.3f\n", vars[_pktstosend]); // numero pacchetti da inviare
        break;

    case _OUTS_SUMMARY_ROW:
        printf("  Pkt sent: %d\n", vars[_pktssent]);        // numero pacchetti inviati
        printf("  Pkt loss: %d\n", vars[_pktsloss]);        //
        printf("  Data Sent: %d bytes\n", vars[_datasent]); // Mega bytes
        // printf("  Data Lost:         %d bytes\n", total_lost);
        // printf("  Pkt Err. %%:      %.3f %%\n", out_bytes_lost_percents);
        printf("  Transfer Time: %.3f ms\n", vars[_ttime]);
        printf("  Throughput: %.3f Mbps\n", vars[_throughput]);
        // printf("  Throughput:         %.3f MB/s     | %.3f Mbps\n", out_throughput_Mbytes, out_throughput_Mbits);
        // printf("  Throughput (pps):   %.3f pps\n", out_throughput_pps);

        // printf("[CLIENT] Transfer Time: %.3f ms | Total Bytes: %d | Throughput: %.3f MB/s (%.3f Mbps)\n",
        //        elapsed_ms, bytes_sent, out_throughput_Mbytes, out_throughput_Mbits);
        break;
    }
}

// -------------------------------------------------------------------------------------------------------- !
ret_t check_ipv4tcp()
{
    char ip[64];
    int port;
    const char *host = test->remote_addr;
    const char *colon = strchr(host, ':');
    if (colon == NULL)
    {
        fprintf(stderr, "[CLIENT] Invalid host format. Use IP:PORT\n");
        return;
    }

    size_t ip_len = colon - host;
    strncpy(ip, host, ip_len);
    ip[ip_len] = '\0';
    port = atoi(colon + 1);

    if (port < MIN_PORT || port > MAX_PORT)
    {
        fprintf(stderr, "[CLIENT] Invalid port number: %d\n", port);
        return retErr;
    }

    if (!Settings->csv_format) // verbose mode options !!!
    {
        pverbose("dsperf started in client mode, loopback at  %s:%d \n", ip, port); // with %s size %d\n", ip, port, "block", test->block_size);
    }
   return retOk; 
}

// -------------------------------------------------------------------------------------------------------- !
ret_t run_ipv4_tcp_client(const char *_server_ip, int _server_port)
{
    // size_t block_size = _test->block_size;
    // size_t mtu = _test->pkt_payload;

    // int time = Settings->time;
    // bool __flag_time_defined = Settings->time_defined;

    // Bandwidth ?????????????????????????
    // const uint64_t target_bitrate_bps = 10 * 1024 * 1024 * 8; // 10 Mbps (Megabits) _10Mps

    // related to model's stuffs
    int pkt_payload_size;
    int pkt_header_size = 40; // TCP/IP standard header
    double efficiency;
    double pkts_to_send;

    int mss = 0; // packet's payload size
    char *pkt_payload;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        pverbose("Can't OPEN socket error!");
        return (rtErr);
    }

    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(_server_port),
    };
    inet_pton(AF_INET, _server_ip, &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        pverbose("Can't CONNECT socket error!");
        close(sock);
        return (rtErr);
    }

    // Packet length settings (socket MSS negotiation !!!)

    socklen_t optlen = sizeof(vars[_pktpayload]); // mss = PAYLOAD !!!!!!!!!!!!!

    if (!Settings->mtu_specified)
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
    if (vars[_pktpayload] >= block_size)
    {
        vars[_pktstosend] = 1;
    }
    else
    {
        vars[_pktstosend] = trunc(block_size / vars[_pktpayload]) + (block_size % (int)vars[_pktpayload]);
    }
    vars[_efficiency] = vars[_pktpayload] / (vars[_pktpayload] + vars[_pktheader]) * 100.0;

    // Allocates a packet's size buffer
    char *packet = (char *)malloc((int)vars[_pktpayload]);
    memset(packet, 'A', (int)vars[_pktpayload]);

    // Performs one or many tests...
    if (Settings->csv_format && !Settings->csv_no_header)
    {
        print_outs(_OUTS_CSV_HEADER);
    }

    vars[_tstcounter] = 0;
    while (vars[_tstcounter]++ < Settings->repetitions)
    {
        vars[_pktssent] = 0;
        int bytes2send = vars[_blocksize];
        double start_time = get_time_microseconds();
        while (bytes2send)
        {
            if (bytes2send >= vars[_pktpayload])
            {
                bytes2send -= send(sock, packet, (int)vars[_pktpayload], 0);
            }
            else
            {
                bytes2send -= send(sock, packet, bytes2send, 0);
            }
            vars[_pktssent]++;
        }

        // Computes vars
        vars[_ttime] = (get_time_microseconds() - start_time) / 1000; // ms
        vars[_datasent] = vars[_blocksize];
        vars[_throughput] = _Byte2Megabits(vars[_datasent]) / vars[_ttime] / 1000; // [Mbps]

        // Prints out vars
        if (Settings->csv_format)
        {
            print_outs(_OUTS_SUMMARY);
        }
        else
        {
            print_outs(_OUTS_CSV_ROW);
        }
    }

    free(packet);
    close(sock);
}

// -------------------------------------------------------------------------------------------------------- !
// dual mode: drop packets / send back packets !!!!!!!!!!
//
ret_t run_ipv4tcp_server(int port)
{
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        return -1;
    }

    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        close(server_sock);
        return;
    }

    if (listen(server_sock, 1) < 0)
    {
        pverbose("Socket listen");
        close(server_sock);
        return;
    }

    pverbose("[SERVER] In ascolto sulla porta %d...\n", port);

    int run_count = 0;
    while (1)
    {
        run_count++;
        pverbose("\n========== Waiting for connection %d (port %d) ==========\n", run_count, port);

        int client_sock = accept(server_sock, NULL, NULL);
        if (client_sock < 0)
        {
            perror("accept");
            continue;
        }

        int mss = 0;
        socklen_t optlen = sizeof(mss);
        if (getsockopt(client_sock, IPPROTO_TCP, TCP_MAXSEG, &mss, &optlen) != 0)
        {
            // printf("[SERVER] MSS negotiated: %d bytes\n", mss);
            perror("not load mss");
        }

        char *buffer = (char *)malloc(mss > 0 ? mss : 2048);
        if (!buffer)
        {
            perror("[SERVER] malloc failed");
            close(client_sock);
            continue;
        }

        int total_received = 0;
        double start = get_time_microseconds();

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

        while (1)
        {
            ssize_t recvd = recv(client_sock, buffer, mss > 0 ? mss : 2048, 0); // Negozia 2048 ?
            if (recvd <= 0)
                break;
            total_received += recvd;
        }

        double end = get_time_microseconds();
        double seconds = (end - start) / 1e6;
        double throughput = (seconds > 0) ? (total_received / (1024.0 * 1024.0) / seconds) : 0;

        pverbose("[SERVER] Run %d - Time: %.6f s | Bytes: %d | Throughput: %.3f MB/s\n",
                 run_count, seconds, total_received, throughput);
        free(buffer);
        close(client_sock);
    }
    close(server_sock);
}

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

*/

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