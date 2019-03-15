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

#include "kaleidoglyph/hid/consumer.h"

#include "DescriptorPrimitives.h"

namespace kaleidoglyph {
namespace hid {
namespace consumer {

static const PROGMEM byte consumer_control_descriptor[] = {
  // Consumer Control (Sound/Media keys)
  D_USAGE_PAGE, 0x0C,                          // usage page (consumer device) 
  D_USAGE, 0x01,                               // usage -- consumer control 
  D_COLLECTION, D_APPLICATION,                 // collection (application) 
  D_REPORT_ID, HID_REPORTID_CONSUMERCONTROL,   // report id 
  // 4 Media Keys 
  D_LOGICAL_MINIMUM, 0x00,                     // logical minimum 
  D_MULTIBYTE(D_LOGICAL_MAXIMUM), 0xFF, 0x03,  // logical maximum (3ff) 
  D_USAGE_MINIMUM, 0x00,                       // usage minimum (0) 
  D_MULTIBYTE(D_USAGE_MAXIMUM), 0xFF, 0x03,    // usage maximum (3ff) 
  D_REPORT_COUNT, 0x04,                        // report count (4) 
  D_REPORT_SIZE, 0x10,                         // report size (16) 
  D_INPUT, 0x00,                               // input 
  D_END_COLLECTION                             // end collection 
};

Report::clear() {
  memset(keycodes_, 0, sizeof(keycodes_));
}

void Report::addKeycode(uint16_t keycode) {
  for (byte i{0}; i < arraySize(keycodes_); ++i) {
    if (keycodes_[i] == 0) {
      keycodes_[i] = keycode;
      break;
    }
  }
}

void Report::releaseKeycode(uint16_t keycode) {
  for (byte i{0}; i < arraySize(keycodes_); ++i) {
    if (keycodes_[i] == keycode) {
      keycodes_[i] == 0;
    }
  }
}

bool Report::operator==(const Report& other) const {
  return (memcmp(keycodes_, other.keycodes_, sizeof(keycodes_)) == 0);
}

void Report::updateFrom(const Report& new_report) {
  memcpy(keycodes_, new_report.keycodes_, sizeof(keycodes_));
}

Dispatcher::Dispatcher() {
  static HIDSubDescriptor node(consumer_control_descriptor,
                               sizeof(consumer_control_descriptor));
  HID().AppendDescriptor(&node);
}

void Dispatcher::init() {
  last_report_.clear();
  sendReport(last_report_);
}

void Dispatcher::sendReportUnchecked_() {
  HID().SendReport(HID_REPORTID_CONSUMERCONTROL,
                   last_report_.keycodes_, sizeof(last_report_.keycodes_));
}

void Dispatcher::sendReport(const Report& report) {
  // If the last report is different than the current report, then we need to send a
  // report. We guard sendReport like this so that calling code doesn't end up spamming
  // the host with empty reports if sendReport is called in a tight loop.

  // if the previous report is the same, return early without a new report.
  if (report == last_report_)
    return;

  sendReportUnchecked_(report);
  last_report_.updateFrom(report);
}

} //
} //
} //
