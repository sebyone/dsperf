/*
 * DaaS-IoT 2019, 2025 (@) Sebyone Srl
 *
 * File: loopback.c
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Disclaimer of Warrant
 * Covered Software is provided under this License on an "as is" basis, without warranty of any kind, either
 * expressed, implied, or statutory, including, without limitation, warranties that the Covered  Software is
 * free of defects, merchantable, fit for a particular purpose or non-infringing.
 * The entire risk as to the quality and performance of the Covered Software is with You.  Should any Covered
 * Software prove defective in any respect, You (not any Contributor) assume the cost of any necessary
 * servicing, repair, or correction.
 * This disclaimer of warranty constitutes an essential part of this License.  No use of any Covered Software
 * is authorized under this License except under this disclaimer.
 *
 * Limitation of Liability
 * Under no circumstances and under no legal theory, whether tort (including negligence), contract, or otherwise,
 * shall any Contributor, or anyone who distributes Covered Software as permitted above, be liable to You for
 * any direct, indirect, special, incidental, or consequential damages of any character including, without
 * limitation, damages for lost profits, loss of goodwill, work stoppage, computer failure or malfunction,
 * or any and all other commercial damages or losses, even if such party shall have been informed of the
 * possibility of such damages.  This limitation of liability shall not apply to liability for death or personal
 * injury resulting from such party's negligence to the extent applicable law prohibits such limitation.
 * Some jurisdictions do not allow the exclusion or limitation of incidental or consequential damages, so this
 * exclusion and limitation may not apply to You.
 *
 * Contributors:
 * plogiacco@smartlab.it - initial design, implementation and documentation
 * sebastiano.meduri@gmail.com  - initial design, implementation and documentation
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <stdbool.h>
#include "version.h"

#include "locals.h"
#include "options.h"

// Testing modules
#include "models\model_ipv4tcp.h"

#ifdef WITH_DAAS
// #include "daas.hpp"
// #include "daas_types.hpp"
#include "models\model_daas.h"
#endif


double now_sec()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

// -------------------------------------------------------------------------------------------------------- !
int main(int argc, char *argv[])
{
    options_t Settings;
    parse_args(argc, argv, &Settings);
    if (validate_args(&Settings, argv[0]) != EXIT_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    switch (Settings.target == 3)
    {
    case 3: // throughput (default)
        switch (Settings.model)
        {
        case 0:                          // ipv4/tcp
            if (Settings.host_role == 0) // 0 = server, 1 = client
            {
                run_ipv4tcp_server(Settings.port);
            }
            else
            {
                run_ipv4tcp_client(&Settings, const char *server_ip, int server_port); // bandwidth
            };

            break;

        case 1: // daas fresbee
#ifdef WITH_DAAS
                // enable node ( if not alredy enabled)

            run_overlay_bandwidth_server(setup);
            //
            run_overlay_bandwidth_client(setup, test);
#else
                // print library not available
#endif
            break;

        case 2: // ipv4/icmp (ping)
            // set enviroment
            // run test tcp

            break;
        }
    }

    return EXIT_SUCCESS;
}
