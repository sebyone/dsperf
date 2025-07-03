
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

stypedef struct // Option example: {1,"S", "<local_addr>:<service>","Starts loopback server mode (without...."}
{
int id;
bool lopt;
const char ocode;
const char* locode;
const char* args;   // empty set no args
const char* note;   // brief description
double lvalue;
double dvalue;
bool bvalue;
char *svalue[_MAX_STR_LEN];
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

-------------

typedef struct
{
    nproto_t model_protocol;        // 0 = unset, _PROTO_DAAS,  _PROTO_IPV4, ...
    tclass_t model_class;           // 0 = unset, Features, Capacity, Continuity, etc.etc.
                                    //
    hrole_t run_mode;               // 0 = unset, _ROLE_CLIENT, _ROLE_SERVER, ...
    int tst_repeats;                // repeats test, default is 1
    long tst_time_slot;             // perform testing for a fixed time period, in seconds
    long tst_block_size;            // generate traffic by block size
    long tst_pkts_num;              // generate traffic by number of pkts (requires pkt_payload)
                                    //
                                    //  _____Tester parmaters
    int pkt_payload;                // pkt's payload size (needs to be supported in testing routine)
                                    // ______Capacity
    long bandwidth;                 // set reference to nominal speed [Mbps]
    int multilink;                  // enable to use concurrent connections
                                    //
                                    // ______Peers addressing
    char local_addr[_MAX_STR_LEN];  // Protocol Local-IF/Addr
    char remote_addr[_MAX_STR_LEN]; // Protocol Remote Addr (URI Loopback Service)
    int service_num;                // Loopback Service Port
                                    //
                                    // ______Output formatting
    bool csv_enabled;               // output will be formatted
    bool csv_format;                //
    bool csv_no_header;             // csv columns header is/not printed out
    char csv_path[_MAX_STR_LEN];    // output will be saved
                                    // char model_path[_OPT_STR_LEN];  // path to configutation file (libdaas.ini)
} options_t;

*/

#define _SHORT_OPTIONS "S:s:b:n:i:c:m:f:t:y:v:V:h:w:g:p:o:x:l"

const static option _LONG_OPTIONS[] = { // (https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html)
    {"help", no_argument, NULL, 0},
    {"capacity", no_argument, NULL, 1},
    {"features", no_argument, 0, 2},
    {"security", no_argument, 0, 3},
    {"continuity", no_argument, 0, 4},
    {"ipv4", no_argument, 0, 5},
    {"daas", no_argument, 0, 6}, // required_argument,
    {0, 0, 0, 0}};               //

// -------------------------------------------------------------------------------------------------------- !
// Parsing
// -------------------------------------------------------------------------------------------------------- !
rt_t parse_args(int argc, char *argv[]) // Syntax validations
{

    int option;
    int loption_index = 0;
    char *psep = NULL;
    size_t tmpsize;
    long tmplong;

    if (argc < 2)
    {
        // print_credits();
        print_usage();
        return rtExit;
    }

    clearOptions(); // Options

    while ((option = getopt_long(argc, argv, _SHORT_OPTIONS, _LONG_OPTIONS, &loption_index)) != -1)
    {

        //DEBUG
        //printf("option %c , %d", option, loption_index);
        //printf(" with arg %s\n", optarg);

        switch (option)
        {
        case 0: // --help
            option = 'h';
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

            // short options _____________________
        case 'v': //  how dsperf details and exit
            print_credits();
            return rtExit;
            break;

        case 'h':
            print_usage();
            return rtExit;
            break;

        case 'k': // Disable all system's messages
        case 'l': // Show local interfaces for protocol
        case 'g': // Disable all system's messages
        case 'V': // Enables messages for the looping server events
            pverbose("parser:  unsupported options '%s' ! \n", option);
            return rtErr;
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
                pverbose("parser: repetitions must be >= 1\n");
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
            pverbose(" '%s' unsupported options ! \n", option);
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
            tmplong = atoi(optarg);
            if (tmplong < 0)
            {
                pverbose("parser: port number must be >0\n");
                return rtErr;
            }
            else
                Options.service_num = (double)tmplong;
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

        case 'o': // Packet's payload size in bytes
            tmplong = atol(optarg);
            if (tmplong < 1)
            {
                pverbose("parser: packet's payload size must be > 0\n");
                return rtErr;
            }
            else
                Options.pkt_payload = tmplong;
            break;

        case 'f':
            Options.csv_enabled = true;
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
            Options.csv_format = true;
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
    printf("  -l                                Lists local interface (require protocol specifications)\n");
    printf("  -g                                Disable all system's messages\n");
    printf("  -w <bandwidth>                    Sets reference to a nominal speed [Mbps] (bandwidth)\n");
    printf("  -x                                Enable to use concurrent connections \n");
    printf("  -p <service>                      Sets service number (port inipv4) \n");

    printf("\nRun mode");
    printf("  -S <local_addr>:<service>         Starts loopback server mode (if omitted uses default service/port)\n");
    printf("  -s <remote_addr>:<service>        Starts as client to test on loopback \n");
    printf("  -i <local_interface/driver>       Forces to use a specific hardware interface (use -l to list availables)\n");
    printf("  -n <repetitions>                  Repeats test\n");
    printf("  -t <period>                       Continuosly testing for time period in [seconds] (max 8h)\n");
    printf("  -V                                Enable messages for the looping server events\n");

    printf("\nTraffic generator\n");
    printf("  -b <bytes>                        Generates traffic trasferring data-block sized in bytes\n");
    printf("  -c <count>                        Generates traffic trasferring number of packets with payload size fixed (use '-p')\n");
    printf("  -o <bytes>                        Packet's payload size in bytes (tries to negotiate payload)\n");

    printf("\nProtocol and Model\n");
    printf("  --ipv4                            Uses IPv4/TCP underlay network (RFCs 790-791)\n");
    printf("  --daas                            Uses DaaS overlay \n");
    printf("\n");
    printf("  --features                        Applies test model to evaluate features compliance\n");
    printf("  --capacity                        Applies test model to mesure throughput (default)\n");
    printf("  --security                        Applies test model to check security threats\n");
    printf("  --continuity                      Applies test model to evaluate availability and its continuity\n");

    printf("\nReporting\n");
    printf("  -y <0/1>                          Outputs results in csv format: '1' no header, '0' with header (default)\n");
    printf("  -f <filename>                     Outputs testing results in csv format to file (client only)\n");
    printf("\n");

    printf("\tNote: commands in upper-case will be available only in server mode.\n");

    printf("\n");
}

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
    printf("  server Loopback: %s -S <local_ip:port> --ipv4 | --daas -V\n", prog_name);
    printf("\n");
    printf("  client Tester:  %s -s <server_ip:port> [-b <blocksize> | -p <packet-size> -c <count> ] -n <repetitions>  [ --ipv4 | --daas ]  \n", prog_name);
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
    Options.pkt_payload = 0; // Packet payload size
    Options.bandwidth = 0;   // set reference to nominal speed [Mbps]

    // Formatting
    Options.csv_enabled = false;   // print out in csv format
    Options.csv_no_header = false; // print out csv columns header
    Options.csv_path[0] = '\0';    // output file (default 'stdout')

    // Addressing
    Options.local_addr[0] = '\0';  // Local interface and address
    Options.remote_addr[0] = '\0'; // Remote address to connect (used in client mode only)
    Options.service_str[0] = '\0'; // Local interface and address
    Options.service_num = 0;       // Service identifier (port for ipv4 stack)
}
