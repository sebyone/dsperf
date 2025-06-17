/*
 * DaaS-IoT 2023 (@) Sebyone Srl
 *
 * File: callback.h
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
/*  -------------------------------------------------------------------------------------------------------- */

#ifndef CALLBACK_H
#define CALLBACK_H

/*
    Model for emits/attends callback  :

*/

#include "system.h"

/*typedef enum : unsigned
{
    _CBP_LOW = 0,
    _CBP_NORMAL,
    _CBP_HIGH
} cbpriority_t;*/

/*  -------------------------------------------------------------------------------------------------------- */
class CallBack // Local access to features table
{
public:
    unsigned callBack(syscode_t event_, int parameter_, unsigned to_icaller_);
    // int retCallBack(syscode_t event_, int parameter_, unsigned int instance_);
    virtual void onCallBack(syscode_t event_, sysgroup_t from_sysgroup_, unsigned from_icaller_, int parameter_) = 0;

protected:
    CallBack();
    ~CallBack();

    unsigned regCallerBack(sysgroup_t sysgroup_, unsigned icaller_, unsigned priority_);
    syscode_t addCallBack(syscode_t event_, sysgroup_t to_sysgroup_); 

    unsigned _icaller = 0; // external ref for registered caller
private:
    Systemx &s = Systemx::Instance(); // singleton instance
};

#endif // H_CALLBACK
