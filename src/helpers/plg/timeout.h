/**
 * DaaS-IoT 2023 (@) Sebyone Srl
 *
 * File: timeout.h
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef TIMEOUT_H__
#define TIMEOUT_H__

#include <chrono>

class Timeout {
public:
    Timeout(){ }
    ~Timeout(){ }

    void start(unsigned int duration_ms_){
        start_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        duration_ms = duration_ms_;
    }

    bool timeouted(){
        unsigned int now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        return duration_ms < (now - start_ms);
    }

private:
    unsigned int start_ms;
    unsigned int duration_ms;

};


#endif