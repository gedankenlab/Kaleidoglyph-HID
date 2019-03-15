// -*- mode: c++ -*-

/*
Copyright (c) 2014-2015 NicoHood
Copyright (c) 2015-2018 Keyboard.io, Inc
Copyright (c) 2019 Michael Richters

See the readme for credit to other people.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once

#include <Arduino.h>
#include <PluggableUSB.h>
#include <HID.h>
#include "HID-Settings.h"

#include <kaleidoglyph/Key.h>
#include <kaleidoglyph/utils.h>

namespace kaleidoglyph {
namespace hid {
namespace consumer {

class Report {

  friend class Dispatcher;

 public:
  void clear();
  void addKeycode(uint16_t keycode);
  void releaseKeycode(uint16_t keycode);
  bool operator==(const Report& other) const;
  void updateFrom(const Report& new_report);

 private:
  uint16_t keycodes_[4];

};

class Dispatcher {

 public:
  Dispatcher();
  void init();
  void sendReportUnchecked_(const Report& report);
  void sendReport(const Report& report);

 private:
  Report last_report_;

};

} //
} //
} //
