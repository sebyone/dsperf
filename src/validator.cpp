
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "options.h"
#include "validator.h"

#include "testers/tester_ipv4tcp.h"
#include "testers/tester_daasfrs.h"
//
extern options_t Options; // options.h

ret_t validate_model_options(exefunc_t &_pfrun)
{

    if (Options.model_protocol == _PROTO_NONE && Options.run_mode == _ROLE_NONE) // Run ok !
    {
        pverbose("validator: specify protocol and run mode !\n");
        print_usage();
        return rtErr;
    }

    if ((Options.tst_block_size > 0 || (Options.tst_pkts_num > 0 && Options.pkt_payload > 0))) // Model "Capacity" ok !
    {
        if (Options.model_class == __unsetted)
        {
            Options.model_class == __Capacity; // defaul class capacity
            pverbose("validator: model default (Capacity) \n");
        }
    }
    else // checks for other classes
    {
        pverbose("validator: invalid model parameters !\n");
        print_help();
        return rtErr;
    }

    if (Options.run_mode == _ROLE_SERVER && (strlen(Options.local_addr) == 0)) // Server ok !
    {
        pverbose("validator: specify remote address !\n");
        print_usage();
        return rtErr;
    }

    if (Options.run_mode == _ROLE_CLIENT && (strlen(Options.remote_addr) == 0)) // Client ok !
    {
        pverbose("validator: specify remote address !\n");
        print_usage();
        return rtErr;
    }

    switch (Options.model_protocol)
    {
    case _PROTO_IPV4:
        set_env_ipv4tcp(Options);
        if (Options.run_mode == _ROLE_SERVER)
        {
            _pfrun = &run_server_ipv4tcp;
        }
        else // _ROLE_CLIENT
        {
            _pfrun = &run_client_ipv4tcp; // bandwidth
        };
        break;

    case _PROTO_DAAS:

#ifdef WITH_DAAS
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
