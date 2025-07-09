
#include "capacity.h"

#include <stdio.h>
#include <time.h>
#include <string.h>

rt_t resetVars(capacity_vars_t &_cvars)
{
    memset(&_cvars, 0, sizeof(capacity_vars_t));
    return rtOk;
}

// ------------------------------------------------------------------------------------------------------------------------------!
rt_t report_capacity(const model_info_t *info_, capacity_vars_t *vars_, report_item_t switch_)
{
    time_t rawtime;
    struct tm *timeinfo;

    switch (switch_)
    {
    case _OUTS_CSV_HEADER:
        printf("#/#\t");
        printf("Data-Block [MB]\t");
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
        printf("%d//%d\t", vars_->tstcounter);
        printf("%.3f\t", _Bytes2Megabytes(vars_->blocksize));
        printf("%d\t", info_->nproto);
        printf("%.2ft", vars_->pktpayload);
        printf("%.2f\t", vars_->pktheader);
        printf("%.3f\t", vars_->pktefficiency);
        printf("%.3f\t", vars_->pktstosend);                 // numero pacchetti da inviare
        printf("%0f\t", vars_->pktssent);                    // numero pacchetti inviati
        printf("%0f\t", vars_->pktsloss);                    //
        printf("%.3f\t", _Bytes2Megabytes(vars_->datasent)); // Mega bytes
        // printf("  Data Lost:         %d bytes\n", total_lost);
        printf("%.3f\t", vars_->pktsloss / vars_->pktssent * 100);
        printf("%.3f\t", vars_->trasftime);
        printf("%.3f\t%.3f\t%.3f\n", vars_->throughput, vars_->throughput / 8, (vars_->trasftime / 1000) / vars_->pktssent);
        break;

    case _OUTS_SUMMARY:
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        printf("\nCapacity report  %s\n", asctime(timeinfo));
        printf("    Test:           %s\n", info_->name);
        printf("                    %s\n", info_->info);
        printf("                    %s\n", info_->team);
        printf("    Protocol:       %d\n", info_->nproto);
        printf("    Test model:     %s\n", __MODELNAME(info_->tmodel));
        printf("    Compliance:     %d\n", info_->compid);

        printf("    Test by Size:   %.3f [MB]\n", _Bytes2Megabytes(vars_->blocksize));
        printf("    Test by Time:   %.0f\n", vars_->timeslot);   // numero pacchetti da inviare
        printf("    Test by Pkts:   %.0f\n", vars_->pktstosend); // numero pacchetti da inviare
        printf("    Prot.header:    %.0f [Bytes]\n", vars_->pktheader);
        printf("    Prot.payload:   %.0f [Bytes]\n", vars_->pktpayload);
        printf("    Prot.eff.:      %.2f [%%]\n", vars_->pktefficiency);
        printf("    Interface MTU:  %.0f [Bytes]\n", vars_->pktmtu);
        printf("    Fragmentation:  %.2f [%%]\n", vars_->fragmentation);
        printf("    Bandwidth:      %.2f [Mbps]\n", vars_->bandwidth);

        printf("\n    Execution       #%d\n", vars_->tstcounter); // Sitemare !

        printf("    Pkts sent:      %.0f\n", vars_->pktssent); // numero pacchetti inviati
        printf("    Pkts rcvd:      %.0f\n", vars_->pktsrcvd); // numero pacchetti ricevuti
        printf("    Pkts loss:      %.0f\n", vars_->pktsloss); //

        printf("    Data sent:      %.0f [Bytes]\n", vars_->datasent); // Mega bytes
        printf("    Data rcvd:      %.0f [Bytes]\n", vars_->datarcvd); // Mega bytes
        printf("    Data loss:      %.0f [Bytes]\n", vars_->dataloss);
        printf("    Errors:         %.2f [%%]\n", vars_->tsterrors);
        printf("    Test time:      %.3f [ms]\n", vars_->trasftime);
        printf("    Open time:      %.3f [ms]\n", vars_->setuptime);
        printf("    Trip time:      %.3f [ms]\n", vars_->rtriptime);
        printf("    Jitter:         %.3f [ms]\n", (vars_->jitter));
        printf("    Latency:        %.3f [ms]\n", (vars_->latyency));
        printf("    Throughput:     %.2f [Mbps]\n", vars_->throughput);
        printf("                    %.2f [MB/s]\n", vars_->throughput / 8.0);
        printf("                    %.2f [pps]\n", (vars_->trasftime * 1000.0) / vars_->pktssent);
        printf("    Saturation:     %.2f [%%]\n", (vars_->saturation));
        printf("\n");
        break;

    case _OUTS_RAW_INFO:
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        printf("\nCapacity report (%s)\n", asctime(timeinfo));
        printf("    Test:           %s\n", info_->name);
        printf("                    %s\n", info_->info);
        printf("                    %s\n", info_->team);
        printf("    Protocol:       %d\n", info_->nproto);
        printf("    Test model:     %d\n", info_->tmodel);
        printf("    Compliance:     %d\n", info_->compid);

        printf("    Test by Data:   %.3f [MB]\n", _Bytes2Megabytes(vars_->blocksize));
        printf("    Test by Time:   %.0f\n", vars_->timeslot);   // numero pacchetti da inviare
        printf("    Test by Pkts:   %.0f\n", vars_->pktstosend); // numero pacchetti da inviare
        printf("    Pkt header:     %.0f [Bytes]\n", vars_->pktheader);
        printf("    Pkt payload:    %.0f [Bytes]\n", vars_->pktpayload);
        printf("    Efficiency:     %.2f [%%]\n", vars_->pktefficiency);
        // printf("    Interface:      %s \n", vars_->);
        printf("    Interface MTU:  %.0f [Bytes]\n", vars_->pktmtu);
        printf("    Fragmentation:  %.2f [%%]\n", vars_->fragmentation);
        printf("    Bandwidth:      %.2f [Mbps]\n", vars_->bandwidth);
        break;

    case _OUTS_RAW_LINE:
        printf("\n    Execution       #%d\n", vars_->tstcounter); // Sitemare !

        printf("    Pkts sent:      %.0f\n", vars_->pktssent); // numero pacchetti inviati
        printf("    Pkts received:  %.0f\n", vars_->pktsrcvd); // numero pacchetti ricevuti
        printf("    Pkts loss:      %.0f\n", vars_->pktsloss); //

        printf("    Data Sent:      %.0f [Bytes]\n", vars_->datasent); // Mega bytes
        printf("    Data received:  %.0f [Bytes]\n", vars_->datarcvd); // Mega bytes
        printf("    Data Lost:      %.0f [Bytes]\n", vars_->dataloss);
        printf("    Errors:         %.2f [%%]\n", vars_->tsterrors);
        printf("    Trasfer Time:   %.3f [ms]\n", vars_->trasftime);
        printf("    Setup Time:     %.3f [ms]\n", vars_->setuptime);
        printf("    R.Trip Time:    %.3f [ms]\n", vars_->rtriptime);
        printf("    Jitter:         %.3f [ms]\n", (vars_->jitter));
        printf("    Latency:        %.3f [ms]\n", (vars_->latyency));
        printf("    Throughput:     %.2f [Mbps]\n", vars_->throughput);
        printf("    Saturation:     %.2f [%%]\n", (vars_->saturation));
        printf("\n");
        break;
    }
    return rtOk;
}