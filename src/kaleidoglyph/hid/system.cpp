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

#include "kaleidoglyph/hid/system.h"

#include <kaleidoglyph/utils.h>
#include "DescriptorPrimitives.h"

namespace kaleidoglyph {
namespace hid {
namespace system {

static const PROGMEM byte hid_descriptor[] = {
  //TODO limit to system keys only?
  //  System Control (Power Down, Sleep, Wakeup, ...)
  D_USAGE_PAGE, D_PAGE_GENERIC_DESKTOP,        // USAGE_PAGE (Generic Desktop)
  D_USAGE, 0x80,                               // USAGE (System Control)
  D_COLLECTION, D_APPLICATION,                 // COLLECTION (Application)
  D_REPORT_ID, HID_REPORTID_SYSTEMCONTROL,     // REPORT_ID
  // 1 system key
  D_LOGICAL_MINIMUM, 0x00,                     // LOGICAL_MINIMUM (0)
  D_MULTIBYTE(D_LOGICAL_MAXIMUM), 0xff, 0x00,  // LOGICAL_MAXIMUM (255)
  D_USAGE_MINIMUM, 0x00,                       // USAGE_MINIMUM (Undefined)
  D_USAGE_MAXIMUM, 0xff,                       // USAGE_MAXIMUM (System Menu Down)
  D_REPORT_COUNT, 0x01,                        // REPORT_COUNT (1)
  D_REPORT_SIZE, 0x08,                         // REPORT_SIZE (8)
  D_INPUT, (D_DATA|D_ARRAY|D_ABSOLUTE),        // INPUT (Data,Ary,Abs)
  D_END_COLLECTION                             // END_COLLECTION
};

Dispatcher::Dispatcher() {
  static HIDSubDescriptor node(hid_descriptor, sizeof(hid_descriptor));
  HID().AppendDescriptor(&node);
}

void Dispatcher::init() {
  sendReport(0);
}

void Dispatcher::sendReport(byte keycode) {
  HID().SendReport(HID_REPORTID_SYSTEMCONTROL, &keycode, sizeof(keycode));
}

} //
} //
} //
