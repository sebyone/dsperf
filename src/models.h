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

/* ------------------------------------------------------------------------------------------------------------------------------

      ISO-OSI                         IEEE 802.x (--ipv4)                       DaaS/Meshin (--daas)
+-----------------------+   +---------------------------------------+   +--------------------------------------+
| 7.Application layer   |   |                                       |   |  Service Layer  (OS-SDK)             |    Overlay
+-----------------------+   |                                       |   +--------------------------------------+
| 6.Presentation layer  |   |                                       |   |  Interface Layer   (API)             |
+-----------------------+   |                                       |   +--------------------------------------+
| 5.Session layer       |   |                                       |   |  Control Layer   (AGENT)             |
+-----------------------+   |                                       |   +--------------------------------------+
| 4.Transport layer     |   |                                       |   |  Mapping Layer    (CORE)             |
+-----------------------+   |                                       |   +--------------------------------------+
| 3.Network layer       |   | Upper layer protocols                 |   |  Channel layer (Drivers)             |
+-----------------------+   +---------------------------------------+   +--------------------------------------+    -----
| 2.Data link layer     |   | Logical link control (LLC)            |   |  lower layer protocols               |    Underlay
+-----------------------+   | Medium access control (MAC)           |   |                                      |
| 1.Physical layer      |   |                                       |   |                                      |
+-----------------------+   +---------------------------------------+   +--------------------------------------+

IEEE 802.1	    LAN Protocols Working Group
IEEE 802.3	    Ethernet specifications
IEEE 802.11	    WLAN (Wireless LAN) Wi-Fi certification & Mesh
IEEE 802.15.3   UWB (ultra wideband, wide band and ultraband), low energy level radio technology for short-range.
IEEE 802.15.4   PAN (personal area network) Low-Rate wireless e.g., Zigbee, WirelessHART, MiWi, etc.
IEEE 802.15.6   BAN (body area network),
               WBAN (wireless body area network),
               BSN (body sensor network),
               MBAN (medical body area network)
IEEE 802.21     MIH (Media Independent Handoff) / DaaS
------------------------------------------------------------------------------------------------------------------------------ */

#ifndef MODELS_H
#define MODELS_H

#pragma once
#include <stdlib.h>
#include <vector>

#include "globals.h"

typedef enum // Testing Models
{
    __unsetted = 0,
    __Capacity,     // throughtput: data quantity/transfer time (traffic generator),  % = throughtput / nominal bandwidth
    __Capabilities, // compliance with features matrix
    __Availability, // service continuity in time windows
    __Security      // security asserts test: data sniffing, peer shadow identity
} tclass_t;

typedef struct
{
    nproto_t nproto;   // Protocol
    tclass_t tclass;   // Model
    unsigned testerid; // Variant
    const char *title;
    const char *info;
    const char *team;
} model_info_t, *pmodel_info_t;

typedef enum
{
    _OUTS_CSV_HEADER = 1, // header
    _OUTS_CSV_ROW,        // simple line
    _OUTS_SUMMARY,
} report_item_t;

// ------------------------------------------------------------------------------------------------------------------------------!
/*
class ITesterModel // Abstract test model_protocol
{
public:
    // Pure virtual constructor
    virtual ~ITesterModel() {};                        // force to invoke specialized destructor
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

// typedef std::vector<netif_t> lnetif_t;
// ret_t get_hwif_ipv4tcp(lnetif_t *ifs); // Returns a list of availaible local interfaces ( )
*/

#endif // MODELS_H
