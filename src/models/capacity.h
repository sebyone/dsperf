/*
 * DaaS-IoT 2019, 2025 (@) Sebyone Srl
 *
 * File: model.h
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

#ifndef MODEL_CAPACITY_H
#define MODEL_CAPACITY_H

#pragma once
#include <stdlib.h>
#include <vector>

#include "../models.h"

// ------------------------------------------------------------------------
// MODEL VARS CAPACITY
// ------------------------------------------------------------------------

typedef struct // Model vars Capacity test
{
    int tstcounter;       //  1. Executions counter
                          //
    double blocksize;     //  2. Traffic data size [MB] ( datatosend )
    double timeslot;      //  1. Traffic in time slot [s]
                          //
    double pktstosend;    //  7. Pkts to send
                          //
    double pktheader;     //  5. Header [bytes]
    double pktpayload;    //  4. Payload size [bytes]
    double pktefficiency; //  6. Protocol Efficiency [%]   - ratio: [%] = payload / packet_size ( header+payload )
                          //
    double datasent;      // 10. Traffic data sent [MB]
    double dataloss;      //  9. Traffic loss  = pktssent - pktstosend
                          //
    double pktssent;      //  8. Pkt sent       - counter of packet really sended ( check socket buffering settings !!!!!!!)
    double pktsloss;      //  9. Pkt loss  = pktssent - pktstosend     - ???????????????????????
                          //
    double setuptime;     // 12. Total links management time  [ms]
    double totaltime;     // 12. Total elapsed test time [ms]
                          //
    double bandwidth;     // set reference to nominal speed [Mbps]
    double jitter;        // 11. Pkt Err.[%]
                          //
    double throughput;    // 13. Throughput [Mbps]
                          //
    double tsterrors;     // 11.
                          //
} capacity_vars_t;

// ------------------------------------------------------------------------------------------------------------------------------!
rt_t resetVars(capacity_vars_t &_cvars);
rt_t report_capacity(const model_info_t &info_, capacity_vars_t &vars_, report_item_t switch_);

#endif // MODEL_CAPACITY_H
