/*
 * dsperf 2024, 2025 (@) Sebyone Srl
 *
 * File: model_daasperf.h
 *
 * Licence
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * Disclaimer of Warrant
 * Covered Software is provided under this License on an "as is" basis, without warranty of any kind.
 * The entire risk as to the quality and performance of the Covered Software is with You.
 * This disclaimer of warranty constitutes an essential part of this License.
 * No use of any Covered Software is authorized under this License except under this disclaimer.
 *
 * Contributors:
 * plogiacco@smartlab.it - initial design, implementation and documentation
 * sebastiano.meduri@gmail.com  - initial design, implementation and documentation
 *
 */

// DaaS Protocol
//
// Daas Fresbee ( refs LibDaas API docs )
// -----------------------------------------------------
// .
// ...

#ifndef MODEL_DAASFRS_H
#define MODEL_DAASFRS_H
#ifdef WITH_DAAS

#define TEST_DAASFRS 2                   // DaaS Fresbee loopback
#define TEST_DAASFRS_CLASS TEST_CAPACITY // Capabilities, Availability, Capacity, Security

#define TEST_MODEL_NAME "DaaS/Fresbee"
#define TEST_MODEL_INFO "Throughput/Bandwidth v.01a"
#define TEST_MODEL_LINK "developers@sebyone.it"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// DaaS node stack - libdaas
#include "daas.hpp"
#include "daas_types.hpp"

#include "../locals.h"

ret_t get_daasfrs_info(); // return test model information (string)

ret_t check_daasfrs();

ret_t run_daasfrs_client(daas_setup_t *setup, options_t *test);

ret_t run_daasfrs_server(daas_setup_t *setup);

#endif // WITH_DAAS
#endif // MODEL_DAASFRS_H