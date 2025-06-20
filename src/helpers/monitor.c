/*
 * DaaS-IoT 2023 (@) Sebyone Srl
 *
 * File: config.cpp
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "monitor.h"

Monitor::Monitor(sysgroup_t sysgroup_) : _sysgroup(sysgroup_)
{
    
}

Monitor::~Monitor()
{

}

/*  -------------------------------------------------------------------------------------------------------- */
Fact *Monitor::getFact(syscode_t code_)
{
    if ((*it).code != code_) // && sysgroup !!!!!!!!!!!!!
    {
        it = s.getFacts()->begin();
        while (it != s.getFacts()->end() && (*it).code != code_)
        {
            it++;
        }
    }
    if ((*it).code == code_)
    {
        return &(*it);
    }
    return NULL;
}

/*  -------------------------------------------------------------------------------------------------------- */
bool Monitor::reload()
{
    /*    s.getFacts()->clear();
        for (auto it = s.getFacts()->begin(); it != s.getFacts()->end(); it++)
        {
            if ((*it).group == _sysgroup)
            {
                fact_t occurrenceTmp = {(*it).code, 0, (*it).mode};
                s.getFacts()->push_back(occurrenceTmp);
            }
        }
    */
    return true;
}
/*  -------------------------------------------------------------------------------------------------------- */
bool Monitor::resetStatistics(timedata_t now_)
{
    // Threading ???
    // zerotime = now_;
    for (auto it = s.getFacts()->begin(); it != s.getFacts()->end(); it++)
    {
        (*it).value = 0; // reset to Zero
    }
    return true;
}

/*  -------------------------------------------------------------------------------------------------------- */
syscode_t Monitor::addFact(syscode_t ncode_, statmode_t mode_)
{
    // Threading ???
    if (s.getFacts()->size() > 0)
    {
        auto it = s.getFacts()->begin();
        while (it != s.getFacts()->end())
        {
            if (((*it).code == ncode_) && ((*it).group == _sysgroup))
            {
                return ncode_;
            }
            it++;
        }
    }
    s.getFacts()->push_back(Fact(_sysgroup, ncode_, mode_)); // Create a new option
    return ncode_;
}

/*  -------------------------------------------------------------------------------------------------------- */
void Monitor::setValue(syscode_t code_, int data_)
{
    if (getFact(code_))
    {
        (*it).value = data_; // Update value
    }
}

/*  -------------------------------------------------------------------------------------------------------- */
void Monitor::incValue(syscode_t code_)
{
    if (getFact(code_))
    {
        (*it).value++; // increment by one
    }
}

/*  -------------------------------------------------------------------------------------------------------- */
void Monitor::avgValue(syscode_t code_, int data_)
{
    if (getFact(code_))
    {
        (*it).value = (((*it).value + data_) / 2); // Average
    }
}

/*  -------------------------------------------------------------------------------------------------------- */
void Monitor::sumValue(syscode_t code_, int data_)
{
    if (getFact(code_))
    {
        (*it).value += data_; // Sum !
    }
}