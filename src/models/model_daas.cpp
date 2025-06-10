
#include "model_daas.h"

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
void run_bandwidth_daas_client(daas_setup_t *daas_setup, program_args_t *test)
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


