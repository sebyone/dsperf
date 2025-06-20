/*
 * dsperf 2024, 2025 (@) Sebyone Srl
 *
 * File: locals.h
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

#ifndef DSPERF_LOCALS_H
#define DSPERF_LOCALS_H

#pragma once

// Setting --------------------------------------------------

#undef WITH_DAAS
#undef DEBUG
#define VERBOSE

//
#define MIN_BLOCK_SIZE 1
#define MAX_BLOCK_SIZE (1024 * 1024 * 1024) // 1GB
#define MIN_PACKET_SIZE 1
#define MAX_PACKET_SIZE (10 * 1024 * 1024) // 10MB

// ---------------------------------------------- END SETTINGS

#define noexec void(0)

#ifdef VERBOSE
#define pverbose printf
#else
#define pverbose noexec
#endif

#ifdef DEBUG
#define pdebug printf
#else
#define pdebug noexec
#endif

typedef enum
{
    rtOk = 0,   // No error
    rtExit = 1, // stdlib EXIT_FAILURE 1
    rtErr = 2   // Generic error
} ret_t;

#if defined(__linux__) || defined(__RASP__)
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#endif

#include <ctype.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#endif // DSPERF_LOCALS_H