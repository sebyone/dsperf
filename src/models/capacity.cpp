
#include "capacity.h"

// #include <cstdio>
// #include <unistd.h>
// #include <stdio.h>

#include <stdio.h>
#include <string.h>

rt_t resetVars(capacity_vars_t &_cvars)
{
    memset(&_cvars, 0, sizeof(capacity_vars_t));
    return rtOk;
}

// ------------------------------------------------------------------------------------------------------------------------------!
rt_t report_capacity(const model_info_t &info_, capacity_vars_t &vars_, report_item_t switch_)
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
        printf("\nCapacity Test summary:\n");
        printf("    Tester:            %s\n", *(info_.title));
        printf("                       %s\n", *(info_.info));
        printf("    Run [%d/%d]\n", vars_.tstcounter, 0); // Sitemare !
        printf("    Data Block:      %.3f [MB]\n", _Byte2Megabyte(vars_.blocksize));
        printf("    Pkt payload:      %d  [Bytes]\n", vars_.pktpayload);
        printf("    Pkt header:       %f [Bytes]\n", vars_.pktheader);
        printf("    Efficiency:     %.3f [%%]\n", vars_.pktefficiency);
        printf("    Pkts to send:   %.3f\n", vars_.pktstosend);       // numero pacchetti da inviare
        printf("    Pkts sent:        %d\n", vars_.pktssent);         // numero pacchetti inviati
        printf("    Pkts loss:        %d\n", vars_.pktsloss);         //
        printf("    Data Sent:        %d [Bytes]\n", vars_.datasent); // Mega bytes
        printf("    Data Lost:        %d [Bytes]\n", vars_.dataloss);
        printf("    Errors:         %.3f [%%]\n", vars_.tsterrors);
        printf("    Transfer Time:  %.3f [ms]\n", vars_.totaltime);
        printf("    Throughput:     %.3f [Mbps]\n", vars_.throughput);
        printf("                    %.3f [MB/s]\n", vars_.throughput / 8);
        printf("                    %.3f [pps]\n", (vars_.totaltime / 1000) / vars_.pktssent);
        printf("    Bandwidth:      %.3f [Mbps]\n", vars_.bandwidth);
        printf("\n");
        break;
    }
    return rtOk;
}