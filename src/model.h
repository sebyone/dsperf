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

#ifndef MODELS_H
#define MODELS_H

#pragma once
#include "globals.h"
#include <stdlib.h>
#include <vector>

typedef struct
{
    nproto_t nproto;
    tclass_t tclass;
    unsigned nmodel;
    const char *title;
    const char *info;
    const char *team;
} model_info_t, *pmodel_info_t;

typedef enum
{
    _OUTS_CSV_HEADER = 1, // header
    _OUTS_CSV_ROW,        // simple line
    _OUTS_SUMMARY,
    _OUTS_SUMMARY_ROW
} report_item_t;

// ------------------------------------------------------------------------
// MODEL CLASS CAPACITY
// ------------------------------------------------------------------------
#define _CAPACITY_KEYS_COUNTER 13 // TODO: usare struct !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
enum capacity_keys                // Class Model Keys
{
    _tstcounter = 0, //  1. test executions
    _blocksize,      //  2. Traffic block size [MB]
    _protocol,       //  3. TEST_MODEL_NAME - Setted
    _pktpayload,     //  4. Payload size [bytes]
    _pktheader,      //  5. Header [bytes]
    _efficiency,     //  6. Efficiency [%]   - ratio: [%] = payload / packet_size ( header+payload )
    _pktstosend,     //  7. Pkts to send
    _pktssent,       //  8. Pkt sent       - counter of packet really sended ( check socket buffering settings !!!!!!!)
    _pktsloss,       //  9. Pkt loss       - ???????????????????????
    _datasent,       // 10. Data Sent [Mb]
    _pktserror,      // 11. Pkt Err.[%]
    _ttime,          // 12. Transfer Time [ms]
    _throughput      // 13. Throughput [Mbps]
};

typedef double capacity_vars_t[_CAPACITY_KEYS_COUNTER];
void report_capacity(pmodel_info_t model_, capacity_vars_t &vars_, report_item_t switch_);

// ------------------------------------------------------------------------------------------------------------------------------!
class ITestModel // Abstract test model_protocol
{
public:
    // Pure virtual constructor
    virtual ~ITestModel() {}; // force to invoke specialized destructor
    virtual unsigned listInterfaces(int &_iflist) = 0; // returns available local hardware for protocol

    // Configure
    virtual int getOptions(int isub_, const unsigned char *buff, int size) = 0;
    virtual int setOptions(int isub_, const unsigned char *buff, int size) = 0;

    // Generals
    virtual bool setEnv(void) = 0;   // initializes hardware & buffers
    virtual void resetEnv(void) = 0; // flushes buffers and release resources

    // Services
    virtual int runLoopbackServer() = 0; // returns: int sch !!!!!!!!!!!!!!!!!
    virtual int runTest(int isub_, unsigned char *buff, int maxSize) = 0;

private:
    virtual int report(int isub_) = 0;
};

typedef std::vector<netif_t> lnetif_t;
ret_t get_hwif_ipv4tcp(lnetif_t *ifs); // Returns a list of availaible local interfaces ( )

#endif // MODELS_H
