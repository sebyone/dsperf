
/*
 * DaaS-IoT 2019, 2025 (@) Sebyone Srl
 *
 * File: block_runner.h
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

#ifndef HARDWARE_H
#define HARDWARE_H

#pragma once

#include "locals.h"

// Data Structures for local hardware resources
//
typedef struct // local interfaces
{
    char *ifname;     // name as OS reported "eth", "wlan", ...
    double bandwidth; // nominal Speed: 50000Mb/s

    /*
  $ ethtool eth0
    Settings for eth0:
    Supported ports: [ FIBRE ]
    Supported link modes:   25000baseCR/Full, 50000baseCR2/Full
    Supported pause frame use: Symmetric Receive-only
    Supports auto-negotiation: Yes
    Supported FEC modes: RS	 BASER	 LLRS
    Advertised link modes:  25000baseCR/Full
                            50000baseCR2/Full
    Advertised pause frame use: Symmetric
    Advertised auto-negotiation: Yes
    Advertised FEC modes: Not reported
    Link partner advertised link modes:  Not reported
    Link partner advertised pause frame use: Symmetric
    Link partner advertised auto-negotiation: No
    Link partner advertised FEC modes: Not reported
    Speed: 50000Mb/s
    Lanes: 2
    Duplex: Full
    Auto-negotiation: on
    Port: FIBRE
    PHYAD: 0
    Transceiver: internal
    netlink error: Operation not permitted
    Current message level: 0x00002081 (8321)
                           drv tx_err hw
    Link detected: yes
    */
} hwif_t;

ret_t get_interfaces(hwif_t *ifs, int *_lsize); // Returns a list of availaible local interfaces ( )

/* Linux-specific socket ioctls */
#define SIOCINQ     FIONREAD
#define SIOCOUTQ    TIOCOUTQ
  
/* Routing table calls. */
#define SIOCADDRT   0x890B      /* add routing table entry  */
#define SIOCDELRT   0x890C      /* delete routing table entry   */
#define SIOCRTMSG   0x890D      /* call to routing system   */
  
/* Socket configuration controls. */
#define SIOCGIFNAME 0x8910      /* get iface name       */
#define SIOCSIFLINK 0x8911      /* set iface channel        */
#define SIOCGIFCONF 0x8912      /* get iface list       */
#define SIOCGIFFLAGS    0x8913      /* get flags            */
#define SIOCSIFFLAGS    0x8914      /* set flags            */
#define SIOCGIFADDR 0x8915      /* get PA address       */
#define SIOCSIFADDR 0x8916      /* set PA address       */
#define SIOCGIFDSTADDR  0x8917      /* get remote PA address    */
#define SIOCSIFDSTADDR  0x8918      /* set remote PA address    */
#define SIOCGIFBRDADDR  0x8919      /* get broadcast PA address */
#define SIOCSIFBRDADDR  0x891a      /* set broadcast PA address */
#define SIOCGIFNETMASK  0x891b      /* get network PA mask      */
#define SIOCSIFNETMASK  0x891c      /* set network PA mask      */
#define SIOCGIFMETRIC   0x891d      /* get metric           */
#define SIOCSIFMETRIC   0x891e      /* set metric           */
#define SIOCGIFMEM  0x891f      /* get memory address (BSD) */
#define SIOCSIFMEM  0x8920      /* set memory address (BSD) */
#define SIOCGIFMTU  0x8921      /* get MTU size         */
#define SIOCSIFMTU  0x8922      /* set MTU size         */
#define SIOCSIFNAME 0x8923      /* set interface name */
#define SIOCSIFHWADDR   0x8924      /* set hardware address     */
#define SIOCGIFENCAP    0x8925      /* get/set encapsulations       */
#define SIOCSIFENCAP    0x8926      
#define SIOCGIFHWADDR   0x8927      /* Get hardware address     */
#define SIOCGIFSLAVE    0x8929      /* Driver slaving support   */
#define SIOCSIFSLAVE    0x8930
#define SIOCADDMULTI    0x8931      /* Multicast address lists  */
#define SIOCDELMULTI    0x8932
#define SIOCGIFINDEX    0x8933      /* name -> if_index mapping  */
#define SIOGIFINDEX SIOCGIFINDEX    /* misprint compatibility :-)   */
#define SIOCSIFPFLAGS   0x8934      /* set/get extended flags set   */
#define SIOCGIFPFLAGS   0x8935
#define SIOCDIFADDR 0x8936      /* delete PA address        */
#define SIOCSIFHWBROADCAST  0x8937  /* set hardware broadcast addr  */
#define SIOCGIFCOUNT    0x8938      /* get number of devices */
  
#define SIOCGIFBR   0x8940      /* Bridging support     */
#define SIOCSIFBR   0x8941      /* Set bridging options     */
  
#define SIOCGIFTXQLEN   0x8942      /* Get the tx queue length  */
#define SIOCSIFTXQLEN   0x8943      /* Set the tx queue length  */
  
/* SIOCGIFDIVERT was:   0x8944      Frame diversion support */
/* SIOCSIFDIVERT was:   0x8945      Set frame diversion options */
  
#define SIOCETHTOOL 0x8946      /* Ethtool interface        */
  
#define SIOCGMIIPHY 0x8947      /* Get address of MII PHY in use. */
#define SIOCGMIIREG 0x8948      /* Read MII PHY register.   */
#define SIOCSMIIREG 0x8949      /* Write MII PHY register.  */
  
#define SIOCWANDEV  0x894A      /* get/set netdev parameters    */
  
/* ARP cache control calls. */
            /*  0x8950 - 0x8952  * obsolete calls, don't re-use */
#define SIOCDARP    0x8953      /* delete ARP table entry   */
#define SIOCGARP    0x8954      /* get ARP table entry      */
#define SIOCSARP    0x8955      /* set ARP table entry      */
  
/* RARP cache control calls. */
#define SIOCDRARP   0x8960      /* delete RARP table entry  */
#define SIOCGRARP   0x8961      /* get RARP table entry     */
#define SIOCSRARP   0x8962      /* set RARP table entry     */
  
/* Driver configuration calls */
  
#define SIOCGIFMAP  0x8970      /* Get device parameters    */
#define SIOCSIFMAP  0x8971      /* Set device parameters    */
  
/* DLCI configuration calls */
  
#define SIOCADDDLCI 0x8980      /* Create new DLCI device   */
#define SIOCDELDLCI 0x8981      /* Delete DLCI device       */
  
#define SIOCGIFVLAN 0x8982      /* 802.1Q VLAN support      */
#define SIOCSIFVLAN 0x8983      /* Set 802.1Q VLAN options  */
  
/* bonding calls */
  
#define SIOCBONDENSLAVE 0x8990      /* enslave a device to the bond */
#define SIOCBONDRELEASE 0x8991      /* release a slave from the bond*/
#define SIOCBONDSETHWADDR      0x8992   /* set the hw addr of the bond  */
#define SIOCBONDSLAVEINFOQUERY 0x8993   /* rtn info about slave state   */
#define SIOCBONDINFOQUERY      0x8994   /* rtn info about bond state    */
#define SIOCBONDCHANGEACTIVE   0x8995   /* update to a new active slave */
  
/* bridge calls */
#define SIOCBRADDBR     0x89a0      /* create new bridge device     */
#define SIOCBRDELBR     0x89a1      /* remove bridge device         */
#define SIOCBRADDIF 0x89a2      /* add interface to bridge      */
#define SIOCBRDELIF 0x89a3      /* remove interface from bridge */
  
/* hardware time stamping: parameters in linux/net_tstamp.h */
#define SIOCSHWTSTAMP   0x89b0
  
/* Device private ioctl calls */
  
/*
 *  These 16 ioctls are available to devices via the do_ioctl() device
 *  vector. Each device should include this file and redefine these names
 *  as their own. Because these are device dependent it is a good idea
 *  _NOT_ to issue them to random objects and hope.
 *
 *  THESE IOCTLS ARE _DEPRECATED_ AND WILL DISAPPEAR IN 2.5.X -DaveM
 */
  
#define SIOCDEVPRIVATE  0x89F0  /* to 89FF */


#endif // HARDWARE_H

