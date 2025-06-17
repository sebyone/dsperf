/**
 * DaaS-IoT 2023 (@) Sebyone Srl
 *
 * File: module.h
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef SYSTEM_MODULES_H
#define SYSTEM_MODULES_H

#include "features.h"
#include "monitor.h"
#include "callback.h"

#include "system.h"

#define _ADD_OPTION(syscode, default_value) settings->addFeature(syscode, default_value)
// #define _GET_OPTION(syscode, default_value) settings->getOption(syscode)
#define _ADD_FACT(syscode, calcmode) statistics->addStatisticFact(syscode, calcmode)
#define _RELOAD_SETTINGS settings->reload()
#define _RELOAD_STATISTICS statistics->reload()

/*  -------------------------------------------------------------------------------------------------------- */
class SysModule
{
public:
    SysModule(sysgroup_t sysgroup_);
    ~SysModule();

    inline sysgroup_t getSysGroup() { return _sysgroup; }
    inline unsigned getSerial() { return _serial; }

    inline int getNVersion()
    {
        return _version;
    }

    Features *settings;
    Monitor *statistics;

    virtual void Backup(IDepot &depot_) = 0;
    virtual void Restore(IDepot &depot_) = 0;

    void setNVersion(int cver_)
    {
        _version = cver_;
    }

private:
    const sysgroup_t _sysgroup; // local features/system group
    unsigned _serial;           // it's protects ?
    int _version;
    Systemx &s = Systemx::Instance(); // singleton instance
};

#endif // SYSTEM_MODULES_H