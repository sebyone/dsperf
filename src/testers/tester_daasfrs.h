/*
 * dsperf 2024, 2025 (@) Sebyone Srl
 *
 * File: model_daasperf.h
 *
 * Licence
 * This Source Code Form  is subject to the terms of the Mozilla Public License, v. 2.0.
 * You can obtain a copy of the MPL License at https://mozilla.org/MPL/2.0/.
 * Disclaimer of Warrant
 * Covered Software is provided under this License on an "as is" basis, without warranty
 * of any kind.The entire risk as to the quality and performance of the Covered Software
 * is  with You.  This  disclaimer  of  warranty  constitutes  an essential part of this 
 * License. No use of any Covered Software is authorized under this License except under 
 * this disclaimer.
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
#ifdef  LIBDAAS_V19

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

#include "../helpers/utils.h"
#include "../model.h"

ret_t set_env_daasfrs(options_t &ops_);  // set default Options values for model

// ret_t get_hwif_ipv4tcp(netif_t &_nif); // returns interface information

// ret_t set_env_ipv4tcp(netif_t &nif_); // updates interface paraeters

ret_t run_server_daasfrs(tclass_t tcls); // start loopback server

ret_t run_client_daasfrs(tclass_t tcls); // performs test

#endif // WITH_DAAS
#endif // MODEL_DAASFRS_H