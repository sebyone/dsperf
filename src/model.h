
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

       ISO-OSI                      IEEE 802.x
+-----------------------+   +---------------------------------------+
| 7.Application layer   |   |                                       |
+-----------------------+   |                                       |
| 6.Presentation layer  |   |                                       |
+-----------------------+   |                                       |
| 5.Session layer       |   |         Upper layer protocols         |
+-----------------------+   |                                       |
| 4.Transport layer     |   |                                       |
+-----------------------+   |                                       |
| 3.Network layer       |   |                                       |
+-----------------------+   +---------------------------------------+
| 2.Data link layer     |   |       Logical link control (LLC)      |
|                       |   |       Medium access control (MAC)     |
+-----------------------+   +---------------------------------------+
| 1.Physical layer      |   |                                       |
+-----------------------+   +---------------------------------------+

IEEE 802.1	    LAN Protocols Working Group
IEEE 802.3	    Ethernet
IEEE 802.11	    WLAN (Wireless LAN) Wi-Fi certification & Mesh
IEEE 802.15.3   UWB (ultra wideband, ultra-wide band and ultraband) very low energy level radio technology for short-range
IEEE 802.15.4   PAN (personal area network) Low-Rate wireless e.g., Zigbee, WirelessHART, MiWi, etc.
IEEE 802.15.6   BAN (body area network),WBAN (wireless body area network), BSN (body sensor network), MBAN (medical body area network)
IEEE 802.21     MIH (Media Independent Handoff) / DaaS
 ------------------------------------------------------------------------------------------------------------------------------ */

#ifndef HARDWARE_H
#define HARDWARE_H

#pragma once

#include "locals.h"

#include <stdlib.h>
#include <vector>


// ------------------------------------------------------------------------------------------------------------------------------!
// Generic model 

typedef enum : unsigned
{
    _PROTO_NONE = 0, // Not defined
    _PROTO_DAAS,     // DaaS-IoT
    _PROTO_IPV4,     // IP stack
    _PROTO_WLAN
} proto_t;

class IModel // Abstract model to test network
{
public:
    // Pure virtual
    virtual ~IModel() {}; // force to invoke specialized destructor

    virtual unsigned listInterfaces(int &_iflist) = 0; // return list of available media interfaces

    // Configure
    virtual int getOptions(int isub_, const unsigned char *buff, int size) = 0;
    virtual int setOptions(int isub_, const unsigned char *buff, int size) = 0;

    // Generals
    virtual bool setEnv(void) = 0;   // initializes hardware & buffers
    virtual void resetEnv(void) = 0; // flushes buffers and release resources

    // Services
    virtual int runLoopbackServer(URI *rUri) = 0; // returns: int sch !!!!!!!!!!!!!!!!!
    virtual int runTest(int isub_, unsigned char *buff, int maxSize) = 0;

    virtual int outReport(int isub_) = 0;
};

// ------------------------------------------------------------------------------------------------------------------------------!

typedef std::vector<netif_t> lnetif_t;

ret_t get_interfaces(lnetif_t *ifs); // Returns a list of availaible local interfaces ( )

#endif // HARDWARE_H
