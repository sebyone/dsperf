#include "model.h"
#include <cstdio>


ret_t get_hwif_ipv4tcp(lnetif_t *ifs)
{

    return rtOk;
}

void report_capacity(pmodel_info_t model_, capacity_vars_t &vars_, report_item_t switch_)
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
        printf("%d//%d\t", vars_[_tstcounter]);
        printf("%.3f\t", _Byte2Megabyte(vars_[_blocksize]));
        printf("%s\t", *(model_->title));
        printf("%d\t", vars_[_pktpayload]);
        printf("%d\t", vars_[_pktheader]);
        printf("%.3f\t", vars_[_efficiency]);
        printf("%.3f\t", vars_[_pktstosend]);               // numero pacchetti da inviare
        printf("%d\t", vars_[_pktssent]);                   // numero pacchetti inviati
        printf("%d\t", 0);                                  //
        printf("%.3f\t", _Byte2Megabyte(vars_[_datasent])); // Mega bytes
        // printf("  Data Lost:         %d bytes\n", total_lost);
        printf("%.3f\t", vars_[_pktsloss] / vars_[_pktssent] * 100);
        printf("%.3f\t", vars_[_ttime]);
        printf("%.3f\t%.3f\t%.3f\n", vars_[_throughput], vars_[_throughput] / 8, (vars_[_ttime] / 1000) / vars_[_pktssent]);
        break;

    case _OUTS_SUMMARY:
        printf("\n[SUMMARY RUN %d/%d]\n", (int)vars_[_tstcounter], (int)vars_[_tstcounter]);
        printf("  Data Block:         %.3f MB\n", _Byte2Megabyte(vars_[_blocksize]));
        printf("  Protocol:           %s\n", *(model_->title));
        // printf("  Packet Length:      %d bytes\n", vars[_pktlength]);
        printf("  Header:             %f bytes\n", vars_[_pktheader]);
        printf("  Efficiency:         %.3f %%\n", vars_[_efficiency]);
        printf("  Pkts to send:     %.3f\n", vars_[_pktstosend]); // numero pacchetti da inviare
        break;

    case _OUTS_SUMMARY_ROW:
        printf("  Pkt sent: %d\n", vars_[_pktssent]);        // numero pacchetti inviati
        printf("  Pkt loss: %d\n", vars_[_pktsloss]);        //
        printf("  Data Sent: %d bytes\n", vars_[_datasent]); // Mega bytes
        // printf("  Data Lost:         %d bytes\n", total_lost);
        // printf("  Pkt Err. %%:      %.3f %%\n", out_bytes_lost_percents);
        printf("  Transfer Time: %.3f ms\n", vars_[_ttime]);
        printf("  Throughput: %.3f Mbps\n", vars_[_throughput]);
        // printf("  Throughput:         %.3f MB/s     | %.3f Mbps\n", out_throughput_Mbytes, out_throughput_Mbits);
        // printf("  Throughput (pps):   %.3f pps\n", out_throughput_pps);
        // printf("[CLIENT] Transfer Time: %.3f ms | Total Bytes: %d | Throughput: %.3f MB/s (%.3f Mbps)\n",
        //        elapsed_ms, bytes_sent, out_throughput_Mbytes, out_throughput_Mbits);
        break;
    }
}