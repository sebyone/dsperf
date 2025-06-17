/*
 * DaaS-IoT 2023 (@) Sebyone Srl
 *
 * File: features.h
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef FEATURES_H
#define FEATURES_H

#include <cstdint>
#include <string>
#include <list>
#include <vector>
#include <memory>

#include "../locals.h"

#include "system.h"

/*  -------------------------------------------------------------------------------------------------------- */
typedef struct /* Features Settings */
{
  syscode_t code;
  bool available;
  int value;
} setting_t;

typedef std::vector<setting_t> options_t;

class Features // Local access to features table
{
public:
  Features(sysgroup_t sysgroup_);
  ~Features();

  syscode_t addFeature(syscode_t ncode_, int defval_); // System/Module
  bool reload();

  options_t *getSettings(); // local settings

  setting_t getOption(syscode_t code_);
  bool setOption(syscode_t code_, int value_);
  bool setAllAvailable();
  bool setAllUnavailable();
  bool setAllDefault(); // set default value for all local features
  bool setAvailable(syscode_t code_);
  bool setUnavailable(syscode_t code_);

private:
  Systemx &s = Systemx::Instance(); // singleton instance

  const sysgroup_t _sysgroup; // local features/system group
  options_t _settings;       // local settings
};

#endif // FEATURES_H
