/*
 * DaaS-IoT 2023 (@) Sebyone Srl
 *
 * File: callback.cpp
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "callback.h"
#include "sysmodule.h"

/*  -------------------------------------------------------------------------------------------------------- */

CallBack::CallBack()
{
}

CallBack::~CallBack()
{
}

/*  -------------------------------------------------------------------------------------------------------- */
unsigned CallBack::regCallerBack(sysgroup_t sysgroup_, unsigned icaller_, unsigned priority_ = 0)
{
    if (s.getCallers()->size() > 0)
    {
        auto it = s.getCallers()->begin();
        while (it != s.getCallers()->end())
        {
            if (((*it).sysgroup == sysgroup_) && ((*it).icaller == icaller_))
            {
                return icaller_;
            }
            it++;
        }
    }
    ////////////// ORDERS BY PRIORITY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    s.getCallers()->push_back( {this, sysgroup_, icaller_, priority_}); // Create a new call
    _icaller = icaller_;
    return icaller_;
}

/*  -------------------------------------------------------------------------------------------------------- */
syscode_t CallBack::addCallBack(syscode_t event_, sysgroup_t to_sysgroup_)
{
    // Threading ???
    if (s.getCalls()->size() > 0)
    {
        auto it = s.getCalls()->begin();
        while (it != s.getCalls()->end())
        {
            if (((*it).event == event_) && ((*it).to_sysgroup == to_sysgroup_))
            {
                return event_;
            }
            it++;
        }
    }
    s.getCalls()->push_back( {event_, to_sysgroup_}); // Create a new call
    return event_;
}

/*  -------------------------------------------------------------------------------------------------------- */
unsigned CallBack::callBack(syscode_t event_, int parameter_, unsigned to_icaller_ = 0)
{
    unsigned callcounter = 0;

    if (to_icaller_ > 0) // Send to icaller
    {
        for (caller_t xc : *s.getCallers())
        {
            if (xc.icaller == to_icaller_)
            {
                xc.pcaller->onCallBack(event_, xc.sysgroup, _icaller, parameter_);
                callcounter++;
            }
        }
    }
    else // Send to all sysgroup's callers
    {
        for (call_t ev : *s.getCalls())
        {
            if (ev.event == event_)
            {
                for (caller_t xc : *s.getCallers())
                {
                    if (ev.to_sysgroup == xc.sysgroup)
                    {
                        xc.pcaller->onCallBack(event_, ev.to_sysgroup, _icaller, parameter_);
                        callcounter++;
                    }
                }
            }
        }
    }
    return callcounter;
}

/*  -------------------------------------------------------------------------------------------------------- */
/*
int CallBack::retCallBack(syscode_t event_, int parameter_, unsigned int instance_)
{
    if (instance_ > 0)
    {
        for (call_t ev : *s.getCalls())
        {
            if (ev.event == event_)
            {
                auto xc = s.getCallers()->begin();
                while (xc != s.getCallers()->end() && xc->instance != instance_)
                {
                    xc++;
                }
                if ((*xc).instance == instance_)
                {
                    return (*xc).pcaller->onCallBack(event_, ev.to_sysgroup, _instance, parameter_);
                }
            }
        }
    }
    return 0;
} */
