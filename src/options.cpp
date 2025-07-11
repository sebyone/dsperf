
#include "options.h"
#include "version.h"

#include <ctype.h>
#include <stdlib.h>
// #include <string.h>
#include <stdio.h>
#include <getopt.h>

// global options
options_t Options;
void clearOptions();

/*
#define _MAX_STR_LEN 256
#define _N_OPTIONS 12

typedef union {
    long lvalue;
    double dvalue;
    bool bvalue;
    char *svalue[_MAX_OPT_LEN];
} vtype_t;

stypedef struct // Option example: {1,"S", "<local_addr>:<service>","Starts loopback server mode (without...."}
{
    int id;
    bool lopt;
    const char ocode;
    const char* locode;
    const char* args;   // empty set no args
    const char* note;   // brief description
    unsigned short vtype;
    vtype_t value;

} option_t;

typedef enum {
    model_protocol = 1,        // 0 = unset, _PROTO_DAAS,  _PROTO_IPV4, ...
    model_class,           // 0 = unset, Features, Capacity, Continuity, etc.etc.
    run_mode,               // 0 = unset, _ROLE_CLIENT, _ROLE_SERVER, ...
    tst_repeats,                // repeats test, default is 1
    tst_time_slot,             // perform testing for a fixed time period, in seconds
    ....
    ...
}
typedef option_t[_N_OPTIONS] options_t, *poptions_t;
*/

#define _SHORT_OPTIONS "S:s:b:n:i:c:m:f:t:y:vVhw:g:p:x:l"

const static option _LONG_OPTIONS[] = { // (https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html)
    {"help", no_argument, 0, 0},
    {"capacity", no_argument, 0, 1},
    {"features", no_argument, 0, 2},
    {"security", no_argument, 0, 3},
    {"continuity", no_argument, 0, 4},
    {"ipv4", no_argument, 0, 5},
    {"daas", no_argument, 0, 6}, // required_argument,
    {0, 0, 0, 0}};               //

// -------------------------------------------------------------------------------------------------------- !
// Parser (syntax checking)
// -------------------------------------------------------------------------------------------------------- !
rt_t parse_args2options(int argc, char *argv[]) // Syntax validations
{

    int option;
    int loption_index = 0;
    char *psep = NULL;
    size_t tmpsize;
    long tmplong;
    int tmpint;

    if (argc < 2)
    {
        // print_credits();
        print_usage();
        return rtExit;
    }

    clearOptions(); // Options

    while ((option = getopt_long(argc, argv, _SHORT_OPTIONS, _LONG_OPTIONS, &loption_index)) != -1)
    {
        // DEBUG
        // printf("option %c , %d", option, loption_index);
        // printf(" with arg %s\n", optarg);

        switch (option)
        {
        case 0: // --help
            print_help();
            return rtExit;
            break;
        case 1: // --capacity
            Options.model_class = __Capacity;
            break;
        case 2: // --features
        case 3: // --security
        case 4: // --continuity
            Options.model_class = __unsetted;
            break;

        case 5: // --ipv4
            Options.model_protocol = _PROTO_IPV4;
            break;
        case 6: // --daas
            Options.model_protocol = _PROTO_DAAS;
            break;

        case 'v': // short options _____________________ how dsperf details and exit
            print_credits();
            return rtExit;
            break;

        case 'h':
            print_help();
            return rtExit;
            break;

        case 'k': // Disable all system's messages
        case 'g': // Disable all system's messages
            pverbose("parser:  unsupported options '%s' ! \n", option);
            return rtErr;
            break;

        case 'l': // Scans system for interfaces and supported protocols
            Options.listinterfaces = true;
            break;

        case 'V': // Enables messages for the looping server events
            Options.pktverbose = true;
            break;

        case 'x': // Enables concurrent connections
            Options.multilink = atoi(optarg);
            if (Options.multilink < 1)
            {
                pverbose("parser: multi-link not allowed \n");
                return rtErr;
            }
            break;
        case 'w': // set reference to nominal speed [Mbps]
            Options.bandwidth = atol(optarg);
            if (Options.bandwidth < 1)
            {
                pverbose("parser: bandwidth '-w' must be followed by speed in [MB/s] >= 1\n");
                return rtErr;
            }
            break;

        case 'S':
        case 's':
            if (Options.run_mode != _ROLE_NONE) // Role already setted
            {
                pverbose("parser: can't specify both -S and -s !\n");
                return rtErr;
            }

            psep = strchr(optarg, ':'); // Extracts service name/number
            if (psep)
            {
                tmpsize = psep - optarg;
                *psep = '\0';
                ++psep;
                strncpy(Options.service_str, psep, strlen(psep));
                tmplong = atoi(Options.service_str);
                if (tmplong > 0 && Options.service_num == 0)
                    Options.service_num = tmplong;
            }
            else
            {
                tmpsize = strlen(optarg);
            }

            if (option == 's') // _ROLE_CLIENT
            {
                if (tmpsize <= 0 || tmpsize > _MAX_STR_LEN)
                {
                    pverbose("parser: invalid <addr_remote> parameter ! \n");
                    return rtErr;
                }
                Options.run_mode = _ROLE_CLIENT;
                strncpy(Options.remote_addr, optarg, tmpsize);
            }
            if (option == 'S') // _ROLE_SERVER
            {
                if (tmpsize <= 0 || tmpsize > _MAX_STR_LEN)
                {
                    pverbose("parser: invalid <addr_local> parameter ! \n");
                    return rtErr;
                }
                Options.run_mode = _ROLE_SERVER;
                strncpy(Options.local_addr, optarg, tmpsize);
            }
            break;

        case 'i': // Forces to use a specific hardware interface
            // pverbose(" '%s' unsupported options ! \n", option);
            tmpsize = strlen(optarg);
            strncpy(Options.local_if_str, optarg, tmpsize);
            return rtErr;
            break;

        case 'n': // Repetitions
            Options.tst_repeats = atoi(optarg);
            if (Options.tst_repeats < 1)
            {
                pverbose("parser: repetitions must be >= 1\n");
                return rtErr;
            }
            break;
        case 'p': // Service number (ipv4 port)
            tmpint = atoi(optarg);
            if (tmpint < 0)
            {
                pverbose("parser: port number must be >0\n");
                return rtErr;
            }
            else
                Options.service_num = tmpint;
            break;

        case 'b': // Traffic block size
            tmplong = atol(optarg);
            if (tmplong < MIN_BLOCK_SIZE)
            {
                pverbose("parser: blocksize must be >0\n");
                return rtErr;
            }
            else
                Options.tst_block_size = tmplong;
            break;

        case 'c': // Traffic pkts number
            tmplong = atol(optarg);
            if (tmplong < MIN_PKT_NUMBER)
            {
                pverbose("parser: packet number must be >0\n");
                return rtErr;
            }
            else
                Options.tst_pkts_num = (double)tmplong;
            break;

        case 'm': // Protocol MTU size in bytes
            tmpint = atoi(optarg);
            if (tmpint < 1)
            {
                pverbose("parser: MTU size must be > 0\n");
                return rtErr;
            }
            else
                // Options.pkt_payload_size = tmplong;
                Options.pkt_mtu_size = tmpint;
            break;

        case 'o': // Packet's PAYLOAD size in bytes
            tmpint = atoi(optarg);
            if (tmpint < 1)
            {
                pverbose("parser: packet payload size must be > 0\n");
                return rtErr;
            }
            else
                Options.pkt_payload_size = tmpint;

            break;

        case 'f':
            Options.csv_enabled = true;
            tmpsize = strlen(optarg);
            strncpy(Options.csv_path, optarg, sizeof(Options.csv_path) - 1);
            break;

        case 't':
            tmplong = atol(optarg);
            if (tmplong > 1 && tmplong < 8 * 3600)
                Options.tst_time_slot = (double)tmplong;
            else
            {
                pverbose("parser: testing time period not valid '-t %s' ! \n", optarg);
                return rtErr;
            }
            break;

        case 'y': // Enables output in csv format: '1' no header, '0' with header (default)
            Options.csv_enabled = true;
            Options.csv_no_header = false;
            if (strlen(optarg) > 1)
            {
                pverbose("parser: formatting options '-y %s' unknown ! \n", optarg);
                return rtErr;
            }
            else
                Options.csv_no_header = (atoi(optarg) == 1);
            break;

        default:
            pverbose("parser: unknow options '%s' !\n", option);
            print_help();
            return rtErr;
        }
    }
    return rtOk;
}

// -------------------------------------------------------------------------------------------------------- !
void print_help()
{
    printf("\nGeneral options\n");
    printf("  -v                                Show dsperf details and exit\n");
    printf("  -h,--help                         Show usage informations\n");
    printf("  -g                                Disable all system's messages\n");
    printf("  -w <bandwidth>                    Sets reference to a nominal speed [Mbps] (bandwidth)\n");
    printf("  -p <service>                      Sets service number (sub-protocol) \n");
    printf("  -l                                Lists local interfaces (require protocol specification)\n");
    printf("  -i <interface>                    Forces to use a specific interface e/o driver (use -l to list)\n");
    //
    printf("\nRun mode");
    printf("  -S <local_addr>:<service>         Starts loopback server mode (if omitted uses default service/port)\n");
    printf("  -s <remote_addr>:<service>        Starts as client to test on loopback \n");
    printf("  -n <repetitions>                  Repeats test\n");
    printf("  -t <period>                       Continuosly testing for time period in [seconds] (max 8h)\n");
    printf("  -b <bytes>                        Generates traffic trasferring data-block sized in bytes\n");
    printf("  -c <n_pkts>                       Generates traffic trasferring number of packets with payload size fixed (use '-o')\n");
    printf("  -V                                Enable messages for the looping server events\n");
    //
    printf("\nTraffic generator\n");
    printf("  -m <mtu_size>                     Protocol MTU size in bytes\n");
    printf("  -o <payload_size>                 Packet payload size in bytes\n");
    printf("  -x <n_threads>                    Enable to use concurrent connections \n");

    //
    printf("\nProtocol and Model\n");
    printf("  --ipv4                            Uses IPv4/TCP underlay network (RFCs 790-791)\n");
    printf("  --daas                            Uses DaaS overlay \n");
    printf("\n");
    printf("  --features                        Applies test model to evaluate features compliance\n");
    printf("  --capacity                        Applies test model to mesure throughput (default)\n");
    printf("  --security                        Applies test model to check security threats\n");
    printf("  --continuity                      Applies test model to evaluate availability and its continuity\n");
    //
    printf("\nReporting\n");
    printf("  -y <0/1>                          Outputs results in csv format: '1' no header, '0' with header (default)\n");
    printf("  -f <filename>                     Outputs testing results in csv format to file (client only)\n");
    printf("\n");
    //
    printf("\tNote: commands in upper-case will be available only in server mode.\n");
    printf("\n");
}

void print_license()
{
    printf("License MPL v.2.0 <https://mozilla.org/MPL/2.0/>\n");
    printf("This Software is provided under this License on an 'as is' basis, without warranty of any kind.\n");
    printf("This disclaimer of warranty constitutes an essential part of this License.\n");
}

void print_credits()
{
    printf("\ndsperf  %d.%d.%d\n", PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, PROJECT_VERSION_PATCH);
    printf("2024-2025 (@) Sebyone Srl\n");
    print_license();
    printf("\n");
}

void print_usage() // const char *prog_name
{
    const char *prog_name = "dsperf";
    printf("Usage:\n");
    printf("\n");
    printf("  server Loopback: %s -S <local_ip:port> [ --ipv4 | --daas ]  -V\n", prog_name);
    printf("\n");
    printf("  client Tester:  %s -s <server_ip:port> [-b <blocksize> | -c <n_pkts> ] -p <pkt-payload-size> -n <repetitions>  [ --ipv4 | --daas ]   \n", prog_name);
    printf("\n");
}

// -------------------------------------------------------------------------------------------------------- !
void clearOptions()
{
    memset(&Options, 0, sizeof(options_t));
    // Model
    Options.model_protocol = _PROTO_NONE; // Test protocol (IPV4, DAAS, etc.etc.)
    Options.model_class = __unsetted;     // Test class (Features, Capacity, etc.etc.)
    // Run mode
    Options.run_mode = _ROLE_NONE; // client or loopback server
    Options.tst_repeats = 1;       // repetitions counter
    Options.tst_time_slot = 0;     // Test for a time period
    Options.tst_block_size = 0;    // Traffic by block-size
    Options.tst_pkts_num = 0;      // Testing for a lot of packet
    Options.multilink = 0;         // enable to use concurrent connections
    // Class: Caqpacity
    Options.pkt_payload_size = 0; // Packet payload size
    Options.bandwidth = 0;        // set reference to nominal speed [Mbps]

    // Addressing
    Options.local_if_str[0] = '\0'; // Local interface to bind
    Options.local_addr[0] = '\0';   // Local interface and address
    // Options.remote_addr[0] = '\0';    // Remote address to connect (used in client mode only)
    Options.service_str[0] = '\0'; // Local interface and address
    Options.service_num = 0;       // Service identifier (port for ipv4 stack)

    // Formatting
    Options.csv_enabled = false;   // print out in csv format
    Options.csv_no_header = false; // print out csv columns header
    Options.csv_path[0] = '\0';    // output file (default 'stdout')
    Options.pktverbose = false;    // Packet verbose mode
    Options.listinterfaces = false;
}
