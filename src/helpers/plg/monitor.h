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

#include "system.h"

/*  -------------------------------------------------------------------------------------------------------- */
typedef enum : unsigned // daasiot.h
{
    STAT_COUNTER = 0, // Supported update mode
    STAT_EVERAGE,
    STAT_SUMMARY
} statmode_t;

/*  -------------------------------------------------------------------------------------------------------- */
typedef struct /* Fact stuff */
{
    unsigned code;  // syscode_t
    double value;
    unsigned mode;  // statmode_t
} occurrence_t;

typedef std::vector<occurrence_t> statistics_t; // std::list

class Monitor   // Local access to features table
{
public:
    Monitor(sysgroup_t sysgroup_);
    ~Monitor();

    syscode_t addStatisticFact(syscode_t ncode_, statmode_t mode_); // to local sysgroup_
    bool reload();
    statistics_t *getStatistics(); // Report

    bool resetStatistics(timedata_t now_);
    void setFact(syscode_t code_, int data_);
    void incFact(syscode_t code_);
    void avgFact(syscode_t code_, int data_);
    void sumFact(syscode_t code_, int data_);
    // bool logFact(unsigned nfld, int value);

    const sysgroup_t _sysgroup; // local system group

private:
    Systemx &s = Systemx::Instance(); // singleton instance
    // Statistics *s;

    statistics_t _statistics; // local facts
    timedata_t zerotime;
};

#endif // H_MONITOR

/*
class Statistics // Singleton
{
public:
    static Statistics *Instance() // volatile = threadsafe ????
    {
        static Statistics x;
        return &x;
    }
    std::vector<Fact> *getFacts(void) { return allfacts; }

protected:
private:
    Statistics() { allfacts = new std::vector<Fact>(); };
    ~Statistics() = default;
    Statistics(const Statistics &) = delete;
    Statistics &operator=(const Statistics &) = delete;
    std::vector<Fact> *allfacts; // volatile = threadsafe ????
};
*/