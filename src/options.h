/*
 * dsperf 2024, 2025 (@) Sebyone Srl
 *
 * File: options.h
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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "globals.h"

#pragma once

void print_credits();

ret_t parse_args(int argc, char *argv[]);

ret_t validate_model_options(exe_t &_pfrun);

#endif // OPTIONS_H
