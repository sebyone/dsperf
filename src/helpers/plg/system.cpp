/**
 * DaaS-IoT 2023 (@) Sebyone Srl
 *
 * File: system.cpp
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 */
#include "system.h"
#include "sysmodule.h"

/*  -------------------------------------------------------------------------------------------------------- */
// class Option

Option::Option(sysgroup_t group_, syscode_t code_, int defValue_) : group(group_), code(code_), defValue(defValue_) {}
Option::~Option() {}

/*  -------------------------------------------------------------------------------------------------------- */
// class Fact

Fact::Fact(sysgroup_t group_, syscode_t code_, unsigned mode_) : group(group_), code(code_), mode(mode_) {}
Fact::~Fact() {}

/*  -------------------------------------------------------------------------------------------------------- */
// class System // Singleton

Systemx &Systemx::Instance()
{
    static Systemx x;
    // volatile int dummy{};
    return x;
}
//
options_t *Systemx::getOptions(void) { return &allopts; }

facts_t *Systemx::getFacts(void) { return &allfacts; }

pmodules_t *Systemx::getModules(void) { return &allmodules; }

unsigned Systemx::registerModule(SysModule *pmodule, sysgroup_t sysgroup_)
{
    allmodules.push_back(pmodule);
    return (icounter++); // 0..n
}

callers_t *Systemx::getCallers(void) { return &allcallers; }

callbacks_t *Systemx::getCalls(void) { return &allcalls; }

bool Systemx::backupSystem(IDepot &repo_)
{
    for (auto ix : allmodules)
    {
        // Save module type ( creation order)
        // Save settings, statistics
        (*ix).Backup(repo_);
    }
    // ONLY IF NODE NOT RUNNING !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    return false;
}

bool Systemx::restoreSystem(IDepot &repo_)
{
    // Read block
    // switch case block of
    // _SYS_CORE single
    // _SYS_AGENT single
    // _SYS_DRIVERS multi

    return false;
}