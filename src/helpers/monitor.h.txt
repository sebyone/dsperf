/*
 * DaaS-IoT 2023 (@) Sebyone Srl
 *
 * File: logger.h
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
/*  -------------------------------------------------------------------------------------------------------- */

#ifndef MONITOR_H
#define MONITOR_H

/*
    Model for logging and statistics :

    parameter       parameter to monitor (channels, core, agent...)
    Statistics      Parameters grid
    Logger          resources logger

*/

#include <cstdint>
#include <string>
#include <list>
#include <vector>

#include "datetime.h"
#include "../locals.h"

// #include "system.h"
/*  -------------------------------------------------------------------------------------------------------- */
typedef unsigned syscode_t;
typedef unsigned sysgroup_t;

/*  -------------------------------------------------------------------------------------------------------- */
typedef enum : unsigned
{
    STAT_COUNTER = 0, // Counter
    STAT_EVERAGE,
    STAT_SUMMARY
} statmode_t;

/*  -------------------------------------------------------------------------------------------------------- */
class Fact // statistic stuff
{
public:
    Fact(sysgroup_t group_, syscode_t code_, unsigned mode_);
    ~Fact();
    sysgroup_t group;
    syscode_t code;
    std::string title;
    std::string comment;
    unsigned mode; // statmode_t
    double value;
};

typedef std::vector<Fact> facts_t;

/*  -------------------------------------------------------------------------------------------------------- */
class Statistics // Singleton
{
public:
    static Statistics &Instance()
    {
        static Statistics x;
        return x;
    }

    inline facts_t *getFacts(void) { return allfacts; }

protected:
private:
    Statistics() { allfacts = new facts_t(); };
    ~Statistics() = default;
    Statistics(const Statistics &) = delete;
    Statistics &operator=(const Statistics &) = delete;
    facts_t *allfacts; // threadsafe ????
    timedata_t zerotime;
};

/*  -------------------------------------------------------------------------------------------------------- */
class Monitor // Accessing
{
public:
    Monitor(sysgroup_t sysgroup_);
    ~Monitor();

    const sysgroup_t _sysgroup; // local system group

    syscode_t addFact(syscode_t code_, std::string title_); // to local sysgroup_
    syscode_t addFact(syscode_t code_, double value_);
    syscode_t addFact(syscode_t code_, std::string title_, double value_);
    syscode_t addFact(syscode_t code_, std::string title_, double value_, statmode_t mode_);
    syscode_t addFact(syscode_t code_, std::string title_, std::string comment_, double value_, statmode_t mode_);

    inline facts_t *getStatistics() { return s.getFacts(); } // Report

    void incValue(syscode_t code_);
    void setValue(syscode_t code_, int data_);
    void avgValue(syscode_t code_, int data_);
    void sumValue(syscode_t code_, int data_);

    bool resetStatistics(timedata_t now_);
    // bool reload();

private:
    Statistics &s = Statistics::Instance(); // singleton instance
    facts_t::iterator it = s.getFacts()->begin();
    Fact *getFact(syscode_t code_);
};

#endif // H_MONITOR
