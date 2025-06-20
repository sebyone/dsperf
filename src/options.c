
#include "options.h"
#include <ctype.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>


static options_t Settings;

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
void print_options(const char *prog_name)
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
ret_t parse_args(int argc, char *argv[])
{

    memset(&Settings, 0, sizeof(&Settings));
    Settings.host_role = -1;
    Settings.model = -1;
    Settings.block_size = 0;
    Settings.mtu_specified = false;
    Settings.pkt_payload = 1500;
    Settings.repetitions = 1;
    Settings.pack_num = 1;
    Settings.csv_enabled = false;
    Settings.csv_no_header = false;
    Settings.version = false;
    Settings.port = 0;
    Settings.remote_din = -1;
    Settings.remote_addr[0] = '\0';
    Settings.model_path[0] = '\0';
    Settings.csv_path[0] = '\0';

    static struct option long_options[] = {
        {"underlay", no_argument, 0, 1},
        {"daas", required_argument, 0, 2},
        {"blocksize", required_argument, 0, 3},
        {0, 0, 0, 0}};

    int option_index = 0;
    int c;

    while ((c = getopt_long(argc, argv, "S:s:n:c:m:f:t:y:v", long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 'S':
            if (Settings.host_role != -1)
            {
                fprintf(stderr, "Error: Cannot specify both -S and -s\n");
                exit(EXIT_FAILURE);
            }
            Settings.host_role = 0;
            if (Settings.model == 0)
            { // underlay: PORT
                Settings.port = atoi(optarg);
                if (Settings.port <= 0)
                {
                    fprintf(stderr, "Error: Invalid port number for server\n");
                    exit(EXIT_FAILURE);
                }
            }
            else if (Settings.model == 1)
            {
                Settings.remote_din = atoi(optarg);
                if (Settings.remote_din < 0)
                {
                    fprintf(stderr, "Error: Invalid DIN for server\n");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                // Layer mode non ancora definito: salvare comunque arg e fare check dopo
                // Conserviamo il valore provvisorio in remote_addr o remote_din, controlliamo in validate
                // Per semplicità: useremo remote_addr come buffer temporaneo per il valore
                strncpy(Settings.remote_addr, optarg, sizeof(Settings.remote_addr) - 1);
            }
            break;

        case 's':
            if (Settings.host_role != -1)
            {
                fprintf(stderr, "Error: Cannot specify both -S and -s\n");
                exit(EXIT_FAILURE);
            }
            Settings.host_role = 1;
            if (Settings.model == 0)
            { // underlay: IP:PORT
                strncpy(Settings.remote_addr, optarg, sizeof(Settings.remote_addr) - 1);
            }
            else if (Settings.model == 1)
            {
                Settings.remote_din = atoi(optarg);
                if (Settings.remote_din < 0)
                {
                    fprintf(stderr, "Error: Invalid DIN for client\n");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                // Layer mode non ancora definito: salvare in remote_addr e validare dopo
                strncpy(Settings.remote_addr, optarg, sizeof(Settings.remote_addr) - 1);
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
            Settings.pack_num = atoi(optarg);
            if (Settings.pack_num < 1)
            {
                fprintf(stderr, "Error: packet number must be >= 1\n");
                exit(EXIT_FAILURE);
            }
            break;

        case 'm':
            Settings.mtu_specified = true;
            Settings.pkt_payload = atoi(optarg);
            if (Settings.pkt_payload < 1)
            {
                fprintf(stderr, "Error: MTU must be >= 1\n");
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
            Settings.version = true;
            break;

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

        case 3: // --blocksize
            Settings.block_size = atoi(optarg);
            if (Settings.block_size < 1)
            {
                fprintf(stderr, "Error: blocksize must be >= 1\n");
                exit(EXIT_FAILURE);
            }
            break;

        default:
            fprintf(stderr, "Unknown option\n");
            exit(EXIT_FAILURE);
        }
    }
}

// -------------------------------------------------------------------------------------------------------- !
ret_t validate_args(const char *prog_name)
{
    // Controlli base
    if (Settings.host_role == -1)
    {
        fprintf(stderr, "Error: Must specify either -S (server) or -s (client)\n");
        return rtExit;
    }
    if (Settings.model == -1)
    {
        fprintf(stderr, "Error: Must specify either --underlay or --daas\n");
        return rtExit;
    }

    // Per server: accetta solo parametri base
    if (Settings.host_role == 0)
    {
        // Se model non è definito, proviamo a dedurlo dall'argomento di -S
        if (Settings.model == 0)
        {
            // -S deve essere porta
            if (Settings.port == 0)
            {
                // Se non settata ancora, proviamo a convertire da remote_addr (tmp)
                if (Settings.remote_addr[0] != '\0')
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
            // -S deve essere remote_din
            if (Settings.remote_din < 0)
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

        // Verifica che non siano presenti opzioni non ammesse per server
        if (Settings.block_size != 0)
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
        if (Settings.mtu_specified)
        {
            fprintf(stderr, "Error: Server must not specify -m (mtu)\n");
            return rtExit;
        }
    }

    // Per client: deve avere tutti i parametri corretti
    if (Settings.host_role == 1)
    {
        // Verifica model e argomenti collegati
        if (Settings.model == 0)
        {
            // underlay: remote_addr deve essere IP:PORT
            if (Settings.remote_addr[0] == '\0')
            {
                fprintf(stderr, "Error: Client must specify IP:PORT for underlay\n");
                return rtExit;
            }
        }
        else if (Settings.model == 1)
        {
            // daas: remote_din >= 0
            if (Settings.remote_din < 0)
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
        if (Settings.mtu_specified && Settings.pkt_payload < 1)
        {
            fprintf(stderr, "Error: MTU must be >= 1\n");
            return rtExit;
        }

        // csv_path se csv_enabled deve essere valorizzato
        if (Settings.csv_enabled && Settings.csv_path[0] == '\0')
        {
            fprintf(stderr, "Error: CSV output enabled but no file specified\n");
            return rtExit;
        }
    }

    return EXIT_SUCCESS;
}
