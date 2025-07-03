/*
 * dsperf 2024, 2025 (@) Sebyone Srl
 *
 * File: utils.h
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

#ifndef UTILS_H
#define UTILS_H
//
#define _1KB 2 ^ 10
#define _1MB 2 ^ 20
#define _1GB 2 ^ 30
#define _Byte2Megabyte(b) ((double)(b) / (2 ^ 20))
#define _Byte2Megabits(b) ((double)(b * 8) / (double)(2 ^ 20))
#define _MegaByte2Megabits(b) ((long)(b * 8))

// Bitwise
//
#define _BV(n) (1 << (n))
#define _LSB(w) ((uint8_t)((w) & 0xff))
#define _MSB(w) ((uint8_t)((w) >> 8U))
#define lowByte(w) ((uint8_t)((w) & 0xff))
#define highByte(w) ((uint8_t)((w) >> 8))
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#define pinToggle(pin) PINB ^= 1UL _BV()


#endif // UTILS_H