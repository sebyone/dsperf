
#ifdef WITH_DAAS        // Quest aè una funzione di utilità che deve andare con l'sdk... 
link_t parse_link_type(int val)
{
    if (val >= _LINK_NONE && val < LINK_MAX_VAL)
    {
        return (link_t)val;
    }
    else
    {
        return _LINK_NONE;
    }
}
#endif
#ifdef WITH_DAAS
bool parse_daas_ini(const char *filepath, daas_setup_t *setup)
{
    FILE *file = fopen(filepath, "r");
    if (!file)
    {
        perror("Failed to open overlay file");
        return false;
    }

    // Inizializza valori
    setup->local_din = 0;
    for (int i = 0; i < MAX_LINKS; i++)
    {
        setup->links.links[i] = _LINK_NONE;
        setup->links.uris[i] = NULL;
    }
    for (int i = 0; i < MAX_REMOTE_LINKS; i++)
    {
        setup->maps.remote_dins[i] = 0;
        setup->maps.remote_links[i] = _LINK_NONE;
        setup->maps.uris[i] = NULL;
    }

    char line[MAX_LINE_LEN];
    enum
    {
        NONE = 0,
        LOCAL,
        MAP
    } current_section = NONE;

    while (fgets(line, sizeof(line), file))
    {
        // Rimuovi newline
        char *pos;
        if ((pos = strchr(line, '\n')))
            *pos = '\0';

        // Trim spazi iniziali
        char *start = line;
        while (isspace(*start))
            start++;

        // Ignora righe vuote e commenti
        if (*start == '\0' || *start == '#' || *start == ';')
            continue;

        // Sezione
        if (*start == '[')
        {
            if (strncmp(start, "[LOCAL]", 7) == 0)
            {
                current_section = LOCAL;
            }
            else if (strncmp(start, "[MAP]", 5) == 0)
            {
                current_section = MAP;
            }
            else
            {
                current_section = NONE; // sezione ignota
            }
            continue;
        }

        // Parsing key=value
        char key[128], value[256];
        if (sscanf(start, "%127[^=]=%255s", key, value) != 2)
        {
            // riga non conforme, salta
            continue;
        }

        // Ora processa in base alla sezione
        if (current_section == LOCAL)
        {
            if (strcmp(key, "DIN") == 0)
            {
                setup->local_din = atoi(value);
            }
            else if (strncmp(key, "LINK_", 5) == 0)
            {
                int idx = atoi(key + 5);
                if (idx >= 0 && idx < MAX_LINKS)
                {
                    setup->links.links[idx] = parse_link_type(atoi(value));
                }
            }
            else if (strncmp(key, "URI_", 4) == 0)
            {
                int idx = atoi(key + 4);
                if (idx >= 0 && idx < MAX_LINKS)
                {
                    free(setup->links.uris[idx]); // libera vecchio se presente
                    setup->links.uris[idx] = strdup(value);
                }
            }
        }
        else if (current_section == MAP)
        {
            if (strncmp(key, "REMOTE_DIN_", 11) == 0)
            {
                int idx = atoi(key + 11);
                if (idx >= 0 && idx < MAX_REMOTE_LINKS)
                {
                    setup->maps.remote_dins[idx] = atoi(value);
                }
            }
            else if (strncmp(key, "REMOTE_LINK_", 12) == 0)
            {
                int idx = atoi(key + 12);
                if (idx >= 0 && idx < MAX_REMOTE_LINKS)
                {
                    setup->maps.remote_links[idx] = parse_link_type(atoi(value));
                }
            }
            else if (strncmp(key, "REMOTE_URI_", 11) == 0)
            {
                int idx = atoi(key + 11);
                if (idx >= 0 && idx < MAX_REMOTE_LINKS)
                {
                    free(setup->maps.uris[idx]);
                    setup->maps.uris[idx] = strdup(value);
                }
            }
        }
    }

    fclose(file);
    return true;
}

#endif