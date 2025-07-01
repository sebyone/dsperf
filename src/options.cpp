
#include "options.h"
#include <ctype.h>
#include <stdlib.h>
// #include <string.h>
#include <stdio.h>
#include <getopt.h>

#include "version.h"
//

options_t Options; // global.h

#define DSPERF_OPTIONS "S:s:b:n:i:c:m:f:t:y:v:V:h:w:g"

static struct option long_options[] = { // "--" (https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html)
    {"ipv4", no_argument, 0, 1},
    {"daas", required_argument, 0, 2},
    {"capacity", required_argument, 0, 3},
    {"help", no_argument, 0, 4},
    {0, 0, 0, 0}}; //

// -------------------------------------------------------------------------------------------------------- !
void clearGlobalOptions()
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
    printf("  server Loopback: %s -S <local_ip:port> --ipv4 |--daas -V\n", prog_name);
    printf("\n");
    printf("  client Tester:  %s -s <server_ip:port> [-b <blocksize> | -p <packet-size> -c <count> ] -n <repetitions>  [ --ipv4 | --daas ]  \n", prog_name);
    printf("\n");
}

// -------------------------------------------------------------------------------------------------------- !
void print_help()
{
    printf("\nGeneral options\n");
    printf("  -v                                Show dsperf details and exit\n");
    printf("  -h, --help                        Show usage informations\n");
    printf("  -l                                Lists local interface (require protocol specifications)\n");
    printf("  -g                                Disable all system's messages\n");
    printf("  -w                                Sets reference to nominal speed [Mbps] (bandwidth)\n");
    printf("  -x                                Enable to use concurrent connections \n");

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
    int option;
    size_t tmpsize;
    long tmplong;

    if (argc < 2)
    {
        print_credits();
        print_usage();
        return rtExit;
    }

    clearGlobalOptions(); // Options

    while ((option = getopt_long(argc, argv, DSPERF_OPTIONS, long_options, &option_index)) != -1)
    {

        switch (option)
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
            tmpsize = strlen(optarg); // <addr_remote>
            if (option == 's')        // _ROLE_CLIENT
            {
                Options.run_mode = _ROLE_CLIENT;
                if (tmpsize > 0 && tmpsize >= _MAX_STR_LEN)
                {
                    pverbose("parser: invalid <addr_remote> parameter ! \n");
                    return rtErr;
                }
                strncpy(Options.remote_addr, optarg, tmpsize);
            }
            if (option == 'S') // _ROLE_SERVER
            {
                Options.run_mode = _ROLE_SERVER;

                if (tmpsize > 0 && tmpsize >= _MAX_STR_LEN)
                {
                    pverbose("parser: invalid <addr_local> parameter ! \n");
                    return rtErr;
                }
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

        case 'p': // Packet's payload size in bytes
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
            if (tmplong > 1 && tmplong << 8 * 3600)
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
            pverbose("parser: unknow options '%c' !\n", option);
            print_help();
            return rtErr;
        }
    }
    return rtOk;
}
