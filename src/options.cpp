
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
    memset(&Settings, 0, sizeof(&Settings));

    Settings.version = false; // Common options
    Settings.host_role = -1;  // Client / Server
    Settings.model = -1;      // Test model
    Settings.repetitions = 1;

    Settings.csv_enabled = false; // Common output options
    Settings.csv_no_header = false;
    Settings.csv_path[0] = '\0';
    Settings.model_path[0] = '\0';

    Settings.block_size = 0; // Specific test model Traffic
    Settings.mss_specified = false;
    Settings.pkt_payload = 1500;
    Settings.pkts_num = 1;

    Settings.port = 0; // Specific IP options
    Settings.remote_din = -1;
    Settings.remote_addr[0] = '\0';
}

// -------------------------------------------------------------------------------------------------------- !
void print_usage(const char *prog_name)
{
    printf("Usage:\n");
    printf("Note: the <bytes> part of --blocksize and --packet-size of the following commands are only\n");
    printf("considered in client mode.\n");
    printf("\n");
    printf("  Sender (Client): %s -s <host:port> [--blocksize <bytes> -n <repetitions> | --packet-size <bytes> -c <count>] --underlay|--daas [-f <csv_file>]\n", prog_name);
    printf("  Receiver (Server): %s -S [port] --underlay|--daas --blocksize|--packet-size\n", prog_name);
}

// -------------------------------------------------------------------------------------------------------- !
void print_help()
{
    printf("\nOptions:\n");
    printf("  -S [port]              Run in server mode (default port: 8080)\n");
    printf("  -s <host:port>         Server address and port to connect to (client mode)\n");
    printf("  --underlay             Use underlay network mode (IPv4 sockets)\n");
    printf("  --daas                 Use DaaS overlay mode\n");
    printf("  -f <csv_file>          CSV file for results output (optional, client only)\n");
    printf("  -v                     Show version information and exit\n");
    printf("\n");
    printf("Note: the <bytes> part of the following commands are considered only in client mode\n");
    printf("  --blocksize <bytes>    Total data to transfer in bytes [%d-%d] (mutually exclusive with --packet-size)\n", MIN_BLOCK_SIZE, MAX_BLOCK_SIZE);
    printf("  -n <repetitions>       Number of repetitions for data block transmission (only with --blocksize)\n");
    printf("  --packet-size <bytes>  Individual packet size in bytes [%d-%d] (mutually exclusive with --blocksize)\n", MIN_PACKET_SIZE, MAX_PACKET_SIZE);
    printf("  -c <count>             Number of packets to send (only with --packet-size)\n");
    printf("\n");
}

// -------------------------------------------------------------------------------------------------------- !
void print_version()
{
    printf("\n");
    printf("dsperf v %d.%d.%d \n", PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, PROJECT_VERSION_PATCH);
    printf("2024, 2025 (@) Sebyone Srl\n");
    printf("\n\n");
}

// -------------------------------------------------------------------------------------------------------- !
ret_t parse_args(int argc, char *argv[])
{
    int option_index = 0;
    int c;

    clearSettings();

    static struct option long_options[] = {// "--" (https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html)
                                           {"underlay", no_argument, 0, 1},
                                           {"daas", required_argument, 0, 2},
                                           {"blocksize", required_argument, 0, 3}, // *name, has_arg, *flag, val
                                           {"help", required_argument, 0, 4},
                                           {0, 0, 0, 0}}; // Serve ?

    while ((c = getopt_long(argc, argv, "S:s:n:c:m:f:t:y:v", long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 'S': // host_role
        case 's':
            if (Settings.host_role != -1) // Already setted
            {
                fprintf(stderr, "error: can't specify both -S and -s !\n");
                exit(EXIT_FAILURE);
            }

            Settings.host_role = (c == 's') ? _ROLE_CLIENT : _ROLE_SERVER;
            if (Settings.host_role == _ROLE_SERVER)
            {
                if (Settings.model == _MODEL_IPV4)
                {
                    // Local interface

                    // Check port
                    Settings.port = atoi(optarg);
                    if (Settings.port <= 0)
                    {
                        fprintf(stderr, "Error: Invalid port number for server\n");
                        exit(EXIT_FAILURE);
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
                    strncpy(Settings.remote_addr, optarg, sizeof(Settings.remote_addr) - 1);
                    if (strlen(Settings.remote_addr) < 6)
                    {
                        fprintf(stderr, "error: invalid server address\n");
                        exit(EXIT_FAILURE);
                    }
                }
                else if (Settings.model == _MODEL_DAAS)
                {
                    Settings.remote_din = atoi(optarg);
                    if (Settings.remote_din <= 0)
                    {
                        fprintf(stderr, "error: invalid remote DIN  \n");
                        exit(EXIT_FAILURE);
                    }
                }
            }

            break;

        case 'n':
            Settings.repetitions = atoi(optarg);
            if (Settings.repetitions < 1)
            {
                fprintf(stderr, "Error: repetitions must be >= 1\n");
                exit(EXIT_FAILURE);
            }
            break;

        case 'c':
            Settings.pkts_num = atoi(optarg);
            if (Settings.pkts_num < 1)
            {
                fprintf(stderr, "Error: packet number must be >= 1\n");
                exit(EXIT_FAILURE);
            }
            break;

        case 'm':
            Settings.mss_specified = true;
            Settings.pkt_payload = atoi(optarg);
            if (Settings.pkt_payload < 1)
            {
                fprintf(stderr, "Error: 'mss' must be >= 1\n");
                exit(EXIT_FAILURE);
            }
            break;

        case 'f':
        {
            Settings.csv_enabled = true;
            strncpy(Settings.csv_path, optarg, sizeof(Settings.csv_path) - 1);
            break;
        }

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
                fprintf(stderr, "Errore: il valore per --csv-no-header (-y) deve essere 0 o 1.\n");
                exit(EXIT_FAILURE);
            }

            Settings.csv_no_header = (val == 1);
        }
        break;

        case 'v':
            Settings.version = true; // ????
            print_version();
            break;

            /*
                    case 1: // --underlay
                        if (Settings.model != -1)
                        {
                            fprintf(stderr, "Error: Cannot specify both --underlay and --daas\n");
                            exit(EXIT_FAILURE);
                        }
                        Settings.model = 0;
                        break;

                    case 2: // --daas
                        if (Settings.model != -1)
                        {
                            fprintf(stderr, "Error: Cannot specify both --underlay and --daas\n");
                            exit(EXIT_FAILURE);
                        }
                        Settings.model = 1;
                        strncpy(Settings.model_path, optarg, sizeof(Settings.model_path) - 1);
                        break;
            */
        case 3: // --blocksize
            Settings.block_size = atoi(optarg);
            if (Settings.block_size < 1)
            {
                fprintf(stderr, "Error: blocksize must be >= 1\n");
                exit(EXIT_FAILURE);
            }
            break;

        case 4: // --help
            print_help();
            break;

        default:
            fprintf(stderr, "Unknown option\n");
            exit(EXIT_FAILURE);
        }
    }
    return rtOk;
}

// -------------------------------------------------------------------------------------------------------- !
ret_t validate_args(const char *prog_name)
{
    // Controlli base
    /*
    if (Settings.host_role == -1)
    {
        fprintf(stderr, "Error: must specify either -S (server) or -s (client)\n");
        return rtExit;
    }
    if (Settings.model == -1)
    {
        fprintf(stderr, "Error: must specify either --underlay or --daas\n");
        return rtExit;
    }

    if (Settings.host_role == 0) /// -1 = unset, 0 = server, 1 = client 2 = router
    {
        if (Settings.model == 0) // Se model non è definito, proviamo a dedurlo dall'argomento di -S
        {
            if (Settings.port == 0) // -S deve essere porta
            {
                if (Settings.remote_addr[0] != '\0') // Se non settata ancora, proviamo a convertire da remote_addr (tmp)
                {
                    Settings.port = atoi(Settings.remote_addr);
                    if (Settings.port <= 0)
                    {
                        fprintf(stderr, "Error: Invalid port number for server\n");
                        return rtExit;
                    }
                }
                else
                {
                    fprintf(stderr, "Error: Server port not specified\n");
                    return rtExit;
                }
            }
        }
        else if (Settings.model == 1)
        {

            if (Settings.remote_din < 0) // -S deve essere remote_din
            {
                if (Settings.remote_addr[0] != '\0')
                {
                    Settings.remote_din = atoi(Settings.remote_addr);
                    if (Settings.remote_din < 0)
                    {
                        fprintf(stderr, "Error: Invalid DIN for server\n");
                        return rtExit;
                    }
                }
                else
                {
                    fprintf(stderr, "Error: Server DIN not specified\n");
                    return rtExit;
                }
            }
        }

        if (Settings.block_size != 0) // Verifica che non siano presenti opzioni non ammesse
        {
            fprintf(stderr, "Error: Server must not specify --blocksize\n");
            return rtExit;
        }
        if (Settings.repetitions != 1)
        {
            fprintf(stderr, "Error: Server must not specify -n (repetitions)\n");
            return rtExit;
        }
        if (Settings.csv_enabled)
        {
            fprintf(stderr, "Error: Server must not specify -f (csv output)\n");
            return rtExit;
        }
        if (Settings.csv_no_header)
        {
            fprintf(stderr, "Error: Server must not specify -y (csv header control)\n");
            return rtExit;
        }
        if (Settings.mss_specified)
        {
            fprintf(stderr, "Error: Server must not specify -m (mtu)\n");
            return rtExit;
        }
    }
*/

    if (Settings.host_role == _ROLE_CLIENT) // Per client: deve avere tutti i parametri corretti
    {
        if (Settings.model == _MODEL_IPV4) // Verifica model e argomenti collegati
        {
            if (Settings.remote_addr[0] == '\0') // underlay: remote_addr deve essere IP:PORT
            {
                fprintf(stderr, "Error: Client must specify IP:PORT for underlay\n");
                return rtExit;
            }
        }
        else if (Settings.model == _MODEL_DAAS)
        {
            if (Settings.remote_din < 0) // daas: remote_din >= 0
            {
                if (Settings.remote_addr[0] != '\0')
                {
                    Settings.remote_din = atoi(Settings.remote_addr);
                    if (Settings.remote_din < 0)
                    {
                        fprintf(stderr, "Error: Invalid DIN for client\n");
                        return rtExit;
                    }
                }
                else
                {
                    fprintf(stderr, "Error: Client DIN not specified\n");
                    return rtExit;
                }
            }
        }

        if (Settings.block_size < 1)
        {
            fprintf(stderr, "Error: Client must specify --blocksize >= 1\n");
            return rtExit;
        }
        if (Settings.repetitions < 1)
        {
            fprintf(stderr, "Error: repetitions must be >= 1\n");
            return rtExit;
        }
        if (Settings.mss_specified && Settings.pkt_payload < 1)
        {
            fprintf(stderr, "Error: MTU must be >= 1\n");
            return rtExit;
        }
        if (Settings.csv_enabled && Settings.csv_path[0] == '\0') // csv_path se csv_enabled deve essere valorizzato
        {
            fprintf(stderr, "Error: CSV output enabled but no file specified\n");
            return rtExit;
        }
    }

    return rtOk;
}
