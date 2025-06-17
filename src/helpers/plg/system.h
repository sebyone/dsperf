/**
 * DaaS-IoT 2023 (@) Sebyone Srl
 *
 * File: system.h
 * Author:
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/*  --------------------------------------------------------------------------------------------------------

        +------------+
        |   locals   | - - defined at compile-time, in main local project folder.
        +------------+     Sources and plug-ins uses same defines...

                                +------------------+
                                |      System      | --> Singleton: all Groups & codes (Options Facts)
                                +------------------+
                                  ^     ^        ^
                                  |     |        |
                         +---------+    |       +---------+
           Statistics <--| monitor |    |       | feature |--> Settings
                         +---------+    |       +---------+
                                 ^      |         ^
                                 |      |         |
                                +------------------+
                                |      Module      |  --> Instances ( serial, group )
                                +------------------+
                                        ^
                                        |
                    +----------------------------------------+
                    | All Classes defined as System's Module |
                    +----------------------------------------+

  -------------------------------------------------------------------------------------------------------- */


#ifndef SYSTEM__H
#define SYSTEM__H

#include <cstdint>
#include <vector>
#include "../locals.h"
#include "repository.h"


class SysModule;
class CallBack;

/*  -------------------------------------------------------------------------------------------------------- */
class Option // settable option
{
public:
    Option(sysgroup_t group_, syscode_t code_, int defValue_);
    ~Option();
    sysgroup_t group;
    syscode_t code;
    int defValue = 0;
};

typedef std::vector<Option> options_t;

/*  -------------------------------------------------------------------------------------------------------- */
class Fact // fact that can be monitored
{
public:
    Fact(sysgroup_t group_, syscode_t code_, unsigned mode_);
    ~Fact();
    sysgroup_t group;
    syscode_t code;
    unsigned mode; //
};

typedef std::vector<Fact> facts_t;

/*  -------------------------------------------------------------------------------------------------------- */
typedef struct
{
    syscode_t event;
    sysgroup_t to_sysgroup;
    // to_instance; Directly called !!!!!!
} call_t;

typedef struct
{
    CallBack *pcaller; /// FIXED ORDER !!!!!!!
    sysgroup_t sysgroup;
    unsigned icaller;
    unsigned priority;
} caller_t;

typedef std::vector<call_t> callbacks_t;
typedef std::vector<caller_t> callers_t;

/*  -------------------------------------------------------------------------------------------------------- */
typedef std::vector<SysModule *> pmodules_t;
/*  -------------------------------------------------------------------------------------------------------- */

class Systemx // Singleton
{
public:
    static Systemx &Instance();
    //
    options_t *getOptions(void);
    facts_t *getFacts(void);
    //
    pmodules_t *getModules(void);

    unsigned registerModule(SysModule *pmodule, sysgroup_t sysgroup_);
    //
    callers_t *getCallers(void);
    callbacks_t *getCalls(void);

    bool backupSystem(IDepot &repo_);
    bool restoreSystem(IDepot &repo_);
    
    
private:
    Systemx() = default;
    ~Systemx() = default;
    Systemx(const Systemx &) = delete;
    Systemx &operator=(const Systemx &) = delete;
    //
    options_t allopts; // Shared !!!!
    facts_t allfacts;  // volatile = threadsafe ????
    callbacks_t allcalls;
    unsigned _sys_mode = 0; // hot/cold start
    //
    callers_t allcallers;
    pmodules_t allmodules; // volatile = threadsafe ????
    //
    unsigned icounter = 100; // Module instances from 100
                             //   unsigned cbcounter = 100; // Callback intances from 100
};

#endif // SYSTEM__H
/*
     BACKUP                                                     RESTORE
     __________________________________________________________________________________________

     - DrvTAB_________________________
         + driver                                                + addDriver(URI *localUri);
             - settings
             - statistics
             - URI_____________________
                 !!! custom fields !!!

     - NodeMAP_________________________                          + addNode(din_t din_, URI *uri_);
         - settings
         - statistics
         + DND_________________________

             - state (nodestate_t)                               - updateNodeState(nodestate_t &state_);
             + links (dlx_t)                                     - addLink(URI &uri_);
             + queue (DDO)                                       - pushDDO(DDO *ddo);

     - Core____________________________
         - settings
         - statistics
         + queue (DME)                                           - routeDME(DME *dme);

     - Agent___________________________
         - settings
         - statistics

*/