
#include "model_daas.h"

#ifdef WITH_DAAS
    daas_setup_t daas_setup;

    if (args.layer_mode == 1)
    {
        if (!parse_daas_ini(args.model_path, &daas_setup))
        {
            fprintf(stderr, "Error: Failed to parse .ini overlay file.\n");
            return EXIT_FAILURE;
        }
    }
#endif

// -------------------------------------------------------------------------------------------------------- !
// Return a node based on the setup.
DaasAPI *setupNode(daas_setup_t *daas_setup, bool csv_format, bool csv_no_header)
{

    // Instantiate a node and set the callback event
    DaasAPI *node = nullptr;
    auto *dummyHandler = new daasEvent(nullptr, csv_format, csv_no_header);
    node = new DaasAPI(dummyHandler, "Dperf");
    static_cast<daasEvent *>(dummyHandler)->setNode(node);

    din_t node_din = daas_setup->local_din;
    printf("Local DIN = %i\n", node_din);

    link_setup_t links = daas_setup->links;
    link_t *link_list = links.links;
    char **uri_list = links.uris;

    map_setup_t maps = daas_setup->maps;
    din_t *remote_dins_list = maps.remote_dins;
    link_t *remote_link_list = maps.remote_links;
    char **remote_uri_list = maps.uris;

    if (node->doInit(100, node_din) != ERROR_NONE)
        return nullptr;

    for (int i = 0; i < MAX_LINKS; i++)
    {
        if (link_list[i] != _LINK_NONE && uri_list[i] != nullptr)
        {
            if (node->enableDriver(link_list[i], uri_list[i]) != ERROR_NONE)
                return nullptr;
        }
    }

    for (int i = 0; i < MAX_REMOTE_LINKS; i++)
    {
        if (remote_link_list[i] != _LINK_NONE && remote_uri_list[i] != nullptr && remote_dins_list[i])
        {
            if (node->map(remote_dins_list[i], remote_link_list[i], remote_uri_list[i]) != ERROR_NONE)
                return nullptr;
        }
    }

    if (node->doPerform(PERFORM_CORE_THREAD) != ERROR_NONE)
        return nullptr;

    return node;
}

// --------------------------------------------------------------------------------------------------------
void run_bandwidth_daas_client(daas_setup_t *daas_setup, options_t *test)
{

    int block_size = test->block_size;
    int repetitions = test->repetitions;
    bool csv_format = test->csv_format;
    bool csv_no_header = test->csv_no_header;
    int pack_num = test->pack_num;

    DaasAPI *node = setupNode(daas_setup, csv_format, csv_no_header);
    if (node == nullptr)
        return;

    if (node->listNodes().size() == 0)
        return;

    // first mapped din for semplicity
    /*
    din_t remote_din = node->listNodes()[0];
    printf("Remote Din: %i\n", remote_din);
    */
    din_t remote_din = test->remote_din;

    if (node->locate(remote_din) != ERROR_NONE)
        return;
    for (int i = 0; i < repetitions; i++)
    {
        printf("Sending Test: %i\n", i + 1);
        node->frisbee_dperf(remote_din, pack_num, block_size, 0);
        usleep(1000000);
    }

    return;
}

// --------------------------------------------------------------------------------------------------------
void run_bandwidth_daas_server(daas_setup_t *daas_setup)
{
    DaasAPI *node = setupNode(daas_setup, false, false);
    if (node == nullptr)
        return;

    if (node->listNodes().size() == 0)
        return;

    printf("=========== NODE RECEIVER LISTENING....... ================\n");
    while (true)
    {
        usleep(1000);
    }
}





#ifdef WITH_DAAS
typedef struct {

    link_t links[MAX_LINKS];
    char* uris[MAX_LINKS];

} link_setup_t;

typedef struct {

    din_t remote_dins[MAX_REMOTE_LINKS];
    link_t remote_links[MAX_REMOTE_LINKS];
    char* uris[MAX_REMOTE_LINKS];

} map_setup_t;

typedef struct {

    din_t local_din;
    link_setup_t links;
    map_setup_t maps;

} daas_setup_t;


class daasEvent : public IDaasApiEvent {

    public:

    daasEvent(DaasAPI* node, bool csv_format, bool csv_no_header, bool csv_enabled, char* csv_file,  int* terminator) : node_(node), csv_format_(csv_format), csv_no_header_(csv_no_header), csv_enabled_(csv_enabled), csv_file_(csv_file), terminator_(terminator) {}
    void dinAcceptedEvent(din_t din) override {}
    void ddoReceivedEvent(int payload_size, typeset_t typeset, din_t din) override {
        DDO *pk;
        if (node_->pull(din, &pk) == ERROR_NONE){
            unsigned char * payload = pk->getPayloadPtr();
            printf("Payload: %s\n", payload);
            delete pk;
        }
    }
    void frisbeeReceivedEvent(din_t din) {}
    void nodeStateReceivedEvent(din_t din) {}
    void atsSyncCompleted(din_t din) {} 
    void frisbeeDperfCompleted(din_t din, uint32_t packets, uint32_t block_size){
        auto dperf_info = node_->get_frisbee_dperf_result();

        if (csv_format_ && csv_no_header_)
    {
        printf("#/#\t");
        printf("Data Block [MB]\t");
        printf("Protocol\t");
        printf("Pkt Length [bytes]\t");
        printf("Header [bytes]\t");
        printf("Efficiency[%%]\t");
        printf("Pkts to send\t");  // numero pacchetti da inviare
        printf("Pkt sent\t");      // numero pacchetti inviati
        printf("Pkt loss\t");      //
        printf("Data Sent[MB]\t"); // Mega bytes
        printf("Pkt Err.[%%]\t");
        printf("Transfer Time [ms]\t");
        printf("Throughput [MB/s]\t[Mb/s]\t[pps]\t");
        printf("Sender first timestamp\t");
        printf("Local end timestamp\t");
        printf("Remote first timestamp\t");
        printf("Remote last timestamp\t");
    }


        double error_pct = packets > 0 ? ((double)(packets - dperf_info.remote_pkt_counter) / packets) * 100.0 : 0.0;
        uint64_t elapsed = dperf_info.remote_last_timestamp - dperf_info.sender_first_timestamp;

        double throughput_MBps = 0.0;
        double throughput_Mbps = 0.0;
        double throughput_pps = 0.0;

        if (elapsed > 0) {
            double elapsed_sec = elapsed / 1000.0;
            throughput_MBps = (dperf_info.remote_data_counter / 1.024e6) / elapsed_sec;
            throughput_Mbps = throughput_MBps * 8;
            throughput_pps = dperf_info.remote_data_counter / elapsed_sec;
        }

        if(csv_enabled_){
        bool file_exists = (access(csv_file_, F_OK) == 0);
        
        FILE *file = fopen(csv_file_, "a+");
        if (file != NULL) {
            if (!file_exists) {
                fprintf(file, "#/#\t");
                fprintf(file, "Data_Block_[MB]\t");
                fprintf(file, "Protocol\t");
                fprintf(file, "Pkt_Length_[bytes]\t");
                fprintf(file, "Header_[bytes]\t");
                fprintf(file, "Efficiency[%%]\t");
                fprintf(file, "Pkts_to_send\t");
                fprintf(file, "Pkt_sent\t");
                fprintf(file, "Pkt_loss\t");
                fprintf(file, "Data_Sent[MB]\t");
                fprintf(file, "Pkt_Err.[%%]\t");
                fprintf(file, "Transfer_Time_[ms]\t");
                fprintf(file, "Throughput_[MB/s]\t[Mb/s]\t[pps]\t");
                fprintf(file, "Sender_first_timestamp\t");
                fprintf(file, "Local_end_timestamp\t");
                fprintf(file, "Remote_first_timestamp\t");
                fprintf(file, "Remote_last_timestamp\n");
            }
            
            fprintf(file, "%d\t", 1);
            fprintf(file, "%.3f\t", (double)block_size / 1.024e6);
            fprintf(file, "IPv4\t");
            fprintf(file, "%d\t", dperf_info.remote_pkt_counter > 0 ? (block_size / dperf_info.remote_pkt_counter) : 0);
            fprintf(file, "40\t");
            double efficiency = ((double)block_size / (block_size + 40 * dperf_info.remote_pkt_counter)) * 100.0;
            fprintf(file, "%.3f\t", efficiency);
            fprintf(file, "%.3f\t", (double)block_size / (block_size / (dperf_info.remote_pkt_counter > 0 ? dperf_info.remote_pkt_counter : 1)));
            fprintf(file, "%d\t", dperf_info.remote_pkt_counter);
            fprintf(file, "%d\t", packets - dperf_info.remote_pkt_counter);
            fprintf(file, "%.3f\t", (double)dperf_info.remote_data_counter / 1.024e6);
            fprintf(file, "%.3f\t", error_pct);
            fprintf(file, "%llu\t", (unsigned long long)elapsed);
            fprintf(file, "%.3f\t%.3f\t%.3f\t", throughput_MBps, throughput_Mbps, throughput_pps);
            fprintf(file, "%.3f\t", (double)dperf_info.sender_first_timestamp);
            fprintf(file, "%.3f\t", (double)dperf_info.local_end_timestamp);
            fprintf(file, "%.3f\t", (double)dperf_info.remote_first_timestamp);
            fprintf(file, "%.3f\n", (double)dperf_info.remote_last_timestamp);
            
            fclose(file);
        }
    }

        if (!csv_format_) {
        printf("  Pkt sent: %d\n", packets);
        printf("  Pkt loss: %d\n", packets - dperf_info.remote_pkt_counter);
        printf("  Data Sent:        %d bytes\n", block_size);
        printf("  Pkt Err. %%:      %.3f %%\n", error_pct);
        printf("  Transfer Time:    %llu ms\n", (unsigned long long)elapsed);
        printf("  Throughput:       %.3f MB/s | %.3f Mbps\n", throughput_MBps, throughput_Mbps);
        printf("  Throughput (pps): %.3f pps\n", throughput_pps);
        printf("\n");
        printf("[Node Sender] Transfer Time: %llu ms | Total Bytes: %d | Throughput: %.3f MB/s (%.3f Mbps)\n",
               (unsigned long long)elapsed, dperf_info.remote_data_counter, throughput_MBps, throughput_Mbps);
        printf("[Local First timestamp]=%.3f | [Local End timestamp]=%.3f | [Remote First timestamp]=%.3f | [Remote Last timestamp]=%.3f\n",
             (double)dperf_info.sender_first_timestamp, (double)dperf_info.local_end_timestamp, (double)dperf_info.remote_first_timestamp, (double)dperf_info.remote_last_timestamp);
    } else {
        printf("%d\t", 1);  // es: Run number o id (adatta come vuoi)
        printf("%.3f\t", (double)block_size / 1.024e6); // Data Block in MB
        printf("IPv4\t");  // Protocollo fisso o variabile se serve
        printf("%d\t", dperf_info.remote_pkt_counter > 0 ? (block_size / dperf_info.remote_pkt_counter) : 0);  // Packet Length approx
        printf("40\t"); // Header bytes fisso o calcolato
        double efficiency = ((double)block_size / (block_size + 40 * dperf_info.remote_pkt_counter)) * 100.0;
        printf("%.3f\t", efficiency);
        printf("%.3f\t", (double)block_size / (block_size / (dperf_info.remote_pkt_counter > 0 ? dperf_info.remote_pkt_counter : 1))); // Pkts to send (approx)
        printf("%d\t", dperf_info.remote_pkt_counter);  // Pkt sent
        printf("%d\t", packets - dperf_info.remote_pkt_counter); // Pkt loss
        printf("%.3f\t", (double)dperf_info.remote_data_counter / 1.024e6); // Data sent MB
        printf("%.3f\t", error_pct);
        printf("%llu\t", (unsigned long long)elapsed);
        printf("%.3f\t%.3f\t%.3f\n", throughput_MBps, throughput_Mbps, throughput_pps);
        printf("%.3f\t", (double)dperf_info.sender_first_timestamp);
        printf("%.3f\t", (double)dperf_info.local_end_timestamp);
        printf("%.3f\t", (double)dperf_info.remote_first_timestamp);
        printf("%.3f\t", (double)dperf_info.remote_last_timestamp);

    } 


    if (terminator_ != nullptr) {
        *terminator_ = 1;
     }
    }

    void setNode(DaasAPI* node) {
        node_ = node;
    }
    private:
    DaasAPI* node_;
    bool csv_format_;
    bool csv_no_header_;
    bool csv_enabled_;
    char* csv_file_;
    int* terminator_;

};

#endif


// ====================================== PARSING UTIL ===================================================== //

#ifdef WITH_DAAS
link_t parse_link_type(int val);
bool parse_daas_ini(const char* filepath, daas_setup_t *setup);
#endif