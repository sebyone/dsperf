
#include "capacity.h"

// #include <cstdio>
// #include <unistd.h>
// #include <stdio.h>

#include <stdio.h>
#include <string.h>

ret_t resetVars(capacity_vars_t &_cvars)
{
    memset(&_cvars, 0, sizeof(capacity_vars_t));
    return rtOk;
}

// ------------------------------------------------------------------------------------------------------------------------------!
ret_t report_capacity(const model_info_t &info_, capacity_vars_t &vars_, report_item_t switch_)
{
    switch (switch_)
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
        printf("Transfer testing_time [ms]\t");
        printf("Throughput [MB/s]\t[Mbps]\t[pps]\n");
        break;

    case _OUTS_CSV_ROW:
        printf("%d//%d\t", vars_.tstcounter);
        printf("%.3f\t", _Byte2Megabyte(vars_.blocksize));
        printf("%s\t", *(info_.title));
        printf("%d\t", vars_.pktpayload);
        printf("%d\t", vars_.pktheader);
        printf("%.3f\t", vars_.pktefficiency);
        printf("%.3f\t", vars_.pktstosend);               // numero pacchetti da inviare
        printf("%d\t", vars_.pktssent);                   // numero pacchetti inviati
        printf("%d\t", vars_.pktsloss);                   //
        printf("%.3f\t", _Byte2Megabyte(vars_.datasent)); // Mega bytes
        // printf("  Data Lost:         %d bytes\n", total_lost);
        printf("%.3f\t", vars_.pktsloss / vars_.pktssent * 100);
        printf("%.3f\t", vars_.totaltime);
        printf("%.3f\t%.3f\t%.3f\n", vars_.throughput, vars_.throughput / 8, (vars_.totaltime / 1000) / vars_.pktssent);
        break;

    case _OUTS_SUMMARY:
        printf("\n[SUMMARY RUN %d/%d]\n", (int)vars_.tstcounter, 1); // Sitemare !!!!!!!!!!!!!!!!!!!!!!!!!!!
        printf("  Data Block:         %.3f MB\n", _Byte2Megabyte(vars_.blocksize));
        printf("  Protocol:           %s\n", *(info_.title));
        // printf("  Packet Length:      %d bytes\n", vars[_pktlength]);
        printf("  Header:             %f bytes\n", vars_.pktheader);
        printf("  Efficiency:         %.3f %%\n", vars_.pktefficiency);
        printf("  Pkts to send:     %.3f\n", vars_.pktstosend); // numero pacchetti da inviare
        break;

    case _OUTS_SUMMARY_ROW:
        printf("  Pkt sent: %d\n", vars_.pktssent);        // numero pacchetti inviati
        printf("  Pkt loss: %d\n", vars_.pktsloss);        //
        printf("  Data Sent: %d bytes\n", vars_.datasent); // Mega bytes
        // printf("  Data Lost:         %d bytes\n", total_lost);
        // printf("  Pkt Err. %%:      %.3f %%\n", out_bytes_lost_percents);
        printf("  Transfer Time: %.3f ms\n", vars_.totaltime);
        printf("  Throughput: %.3f Mbps\n", vars_.throughput);
        // printf("  Throughput:         %.3f MB/s     | %.3f Mbps\n", out_throughput_Mbytes, out_throughput_Mbits);
        // printf("  Throughput (pps):   %.3f pps\n", out_throughput_pps);
        // printf("[CLIENT] Transfer Time: %.3f ms | Total Bytes: %d | Throughput: %.3f MB/s (%.3f Mbps)\n",
        //        elapsed_ms, bytes_sent, out_throughput_Mbytes, out_throughput_Mbits);
        break;
    }
}