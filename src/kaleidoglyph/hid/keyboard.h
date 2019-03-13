// -*- c++ -*-

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

#include "kaleidoglyph/Key.h"
#include "kaleidoglyph/utils.h"
#include "HIDAliases.h"

namespace kaleidoglyph {
namespace hid {
namespace keyboard {

class Report {

  friend class Dispatcher;

 public:
  Report() {}

  void clear();

  bool readKeycode(byte keycode) const;
  void addKeycode(byte keycode);
  void removeKeycode(byte keycode);

  byte getModifiers() const { return data_[0]; }
  void setModifiers(byte modifiers) { data_[0] = modifiers; }
  void addModifiers(byte modifiers) { data_[0] |= modifiers; }
  void removeModifiers(byte modifiers) { data_[0] &= ~modifiers; }

  bool operator==(const Report& other) const {
    return (memcmp(data_, other.data_, sizeof(data_)) == 0);
  }
  bool operator!=(const Report& other) const {
    return !(*this == other);
  }

 private:
  static constexpr byte keycode_bytes = bitfieldSize(HID_KEYBOARD_FIRST_MODIFIER);

  // It's important to make this a single array, rather than a struct with a separate
  // modifiers byte and keycodes array, because we're going to send this report data
  // directly to the HID().sendReport() function, and this is the only way to guarantee
  // that there won't be any padding bytes between the two members.
  byte data_[1 + keycode_bytes] = {};

  bool updatePlainReleases_(const Report& new_report);

  void updateFrom_(const Report& other) {
    memcpy(data_, other.data_, sizeof(data_));
  }
  void translateToBootProtocol_(byte (&boot_report)[8]) const;

};

class Dispatcher : PluggableUSBModule {

 public:
  Dispatcher();
  void init();
  byte getLedState() const {
    return HID().getLEDs();
  }
  byte lastModifierState() const {
    return last_report_.getModifiers();
  }
  void sendReport(const Report &report);

  // should be enum class
  static constexpr byte boot_mode = HID_BOOT_PROTOCOL;
  static constexpr byte nkro_mode = HID_REPORT_PROTOCOL;

  bool getProtocol() const {
    return hid_protocol_;
  }
  void setProtocol(bool mode) {
    hid_protocol_ = mode;
  }

 private:
  Report last_report_;

  byte hid_protocol_{nkro_mode};
  byte boot_report_[8];

  int sendReportUnchecked_(const Report &report);

 protected:
  // PluggableUSBModule
  int getInterface(byte* interface_count);
  int getDescriptor(USBSetup& setup);
  bool setup(USBSetup& setup);

  byte epType[1] = {EP_TYPE_INTERRUPT_IN};
  byte idle{1};
  byte leds{0};

};

} // namespace keyboard {
} // namespace hid {
} // namespace kaleidoscope {
