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

#include "version.h"

#include "locals.h"
#include "options.h"
// #include "helpers/monitor.h"

extern Settings; // options_t Settings;

// -------------------------------------------------------------------------------------------------------- !
int main(int argc, char *argv[])
{

    parse_args(argc, argv);
    if (validate_args(argv[0]) != rtOk)
    {
        return EXIT_FAILURE;
    }

    switch (Settings.model)
    {

#ifdef TEST_IPV4TCP
    case TEST_IPV4TCP: // TEST_CLASS_CAPACITY, MODEL: ipv4/tcp
        if (check_ipv4tcp())
        {
            if (Settings.host_role == 0) // 0 = server, 1 = client
            {
                run_ipv4tcp_server(Settings.port);
            }
            else
            {
                run_ipv4tcp_client(&Settings, Settings.remote_addr, Settings.port); // bandwidth
            };
            return EXIT_SUCCESS;
        }
        break;
#endif
#ifdef TEST_IPV4UDP
    case 2:                          // MODEL: ipv4/udp
        if (Settings.host_role == 0) // 0 = server, 1 = client
        {
            run_ipv4udp_server(Settings.port);
        }
        else
        {
            run_ipv4udp_client(&Settings, const char *server_ip, int server_port); // bandwidth
        };
        return EXIT_SUCCESS;
        break;
#endif
#ifdef WITH_DAAS
    case 7:                          // daas fresbee
        if (Settings.host_role == 0) // 0 = server, 1 = client
        {
            run_ipv4tcp_server(Settings.port);
        }
        else
        {
            run_ipv4tcp_client(&Settings, const char *server_ip, int server_port); // bandwidth
        };
        return EXIT_SUCCESS;
        break;
#endif
    case 8: // ipv4/icmp (ping)
        // set enviroment
        // run test tcp

        break;

    default:
        // print out test not available !!!
    }

    return EXIT_FAILURE;
}
