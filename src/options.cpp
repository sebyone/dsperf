
#include "options.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>

#include "version.h"

options_t Settings;

// -------------------------------------------------------------------------------------------------------- !
void clearSettings()
{
    memset(&Settings, 0, sizeof(options_t));

    //  Settings.version = false; // Common options
    Settings.host_role = -1; // Client / Server
    Settings.model = -1;     // Test model
    Settings.repetitions = 1;

    Settings.csv_enabled = false; // Common output options
    Settings.csv_no_header = false;
    Settings.csv_path[0] = '\0';
    Settings.model_path[0] = '\0';

    Settings.block_size = 0; // Specific test model Traffic
    Settings.mss_specified = false;
    Settings.pkt_payload = IPV4_MAX_MSS;
    Settings.pkts_num = 1;

    Settings.ipv4_addr[0] = '\0';
    Settings.ipv4_port = IPV4_DEF_SPORT; // Specific IP options

    Settings.remote_din = -1;
}

// -------------------------------------------------------------------------------------------------------- !
void print_usage(const char *prog_name)
{
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
    printf("\nOptions:\n");
    printf("  -v                     Show dsperf details and exit\n");
    printf("  -S <local_ip:port>     Start loopback server mode (default 127.0.0.1:%d)\n", IPV4_DEF_SPORT);
    printf("  -V                     Server in verbose mode\n");
    printf("  -s <remote_ip:port>    Start as client to test on loopback \n");
    printf("  -i <local_ip:port>     Forces the client to use a specific interface ()\n");

    printf("\nTraffic generator\n");
    printf("  -b <bytes>             Data-block to transfer in bytes [%d-%d]  \n", MIN_BLOCK_SIZE, MAX_BLOCK_SIZE);
    printf("  -p <bytes>             Packet's payload size in bytes [%d-%d] (tries to negotiate 'mss' in ipv4tcp)\n", IPV4_MIN_MSS, IPV4_MAX_MSS);
    printf("  -c <count>             Number of packets to send (only with '-p')\n");
    printf("  -n <repetitions>       Number of Test repetitions (only with ipv4tcp)\n");

    printf("\nTesting models\n");
    printf("  --ipv4                 Test IPv4/TCP underlay network (RFCs 790-791)\n");
    printf("  --daas                 Use DaaS overlay mode\n");

    printf("\nFormatting\n");
    printf("  -f <csv_file>          CSV file for results output (optional, client only)\n");

    printf("\n");
    printf("\tNote: commands in upper-case will be considered only in server mode.\n");

    printf("\n");
}

// -------------------------------------------------------------------------------------------------------- !
void print_version()
{
    printf("dsperf  %d.%d.%d\n", PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, PROJECT_VERSION_PATCH);
    printf("2024-2025 (@) Sebyone Srl\n\n");
    printf("License MPL v.2.0 <https://mozilla.org/MPL/2.0/>\n");
    printf("This Software is provided under this License on an 'as is' basis, without warranty of any kind.\n");
    printf("This disclaimer of warranty constitutes an essential part of this License.\n");
    printf("\n");
}

// -------------------------------------------------------------------------------------------------------- !
ret_t parse_args(int argc, char *argv[])
{
    int option_index = 0;
    int c;

    if (argc < 2)
    {
        print_version();
        print_usage(argv[0]);
        return rtExit;
    }

    clearSettings();

    static struct option long_options[] = {// "--" (https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html)
                                           {"ipv4", no_argument, 0, 1},
                                           {"daas", required_argument, 0, 2},
                                           {"v4udp", required_argument, 0, 3},
                                           {"help", no_argument, 0, 4},
                                           {0, 0, 0, 0}}; //
                                                          // {"blocksiz", required_argument, 0, 3}, // *name, has_arg, *flag, val

    while ((c = getopt_long(argc, argv, "S:s:b:n:c:m:f:t:y:v:h", long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 'S': // host_role
        case 's':
            if (Settings.host_role != -1) // Already setted
            {
                pverbose("error: can't specify both -S and -s !\n");
                return rtErr;
                // exit(EXIT_FAILURE);
            }

            Settings.host_role = (c == 's') ? _ROLE_CLIENT : _ROLE_SERVER;

            if (Settings.host_role == _ROLE_SERVER)
            {

                if (Settings.model == _MODEL_IPV4)
                {
                    // Local interface ????????????????????????????????????????
                    // Settings.ipv4_addr

                    Settings.ipv4_port = atoi(optarg); // Check ipv4_port
                    if (Settings.ipv4_port <= 0)
                    {
                        pverbose("Error: invalid IPv4 Service Port number !\n");
                        return rtErr;
                    }
                }
                else if (Settings.model == _MODEL_DAAS)
                {
                    // Check daas local settings
                }
            }
            else // _ROLE_CLIENT
            {
                if (Settings.model == _MODEL_IPV4)
                {
                    // Check host:port

                    size_t salen = strlen(optarg);
                    char *psaddr = (char *)malloc(salen);

                    strncpy(psaddr, optarg, salen);
                    char *psport = strchr(psaddr, ':');

                    if (psport == NULL)
                    {
                        pverbose("[CLIENT] Invalid host format. Use IP:PORT\n");
                        return rtErr;
                    }
                    else
                    {
                        size_t slen = (size_t)(psaddr - psport);
                        if (slen < _OPT_STR_LEN)
                        {
                            strncpy(Settings.ipv4_addr, psaddr, slen);
                            Settings.ipv4_addr[slen] = '\0';
                            psport++; // Skip ':'
                            Settings.ipv4_port = atoi(psport);
                        }
                        else
                        {
                        }
                    }

                    if (strlen(Settings.ipv4_addr) < 6)
                    {
                        pverbose("error: invalid server address\n");
                        return rtErr;
                    }
                }

                else if (Settings.model == _MODEL_DAAS)
                {
                    Settings.remote_din = atoi(optarg);
                    if (Settings.remote_din <= 0)
                    {
                        pverbose("error: invalid remote DIN  \n");
                        return rtErr;
                    }
                }
            }

            break;

        case 'n':
            Settings.repetitions = atoi(optarg);
            if (Settings.repetitions < 1)
            {
                pverbose("Error: repetitions must be >= 1\n");
                // exit(EXIT_FAILURE);
                return rtErr;
            }
            break;

        case 'c':
            Settings.pkts_num = atoi(optarg);
            if (Settings.pkts_num < 1)
            {
                pverbose("Error: packet number must be >= 1\n");
                return rtErr;
            }
            break;

        case 'm':
            Settings.mss_specified = true;
            Settings.pkt_payload = atoi(optarg);
            if (Settings.pkt_payload < 1)
            {
                pverbose("Error: 'mss' must be >= 1\n");
                return rtErr;
            }
            break;

        case 'f':

            Settings.csv_enabled = true;
            strncpy(Settings.csv_path, optarg, sizeof(Settings.csv_path) - 1);
            break;

        case 't':
        {
            Settings.time_defined = true;
            int val = atoi(optarg);
            Settings.time = val;
            break;
        }

        case 'y':
        {
            Settings.csv_format = true;
            Settings.csv_no_header = false;

            int val = atoi(optarg);
            if (val != 0 && val != 1)
            {
                pverbose("Errore: il valore per --csv-no-header (-y) deve essere 0 o 1.\n");
                return rtErr;
            }

            Settings.csv_no_header = (val == 1);
        }
        break;

        case 'v': //  Show version information and exit

            print_version();
            return rtExit;
            break;

            /*
                    case 1: // --underlay
                        if (Settings.model != -1)
                        {
                            pverbose(stderr, "Error: Cannot specify both --underlay and --daas\n");
                            exit(EXIT_FAILURE);
                        }
                        Settings.model = 0;
                        break;

                    case 2: // --daas
                        if (Settings.model != -1)
                        {
                            pverbose(stderr, "Error: Cannot specify both --underlay and --daas\n");
                            exit(EXIT_FAILURE);
                        }
                        Settings.model = 1;
                        strncpy(Settings.model_path, optarg, sizeof(Settings.model_path) - 1);
                        break;
            */

        case 'b': // --blocksize
            Settings.block_size = (unsigned)atol(optarg);
            if (Settings.block_size < 1)
            {
                pverbose("Error: blocksize must be >= 1\n");
                return rtErr;
            }
            break;

        case 3: // --ipv4

            // Set model

            break;

        case 4:   // --help
        case 'h': // --help
            print_help();
            return rtExit;
            break;

        default:
            pverbose("Unknown option\n");
            return rtErr;
        }
    }
    return rtOk;
}

// -------------------------------------------------------------------------------------------------------- !
ret_t validate_options()
{
    // Controlli base
    /*
    if (Settings.host_role == -1)
    {
        pverbose(stderr, "Error: must specify either -S (server) or -s (client)\n");
        return rtExit;
    }
    if (Settings.model == -1)
    {
        pverbose(stderr, "Error: must specify either --underlay or --daas\n");
        return rtExit;
    }

    if (Settings.host_role == 0) /// -1 = unset, 0 = server, 1 = client 2 = router
    {
        if (Settings.model == 0) // Se model non è definito, proviamo a dedurlo dall'argomento di -S
        {
            if (Settings.port == 0) // -S deve essere porta
            {
                if (Settings.ipv4_addr[0] != '\0') // Se non settata ancora, proviamo a convertire da ipv4_addr (tmp)
                {
                    Settings.port = atoi(Settings.ipv4_addr);
                    if (Settings.port <= 0)
                    {
                        pverbose(stderr, "Error: Invalid port number for server\n");
                        return rtExit;
                    }
                }
                else
                {
                    pverbose(stderr, "Error: Server port not specified\n");
                    return rtExit;
                }
            }
        }
        else if (Settings.model == 1)
        {

            if (Settings.remote_din < 0) // -S deve essere remote_din
            {
                if (Settings.ipv4_addr[0] != '\0')
                {
                    Settings.remote_din = atoi(Settings.ipv4_addr);
                    if (Settings.remote_din < 0)
                    {
                        pverbose(stderr, "Error: Invalid DIN for server\n");
                        return rtExit;
                    }
                }
                else
                {
                    pverbose(stderr, "Error: Server DIN not specified\n");
                    return rtExit;
                }
            }
        }

        if (Settings.block_size != 0) // Verifica che non siano presenti opzioni non ammesse
        {
            pverbose(stderr, "Error: Server must not specify --blocksize\n");
            return rtExit;
        }
        if (Settings.repetitions != 1)
        {
            pverbose(stderr, "Error: Server must not specify -n (repetitions)\n");
            return rtExit;
        }
        if (Settings.csv_enabled)
        {
            pverbose(stderr, "Error: Server must not specify -f (csv output)\n");
            return rtExit;
        }
        if (Settings.csv_no_header)
        {
            pverbose(stderr, "Error: Server must not specify -y (csv header control)\n");
            return rtExit;
        }
        if (Settings.mss_specified)
        {
            pverbose(stderr, "Error: Server must not specify -m (mtu)\n");
            return rtExit;
        }
    }
*/

    if (Settings.host_role == _ROLE_CLIENT) // Per client: deve avere tutti i parametri corretti
    {
        if (Settings.model == _MODEL_IPV4) // Verifica model e argomenti collegati
        {
            if (Settings.ipv4_addr[0] == '\0') // underlay: ipv4_addr deve essere IP:PORT
            {
                pverbose("Error: Client must specify IP:PORT for underlay\n");
                return rtExit;
            }
        }
        else if (Settings.model == _MODEL_DAAS)
        {
            if (Settings.remote_din < 0) // daas: remote_din >= 0
            {
                if (Settings.ipv4_addr[0] != '\0')
                {
                    Settings.remote_din = atoi(Settings.ipv4_addr);
                    if (Settings.remote_din < 0)
                    {
                        pverbose("Error: Invalid DIN for client\n");
                        return rtExit;
                    }
                }
                else
                {
                    pverbose("Error: Client DIN not specified\n");
                    return rtExit;
                }
            }
        }

        if (Settings.block_size < 1)
        {
            pverbose("Error: Client must specify --blocksize >= 1\n");
            return rtExit;
        }
        if (Settings.repetitions < 1)
        {
            pverbose("Error: repetitions must be >= 1\n");
            return rtExit;
        }
        if (Settings.mss_specified && Settings.pkt_payload < 1)
        {
            pverbose("Error: MTU must be >= 1\n");
            return rtExit;
        }
        if (Settings.csv_enabled && Settings.csv_path[0] == '\0') // csv_path se csv_enabled deve essere valorizzato
        {
            pverbose("Error: CSV output enabled but no file specified\n");
            return rtExit;
        }
    }

    return rtOk;
}
