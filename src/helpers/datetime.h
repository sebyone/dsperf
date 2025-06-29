/*
 * DaaS-IoT 2023 (@) Sebyone Srl
 *
 * File: channel.h
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef DATETIME_H
#define DATETIME_H

typedef long timedata_t;

// -------------------------------------------------------------------------------------------------------- !

#ifdef __cplusplus
extern "C"
{
#endif

    double now_millis();
    double now_micros();
    double now_seconds();
  //  double now_sec();

#ifdef __cplusplus
}
#endif

#endif // DATETIME_H
