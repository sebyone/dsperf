/*
 * DaaS-IoT 2023 (@) Sebyone Srl
 *
 * File: config.cpp
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include "monitor.h"
#include "system.h"
// #include "datetime.h"

Monitor::Monitor(sysgroup_t sysgroup_) : _sysgroup(sysgroup_)
{
    // occurrencies = new std::vector<occurrence_t>();

   // Systemx &x = Systemx::Instance(); // singleton instance
   // x.getNewModuleSerials( this,sysgroup_); //  "Istanza 236");
}

Monitor::~Monitor()
{
}
/*  -------------------------------------------------------------------------------------------------------- */
bool Monitor::reload()
{
    _statistics.clear();
    for (auto it = s.getFacts()->begin(); it != s.getFacts()->end(); it++)
    {
        if ((*it).group == _sysgroup)
        {
            occurrence_t occurrenceTmp = {(*it).code, 0, (*it).mode};
            _statistics.push_back(occurrenceTmp);
        }
    }
    return true;
}
/*  -------------------------------------------------------------------------------------------------------- */
bool Monitor::resetStatistics(timedata_t now_)
{
    // Threading ???
    zerotime = now_;
    for (auto it = _statistics.begin(); it != _statistics.end(); it++)
    {
        (*it).value = 0; // reset to Zero
    }
    return true;
}

/*  -------------------------------------------------------------------------------------------------------- */
syscode_t Monitor::addStatisticFact(syscode_t ncode_, statmode_t mode_)
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
std::vector<occurrence_t> *Monitor::getStatistics() // Report
{
    return &_statistics;
}

/*  -------------------------------------------------------------------------------------------------------- */
void Monitor::setFact(syscode_t code_, int data_)
{
    auto it = _statistics.begin();
    while (it != _statistics.end() && (*it).code != code_)
    {
        it++;
    }
    if ((*it).code == code_)
    {
        (*it).value = data_; // Update value
    }
}

/*  -------------------------------------------------------------------------------------------------------- */
void Monitor::incFact(syscode_t code_)
{
    this->setFact(code_, 1); // increment by one
}

/*  -------------------------------------------------------------------------------------------------------- */
void Monitor::avgFact(syscode_t code_, int data_)
{
    auto it = _statistics.begin();
    while (it != _statistics.end() && (*it).code != code_)
    {
        it++;
    }
    if ((*it).code == code_)
    {
        (*it).value = (int)(((*it).value + data_) / 2); // Average truncate !
    }
}

/*  -------------------------------------------------------------------------------------------------------- */
void Monitor::sumFact(syscode_t code_, int data_)
{
    auto it = _statistics.begin();
    while (it != _statistics.end() && (*it).code != code_)
    {
        it++;
    }
    if ((*it).code == code_)
    {
        (*it).value = (*it).value + data_; // Sum !
    }
}