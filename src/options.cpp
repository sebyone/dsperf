
#include "options.h"

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
ret_t parse_args(int argc, char *argv[], options_t *_settings)
{
    memset(_settings, 0, sizeof(*_settings));
    _settings->host_role = -1;
    _settings->model = -1;
    _settings->block_size = 0;
    _settings->mtu_defined = false;
    _settings->pkt_payload = 1500;
    _settings->repetitions = 1;
    _settings->pack_num = 1;
    _settings->csv_enabled = false;
    _settings->csv_no_header = false;
    _settings->version = false;
    _settings->port = 0;
    _settings->remote_din = -1;
    _settings->remote_addr[0] = '\0';
    _settings->model_path[0] = '\0';
    _settings->csv_path[0] = '\0';

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
            if (_settings->host_role != -1)
            {
                fprintf(stderr, "Error: Cannot specify both -S and -s\n");
                exit(EXIT_FAILURE);
            }
            _settings->host_role = 0;
            if (_settings->model == 0)
            { // underlay: PORT
                _settings->port = atoi(optarg);
                if (_settings->port <= 0)
                {
                    fprintf(stderr, "Error: Invalid port number for server\n");
                    exit(EXIT_FAILURE);
                }
            }
            else if (_settings->model == 1)
            {
                _settings->remote_din = atoi(optarg);
                if (_settings->remote_din < 0)
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
                strncpy(_settings->remote_addr, optarg, sizeof(_settings->remote_addr) - 1);
            }
            break;

        case 's':
            if (_settings->host_role != -1)
            {
                fprintf(stderr, "Error: Cannot specify both -S and -s\n");
                exit(EXIT_FAILURE);
            }
            _settings->host_role = 1;
            if (_settings->model == 0)
            { // underlay: IP:PORT
                strncpy(_settings->remote_addr, optarg, sizeof(_settings->remote_addr) - 1);
            }
            else if (_settings->model == 1)
            {
                _settings->remote_din = atoi(optarg);
                if (_settings->remote_din < 0)
                {
                    fprintf(stderr, "Error: Invalid DIN for client\n");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                // Layer mode non ancora definito: salvare in remote_addr e validare dopo
                strncpy(_settings->remote_addr, optarg, sizeof(_settings->remote_addr) - 1);
            }
            break;

        case 'n':
            _settings->repetitions = atoi(optarg);
            if (_settings->repetitions < 1)
            {
                fprintf(stderr, "Error: repetitions must be >= 1\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'c':
            _settings->pack_num = atoi(optarg);
            if (_settings->pack_num < 1)
            {
                fprintf(stderr, "Error: packet number must be >= 1\n");
                exit(EXIT_FAILURE);
            }
            break;

        case 'm':
            _settings->mtu_defined = true;
            _settings->pkt_payload = atoi(optarg);
            if (_settings->pkt_payload < 1)
            {
                fprintf(stderr, "Error: MTU must be >= 1\n");
                exit(EXIT_FAILURE);
            }
            break;

        case 'f':
        {
            _settings->csv_enabled = true;
            strncpy(_settings->csv_path, optarg, sizeof(_settings->csv_path) - 1);
            break;
        }

        case 't':
        {
            _settings->time_defined = true;
            int val = atoi(optarg);
            _settings->time = val;
            break;
        }
        case 'y':
        {
            _settings->csv_format = true;
            _settings->csv_no_header = false;

            int val = atoi(optarg);
            if (val != 0 && val != 1)
            {
                fprintf(stderr, "Errore: il valore per --csv-no-header (-y) deve essere 0 o 1.\n");
                exit(EXIT_FAILURE);
            }

            _settings->csv_no_header = (val == 1);
        }
        break;

        case 'v':
            _settings->version = true;
            break;

        case 1: // --underlay
            if (_settings->model != -1)
            {
                fprintf(stderr, "Error: Cannot specify both --underlay and --daas\n");
                exit(EXIT_FAILURE);
            }
            _settings->model = 0;
            break;

        case 2: // --daas
            if (_settings->model != -1)
            {
                fprintf(stderr, "Error: Cannot specify both --underlay and --daas\n");
                exit(EXIT_FAILURE);
            }
            _settings->model = 1;
            strncpy(_settings->model_path, optarg, sizeof(_settings->model_path) - 1);
            break;

        case 3: // --blocksize
            _settings->block_size = atoi(optarg);
            if (_settings->block_size < 1)
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
ret_t validate_args(options_t *args, const char *prog_name)
{
    // Controlli base
    if (args->host_role == -1)
    {
        fprintf(stderr, "Error: Must specify either -S (server) or -s (client)\n");
        return rtExit;
    }
    if (args->model == -1)
    {
        fprintf(stderr, "Error: Must specify either --underlay or --daas\n");
        return rtExit;
    }

    // Per server: accetta solo parametri base
    if (args->host_role == 0)
    {
        // Se model non è definito, proviamo a dedurlo dall'argomento di -S
        if (args->model == 0)
        {
            // -S deve essere porta
            if (args->port == 0)
            {
                // Se non settata ancora, proviamo a convertire da remote_addr (tmp)
                if (args->remote_addr[0] != '\0')
                {
                    args->port = atoi(args->remote_addr);
                    if (args->port <= 0)
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
        else if (args->model == 1)
        {
            // -S deve essere remote_din
            if (args->remote_din < 0)
            {
                if (args->remote_addr[0] != '\0')
                {
                    args->remote_din = atoi(args->remote_addr);
                    if (args->remote_din < 0)
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
        if (args->block_size != 0)
        {
            fprintf(stderr, "Error: Server must not specify --blocksize\n");
            return rtExit;
        }
        if (args->repetitions != 1)
        {
            fprintf(stderr, "Error: Server must not specify -n (repetitions)\n");
            return rtExit;
        }
        if (args->csv_enabled)
        {
            fprintf(stderr, "Error: Server must not specify -f (csv output)\n");
            return rtExit;
        }
        if (args->csv_no_header)
        {
            fprintf(stderr, "Error: Server must not specify -y (csv header control)\n");
            return rtExit;
        }
        if (args->mtu_defined)
        {
            fprintf(stderr, "Error: Server must not specify -m (mtu)\n");
            return rtExit;
        }
    }

    // Per client: deve avere tutti i parametri corretti
    if (args->host_role == 1)
    {
        // Verifica model e argomenti collegati
        if (args->model == 0)
        {
            // underlay: remote_addr deve essere IP:PORT
            if (args->remote_addr[0] == '\0')
            {
                fprintf(stderr, "Error: Client must specify IP:PORT for underlay\n");
                return rtExit;
            }
        }
        else if (args->model == 1)
        {
            // daas: remote_din >= 0
            if (args->remote_din < 0)
            {
                if (args->remote_addr[0] != '\0')
                {
                    args->remote_din = atoi(args->remote_addr);
                    if (args->remote_din < 0)
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

        if (args->block_size < 1)
        {
            fprintf(stderr, "Error: Client must specify --blocksize >= 1\n");
            return rtExit;
        }
        if (args->repetitions < 1)
        {
            fprintf(stderr, "Error: repetitions must be >= 1\n");
            return rtExit;
        }
        if (args->mtu_defined && args->pkt_payload < 1)
        {
            fprintf(stderr, "Error: MTU must be >= 1\n");
            return rtExit;
        }

        // csv_path se csv_enabled deve essere valorizzato
        if (args->csv_enabled && args->csv_path[0] == '\0')
        {
            fprintf(stderr, "Error: CSV output enabled but no file specified\n");
            return rtExit;
        }
    }

    return EXIT_SUCCESS;
}
