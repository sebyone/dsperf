/**
 * DaaS-IoT 2023 (@) Sebyone Srl
 *
 * File: module.cpp
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// #include "system.h"
#include "sysmodule.h"

/*  -------------------------------------------------------------------------------------------------------- */
SysModule::SysModule(sysgroup_t sysgroup_) : _sysgroup(sysgroup_)
{
    //_sysgroup = sysgroup_;
    settings = new Features(sysgroup_);
    statistics = new Monitor(sysgroup_);
    _serial = s.registerModule(this, sysgroup_);
    // callbacks = new CallBack(_serial);
}

SysModule::~SysModule()
{

    // delete statistics;
    //  delete callbacks;
}
