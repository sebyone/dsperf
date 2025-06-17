/*
 * DaaS-IoT 2023 (@) Sebyone Srl
 *
 * File: config.cpp
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "features.h"

#include "system.h"

Features::Features(sysgroup_t sysgroup_) : _sysgroup(sysgroup_)
{
    // reloadSettings();
    // Systemx *sys = new Systemx();
    // sys->RegMe(this, "Istanza 234");
    //  Features &s = Features::Instance(); // singleton instance

    // Systemx *sys;
    // sys->RegConfig(getSettings(), "Istanza 237");
}

Features::~Features()
{
    // if vector contains objects, not pointers to objects, the vector takes care of all the cleanup !!
    // for (auto it = settings.begin(); it != settings.end(); it++)
    //{
    // delete settings
    //}
}

/*  -------------------------------------------------------------------------------------------------------- */
bool Features::reload()
{
    _settings.clear();

    for (auto it = s.getOptions()->begin(); it != s.getOptions()->end(); it++)
    {
        if ((*it).group == _sysgroup)
        {
            _settings.push_back(setting_t{(*it).code, true, (*it).defValue});
        }
    }
    return true;
}

/*  -------------------------------------------------------------------------------------------------------- */
syscode_t Features::addFeature(syscode_t ncode_, int defval_)
{
    // Threading ???
    if (s.getOptions()->size() > 0)
    {
        auto it = s.getOptions()->begin();
        while (it != s.getOptions()->end())
        {
            if (((*it).code == ncode_) && ((*it).group == _sysgroup))
            {
                return ncode_;
            }
            it++;
        }
    }
    s.getOptions()->push_back(Option(_sysgroup, ncode_, defval_)); // Create a new option
    return ncode_;
}

bool Features::setOption(syscode_t code_, int value_)
{
    auto it = _settings.begin();
    while (it != _settings.end() && (*it).code != code_)
    {
        it++;
    }
    if ((*it).code == code_ && (*it).available)
    {
        (*it).value = value_;
        return true;
    }
    return false;
}

setting_t Features::getOption(syscode_t code_)
{
    auto it = _settings.begin();
    while (it != _settings.end() && (*it).code != code_)
    {
        it++;
    }
    if ((*it).code == code_)
    {
        return (*it);
    }
    return setting_t{___undefined, false, 0}; // DefaultVal
}

bool Features::setAllAvailable()
{
    for (auto it = _settings.begin(); it != _settings.end(); it++)
    {
        (*it).available = true; // enabled/disabled
    }
    return true;
}

bool Features::setAllUnavailable()
{
    for (auto it = _settings.begin(); it != _settings.end(); it++)
    {
        (*it).available = false; // enabled/disabled
    }
    return true;
}

bool Features::setAllDefault()
{
    for (auto it = _settings.begin(); it != _settings.end(); it++)
    {
        (*it).value = s.getOptions()->at((*it).code - 1).defValue; // enabled/disabled
    }
    return true;
}

bool Features::setAvailable(syscode_t code_)
{
    auto it = _settings.begin();
    while (it != _settings.end() && (*it).code != code_)
    {
        it++;
    }
    if ((*it).code == code_)
    {
        (*it).available = true;
        return true;
    }
    return 0; // DefaultVal
}

bool Features::setUnavailable(syscode_t code_)
{
    auto it = _settings.begin();
    while (it != _settings.end() && (*it).code != code_)
    {
        it++;
    }
    if ((*it).code == code_)
    {
        (*it).available = false;
        return true;
    }
    return false; // DefaultVal
}

options_t *Features::getSettings()
{
    return &_settings;
}
