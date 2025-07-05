
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
        printf("\nCapacity Test:\n");
        printf("    Tester:         %s\n", info_.title);
        printf("                    %s\n", info_.info);
        printf("    Runs            #%d\n", vars_.tstcounter); // Sitemare !
        printf("    Pkt header:     %.0f [Bytes]\n", vars_.pktheader);
        printf("    Pkt payload:    %.0f [Bytes]\n", vars_.pktpayload);
        printf("    Efficiency:     %.2f [%%]\n", vars_.pktefficiency);
        printf("    Pkts to send:   %.0f\n", vars_.pktstosend); // numero pacchetti da inviare
        printf("    Pkts sent:      %.0f\n", vars_.pktssent);   // numero pacchetti inviati
        printf("    Pkts loss:      %.0f\n", vars_.pktsloss);   //
        printf("    Data to send:   %.3f [MB]\n", _Byte2Megabyte(vars_.blocksize));
        printf("    Data Sent:      %.0f [Bytes]\n", vars_.datasent); // Mega bytes
        printf("    Data Lost:      %.0f [Bytes]\n", vars_.dataloss);
        printf("    Errors:         %.2f [%%]\n", vars_.tsterrors);
        printf("    Set-up time:    %.2f [ms]\n", vars_.setuptime);
        printf("    Trip time:      %.2f [ms]\n", vars_.triptime);        
        printf("    Total time:     %.2f [ms]\n", vars_.totaltime);
        printf("    Jitter:         %.2f [%]\n", (vars_.jitter));
        printf("    Throughput:     %.2f [Mbps]\n", vars_.throughput);
        printf("                    %.2f [MB/s]\n", vars_.throughput / 8.0);
        printf("                    %.2f [pps]\n", (vars_.totaltime * 1000.0) / vars_.pktssent);
        printf("    Bandwidth:      %.2f [Mbps]\n", vars_.bandwidth);
        printf("    Saturation:     %.2f [%]\n", (vars_.saturation));
        printf("\n");
        break;
    }
    return rtOk;
}