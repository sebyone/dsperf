
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "options.h"
#include "validator.h"

#include "testers/tester_ipv4tcp.h"
#include "testers/tester_daasfrs.h"
//
extern options_t Options; // options.h

// -------------------------------------------------------------------------------------------------------- !
// Validator (semanthic checking)
// -------------------------------------------------------------------------------------------------------- !
rt_t validate_model_options(func_ptr *_pfrun)
{
    bool valid;

#if (1) // DEBUG
    pverbose("DEBUG OPTIONS:\n");
    pverbose("Options.model_protocol = %d \n", Options.model_protocol); // Test protocol (IPV4, DAAS, etc.etc.)
    pverbose("Options.model_class = %d \n", Options.model_class);       // Test class (Features, Capacity, etc.etc.)

    pverbose("Run mode\n");
    pverbose("Options.run_mode = %d \n", Options.run_mode);             // client or loopback server
    pverbose("Options.tst_repeats = %d \n", Options.tst_repeats);       // repetitions counter
    pverbose("Options.tst_time_slot = %d \n", Options.tst_time_slot);   // Test for a time period
    pverbose("Options.tst_block_size = %d \n", Options.tst_block_size); // Traffic by block-size
    pverbose("Options.tst_pkts_num = %d \n", Options.tst_pkts_num);     // Testing for a lot of packet
    pverbose("Options.multilink = %d \n", Options.multilink);           // enable to use concurrent connections

    pverbose("Class: Capacity\n");
    pverbose("Options.pkt_payload = %d\n", Options.pkt_payload); // Packet payload size
    pverbose("Options.bandwidth = %d\n", Options.bandwidth);     // set reference to nominal speed [Mbps]

    pverbose("Formatting\n");
    pverbose("Options.csv_enabled = %d\n", Options.csv_enabled);      // print out in csv format
    pverbose("Options.csv_no_header  = %d\n", Options.csv_no_header); // print out csv columns header
    pverbose("Options.csv_path[0]  = %s\n", Options.csv_path);        // output file (default 'stdout')

    pverbose("Addressing\n");
    pverbose("Options.local_hwif_str = %s\n", Options.local_hwif_str); // Local interface to bind
    pverbose("Options.local_addr  = %s\n", Options.local_addr);        // Local interface and address
    pverbose("Options.remote_addr  = %s\n", Options.remote_addr);      // Remote address to connect (used in client mode only)
    pverbose("Options.service_str  = %s\n", Options.service_str);      // Service identifier (port for ipv4 stack)
    pverbose("Options.service_num  = %d\n", Options.service_num);      // Service identifier (port for ipv4 stack)
#endif // DEBUG

    if (Options.model_protocol == _PROTO_NONE || Options.run_mode == _ROLE_NONE) // Invad PROTO or ROLE !
    {
        pverbose("validator: needs <protocol> and <mode> !\n");
        print_usage();
        return rtErr;
    }

    if (Options.model_class == __unsetted) // default model 'capacity' !
    {
        Options.model_class == __Capacity;
        pverbose("validator: model to default (Capacity) \n");
    }

    switch (Options.model_protocol)
    {
    case _PROTO_IPV4:
        if (Options.run_mode == _ROLE_SERVER)
        {
            valid = (strlen(Options.local_addr) > 0);
            if (!valid) // Server error !
            {
                pverbose("validator: too few parameters to run Loopback server !\n");
                print_help();
                return rtErr;
            }
            *_pfrun = run_server_ipv4tcp;
        }
        else // _ROLE_CLIENT
        {
            valid = (strlen(Options.remote_addr) > 0);                                                                // remote_addr
            valid = valid && ((Options.tst_block_size > 0) || (Options.tst_pkts_num > 0 && Options.pkt_payload > 0)); // blocksize OR pkt_num AND pkt_size

            if (!valid) // Client error !
            {
                pverbose("validator: too few parameters to run Tester Client !\n");
                print_help();
                return rtErr;
            }
            *_pfrun = run_client_ipv4tcp; // bandwidth
        };
        return set_env_ipv4tcp(Options);
        break;

    case _PROTO_DAAS:
#ifdef LIBDAAS_V19
        set_env_daasfrs(Options);
        if (Options.host_role == _ROLE_SERVER)
        {
            _pfrun = &run_server_daasfrs();
        }
        else // _ROLE_CLIENT
        {
            _pfrun = &run_client_daasfrs(); // bandwidth
        };
        break;
#else
        pverbose("validator: libdaas not linked !\n");
        return rtErr;
#endif
    }
    return rtOk;
}
