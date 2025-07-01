
#include "options.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>

#include "version.h"

#include "models/model_ipv4tcp.h"
#include "models/model_daasfrs.h"
//

options_t Options; // global.h

#define DSPERF_OPTIONS "S:s:b:n:i:c:m:f:t:y:v:V:h"

static struct option long_options[] = { // "--" (https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html)
    {"ipv4", no_argument, 0, 1},
    {"daas", required_argument, 0, 2},
    {"capacity", required_argument, 0, 3},
    {"help", no_argument, 0, 4},
    {0, 0, 0, 0}}; //

// -------------------------------------------------------------------------------------------------------- !
void clearOptions()
{
    memset(&Options, 0, sizeof(options_t));

    // Model
    Options.model_protocol = _PROTO_NONE; // Test protocol (IPV4, DAAS, etc.etc.)
    Options.model_class = __unsetted;     // Test class (Features, Capacity, etc.etc.)

    // Run mode
    Options.host_role = _ROLE_NONE; // client or loopback server
    Options.repetitions = 1;        // repetitions counter

    // Class: Caqpacity
    Options.block_size = 0;  // Traffic by block-size
    Options.pkts_num = 0;    // Traffic by packet number
    Options.pkt_payload = 0; // Packet payload size

    // Formatting
    Options.csv_enabled = false;   // print out in csv format
    Options.csv_no_header = false; // print out csv columns header
    Options.csv_path[0] = '\0';    // output file (default 'stdout')

    // Addressing
    Options.service_num = 0;       // Service identifier (port for ipv4 stack)
    Options.local_addr[0] = '\0';  // Local interface and address
    Options.remote_addr[0] = '\0'; // Remote address to connect (used in client mode only)
}

// -------------------------------------------------------------------------------------------------------- !
void print_credits()
{
    printf("\ndsperf  %d.%d.%d\n", PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, PROJECT_VERSION_PATCH);
    printf("2024-2025 (@) Sebyone Srl\n");
    printf("License MPL v.2.0 <https://mozilla.org/MPL/2.0/>\n");
    printf("This Software is provided under this License on an 'as is' basis, without warranty of any kind.\n");
    printf("This disclaimer of warranty constitutes an essential part of this License.\n");
    printf("\n");
}

void print_usage() // const char *prog_name
{
    const char *prog_name = "dsperf";
    printf("Usage:\n");
    printf("\n");
    printf("  Loopback server: %s -S <local_ip:port> --ipv4|--daas -V\n", prog_name);
    printf("\n");
    printf("  Tester client: %s -s <server_ip:port> [-b <blocksize> | -p <packet-size> -c <count> ] -n <repetitions>  [ --ipv4 | --daas ]  \n", prog_name);
    printf("\n");
}

// -------------------------------------------------------------------------------------------------------- !
void print_help()
{
    printf("\nGeneral options\n");
    printf("  -v                                Show dsperf details and exit\n");
    printf("  -h, --help                        Show usage informations\n");
    printf("  -l                                Lists local interface (require protocol specifications)\n");
    printf("  -x                                Disable all system's messages\n");
    printf("  -V                                Enable messages for the looping server events\n");

    printf("\nRun mode");
    printf("  -S <local_addr>:<service>         Starts loopback server mode (if omitted uses default service/port)\n");
    printf("  -s <remote_addr>:<service>        Starts as client to test on loopback \n");
    printf("  -i <local_interface/driver>       Forces to use a specific hardware interface (use -l to list availables)\n");
    printf("  -n <repetitions>                  Repeats testing\n");
    printf("  -t <period>                       Continuosly testing for time period [seconds] (max 8h)\n");

    printf("\nTraffic generator\n");
    printf("  -b <bytes>                        Generates traffic trasferring data-block sized in bytes\n");
    printf("  -c <count>                        Generates traffic trasferring number of packets with payload size fixed (use '-p')\n");
    printf("  -p <bytes>                        Packet's payload size in bytes (tries to negotiate payload)\n");

    printf("\nProtocol and Model\n");
    printf("  --ipv4                            Uses IPv4/TCP underlay network (RFCs 790-791)\n");
    printf("  --daas                            Uses DaaS overlay \n");

    printf("\n  --capacity (default)\n");
    printf("  --security\n");
    printf("  --features (Capabilities)\n");
    printf("  --continuity (Availability)\n");

    printf("\nReporting\n");
    printf("  -y <0/1>                          Outputs results in csv format: '1' no header, '0' with header (default)\n");
    printf("  -f <filename>                     Outputs testing results in csv format to file (client only)\n");
    printf("\n");

    printf("\tNote: commands in upper-case will be available only in server mode.\n");

    printf("\n");
}

// -------------------------------------------------------------------------------------------------------- !
// Parsing
// -------------------------------------------------------------------------------------------------------- !
ret_t parse_args(int argc, char *argv[]) // Syntax validations
{
    int option_index = 0;
    int c;

    if (argc < 2)
    {
        print_credits();
        print_usage();
        return rtExit;
    }

    clearOptions(); // Options

    while ((c = getopt_long(argc, argv, DSPERF_OPTIONS, long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 'v': //  how dsperf details and exit
            print_credits();
            return rtExit;
            break;

        case 4: // --help
        case 'h':
            print_usage();
            return rtExit;
            break;

        case 'l': // Show local interfaces for protocol
        case 'x': // Disable all system's messages
        case 'V': // Enable messages for the looping server events
            pverbose(" '%s' unsupported options ! \n", c);
            return rtErr;
            break;

        case 'S':
        case 's':
            if (Options.host_role != _ROLE_NONE) // Role already setted
            {
                pverbose("error: can't specify both -S and -s !\n");
                return rtErr;
            }
            size_t salen = strlen(optarg); // <addr_remote>
            if (c == 's')                  // _ROLE_CLIENT
            {
                Options.host_role = _ROLE_CLIENT;
                if (salen > 0 && salen >= _OPT_STR_LEN)
                {
                    pverbose("error: invalid <addr_remote> parameter ! \n");
                    return rtErr;
                }
                strncpy(Options.remote_addr, optarg, salen);
            }
            if (c == 'S') // _ROLE_SERVER
            {
                Options.host_role = _ROLE_SERVER;

                if (salen > 0 && salen >= _OPT_STR_LEN)
                {
                    pverbose("error: invalid <addr_local> parameter ! \n");
                    return rtErr;
                }
                strncpy(Options.local_addr, optarg, salen);
            }
            break;

        case 'i': // Forces to use a specific hardware interface
            pverbose(" '%s' unsupported options ! \n", c);
            return rtErr;
            break;

        case 'n': // Repetitions
            Options.repetitions = atoi(optarg);
            if (Options.repetitions < 1)
            {
                pverbose("Error: repetitions must be >= 1\n");
                return rtErr;
            }
            break;

        case 'b': // Traffic block size
            long bs = atol(optarg);
            if (bs < MIN_BLOCK_SIZE)
            {
                pverbose("Error: blocksize must be >0\n");
                return rtErr;
            }
            else
                Options.block_size = bs;
            break;

        case 'c': // Traffic pkts number
            long pn = atol(optarg);
            if (pn < MIN_PKT_NUMBER)
            {
                pverbose("Error: packet number must be >0\n");
                return rtErr;
            }
            else
                Options.pkts_num = pn;
            break;

        case 'p': // Packet's payload size in bytes
            long pp = atol(optarg);
            if (pp < 1)
            {
                pverbose("error: packet's payload size must be > 0\n");
                return rtErr;
            }
            else
                Options.pkt_payload = pp;
            break;

        case 'f':
            Options.csv_enabled = true;
            strncpy(Options.csv_path, optarg, sizeof(Options.csv_path) - 1);
            break;

        case 't':
            unsigned tt = atoi(optarg);
            if (tt > 1 && tt << 8 * 3600)
                Options.testing_time = tt;
            else
            {
                pverbose("error: testing time period not valid '-t %s' ! \n", optarg);
                return rtErr;
            }
            break;

        case 'y': // Enables output in csv format: '1' no header, '0' with header (default)
            Options.csv_format = true;
            Options.csv_no_header = false;
            if (strlen(optarg) > 1)
            {
                pverbose("error: formatting options '-y %s' unknown ! \n", optarg);
                return rtErr;
            }
            else
                Options.csv_no_header = (atoi(optarg) == 1);
            break;

        case 1: // --ipv4
            Options.model_protocol = _PROTO_IPV4;
            break;

        case 2: // --daas
            Options.model_protocol = _PROTO_DAAS;
            break;

        case 3: // --capacity
            Options.model_class = __Capacity;
            break;

        default:
            pverbose("error: unknow options '%c' !\n", c);
            print_help();
            return rtErr;
        }
    }
    return rtOk;
}

// -------------------------------------------------------------------------------------------------------- !
// Validation
// -------------------------------------------------------------------------------------------------------- !
ret_t validate_model_options(exefunc_t &_pfrun)
{

    if (Options.model_protocol == _PROTO_NONE && Options.host_role == _ROLE_NONE) // Model and Protocol ok !
    {
        pverbose("error: specify protocol and run mode !\n");
        print_usage();
        return rtErr;
    }

    if ((Options.block_size > 0 || (Options.pkts_num > 0 && Options.pkt_payload > 0))) // Model class "Capacity" ok !
    {
        if (Options.model_class == __unsetted)
        {
            Options.model_class == __Capacity; // defaul class capacity
        }
    }
    else // checks for other classes
    {
        pverbose("error: invalid parameters for test class !\n");
        print_help();
        return rtErr;
    }

    if (Options.host_role == _ROLE_CLIENT && (strlen(Options.remote_addr) == 0)) // Client ok !
    {
        pverbose("error: specify remote address !\n");
        print_help();
        return rtErr;
    }

    switch (Options.model_protocol)
    {
    case _PROTO_IPV4:
        set_env_ipv4tcp(Options);
        if (Options.host_role == _ROLE_SERVER)
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
        pverbose("error: libdaas not linked !\n");
        return rtErr;
#endif
    }
    return rtOk;
}
