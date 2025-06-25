/*
 * DaaS-IoT 2019, 2025 (@) Sebyone Srl
 *
 * File: daas.h
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
 * lgrillo@sebyone.com  - implementation and documentation
 *
 */


// CLI.h opensource

#ifndef DAAS_H
#define DAAS_H

#include "daas_types.hpp"

/* ----------------------------------------------------------------------------------------------------------- */

class DaasAPI : public DaaSEvent // SDK CPP11 API
{
public:
    DaasAPI();                                    // setOnChangesCallback(void (*func_cb)(unsigned, unsigned, unsigned, int))
    DaasAPI(IDaasApiEvent *);                     // setOnChangesCallback(void (*func_cb)(unsigned, unsigned, unsigned, int))
    DaasAPI(IDaasApiEvent *, const char *lhver_); // setOnChangesCallback(void (*func_cb)(unsigned, unsigned, unsigned, int))
    ~DaasAPI();

    //////////////////////////////////////////////////////////////////////
    ////  G E N E R A L S                                             ////
    //////////////////////////////////////////////////////////////////////

    const char *getVersion();                 // returns local daas-stack details
    daas_error_t doInit(din_t sid_, din_t din_);  // initializes services and resources (Real-Time or Multi-Threading, release dependent)
    daas_error_t doPerform(performs_mode_t mode); // perform node's task ( in RT mode needs to be called cyclically)
    daas_error_t doEnd();                         // releases resources and deactivates node
    daas_error_t doReset();                       // reset resources and restarts services

    //////////////////////////////////////////////////////////////////////
    // void onChanges(unsigned event_, unsigned from_sysgroup_, unsigned from_icaller_, int parameter_) { }
    //////////////////////////////////////////////////////////////////////
    ////  _ddo_ready: -> din                                          ////
    ////  _map_fetched -> din                                         ////
    ////  _channel_faults  -> to_remote                               ////
    ////  _channel_available -> to_remote                             ////
    //////////////////////////////////////////////////////////////////////

    nodestate_t getStatus(); // returns local node's instance status
    apilist_t listSystems(); // lists all active system's modules

    const char * listAvailableDrivers();                                 // Returns drivers list: 1="INET4", 2="UART", 3="MQTT"
    daas_error_t enableDriver(link_t driver_id, const char * local_uri); // Configure driver for network technology (links)

    apilist_t getSystemSettings(const uint32_t id_system);                                                          // get/set local stack running options
    bool setSystemOption(const uint32_t id_system, const uint32_t id_option, const uint32_t option_value_); // get/set local stack running options

    apilist_t getSystemStatistics(const uint32_t id_system); // set local stack running options
    bool doStatisticsReset(const uint32_t id_system);        // Reset system's statistics data

    /* Mapping      -------------------------------------------------------------------------------------------- */
    daas_error_t map(din_t din);                                                   // adds new node to local instance
    daas_error_t map(din_t din, link_t link_, const char* suri);                   // adds node-identifier and related physical address ( link: 1="INET4", 2="UART", 3="MQTT5")
    daas_error_t map(din_t din, link_t link_, const char* suri, const char* skey); // adds node-identifier and related physical address ( link: 1="INET4", 2="UART", 3="MQTT5")
    daas_error_t remove(din_t din);

    /* Availability -------------------------------------------------------------------------------------------- */
    dinlist_t listNodes();          // Returns map entries  (knows nodes) ( din1, din2, )
    daas_error_t locate(din_t din); // Verify data in mapping-table and return “availability” (no network's activities)

    /* Security     -------------------------------------------------------------------------------------------- */
    nodestate_t lock(const char* skey, unsigned policy_); // → Update security policy for local node !!!!!!!!!!!!!

    /* Transfer     -------------------------------------------------------------------------------------------- */
    tsetlist_t &listTypesets();                                            // Returns list of user-defined typesets
    daas_error_t addTypeset(const uint16_t typeset_code, const uint16_t typeset_size); // Adds user-defined typeset

    daas_error_t pull(din_t din, DDO **inboundDDO); // → return first DDO received, if it exists
    daas_error_t push(din_t din, DDO *outboundDDO); // → send a DDO to "din"

private:
    // din_t _lsid;
    // din_t _ldin;
    char _lhver[8];
    IDaasApiEvent *daasApiEvents;

    // Ereditato tramite DaaSEvent
    int daasEvent(int, int, int, int) override;
};

#endif // DAASIOT_H
