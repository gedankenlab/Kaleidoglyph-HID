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

#include "kaleidoglyph/hid/keyboard.h"

#include <Arduino.h>

#include "DescriptorPrimitives.h"
#include "HID-Settings.h"
#include "kaleidoglyph/cKey.h"

namespace kaleidoglyph {
namespace hid {
namespace keyboard {

void Report::clear() {
  memset(&data_, 0, sizeof(data_));
}

// This method is of dubious value
bool Report::readKeycode(byte keycode) const {
  byte n = keycode / 8;
  byte i = keycode % 8;
  if (keycode < HID_KEYBOARD_FIRST_MODIFIER) {
    return bitRead(data_[1 + n], i);
  } else if (keycode <= HID_KEYBOARD_LAST_MODIFIER) {
    return bitRead(data_[0], i);
  }
  return false;
}

void Report::addKeycode(byte keycode) {
  byte n = keycode / 8;
  byte i = keycode % 8;
  if (keycode < HID_KEYBOARD_FIRST_MODIFIER) {
    bitSet(data_[1 + n], i);
  } else if (keycode <= HID_KEYBOARD_LAST_MODIFIER) {
    bitSet(data_[0], i);
  }
  for (byte i{0}; i < arraySize(data_); ++i) {
    Serial.print(".");
    if (data_[i] != 0) Serial.print(int(data_[i]));
  }
  Serial.println();
}

void Report::removeKeycode(byte keycode) {
  byte n = keycode / 8;
  byte i = keycode % 8;
  if (keycode < HID_KEYBOARD_FIRST_MODIFIER) {
    bitClear(data_[1 + n], i);
  } else if (keycode <= HID_KEYBOARD_LAST_MODIFIER) {
    bitClear(data_[0], i);
  }
}

// Update the report to remove any keycodes that don't appear in `new_report`. If any such
// keycodes are found, return `true`, otherwise return `false`. This efficiently deals
// with the problem of a plain keycode release in the same update as a new modifier
// press. We don't want to risk the modifier applying to the plain keycode before that
// keycode is released.
bool Report::updatePlainReleases_(const Report &new_report) {
  bool result{false};
  for (byte i{1}; i < arraySize(data_); ++i) {
    byte released_keycodes = data_[i] & ~(new_report.data_[i]);
    if (released_keycodes != 0) {
      data_[i] &= ~released_keycodes;
      result = true;
    }
  }
  return result;
}





static constexpr PROGMEM byte nkro_descriptor[] = {
  //  NKRO Keyboard
  D_USAGE_PAGE, D_PAGE_GENERIC_DESKTOP,
  D_USAGE, D_USAGE_KEYBOARD,
  D_COLLECTION, D_APPLICATION,
  D_REPORT_ID, HID_REPORTID_NKRO_KEYBOARD,
  D_USAGE_PAGE, D_PAGE_KEYBOARD,


  /* Key modifier byte */
  D_USAGE_MINIMUM, HID_KEYBOARD_FIRST_MODIFIER,
  D_USAGE_MAXIMUM, HID_KEYBOARD_LAST_MODIFIER,
  D_LOGICAL_MINIMUM, 0x00,
  D_LOGICAL_MAXIMUM, 0x01,
  D_REPORT_SIZE, 0x01,
  D_REPORT_COUNT, 0x08,
  D_INPUT, (D_DATA|D_VARIABLE|D_ABSOLUTE),


	/* 5 LEDs for num lock etc, 3 left for advanced, custom usage */
  D_USAGE_PAGE, D_PAGE_LEDS,
  D_USAGE_MINIMUM, 0x01,
  D_USAGE_MAXIMUM, 0x08,
  D_REPORT_COUNT, 0x08,
  D_REPORT_SIZE, 0x01,
  D_OUTPUT, (D_DATA | D_VARIABLE | D_ABSOLUTE),

  /* NKRO Keyboard */
  D_USAGE_PAGE, D_PAGE_KEYBOARD,

  // Padding 4 bits, to skip NO_EVENT & 3 error states.
  D_REPORT_SIZE, 0x04,
  D_REPORT_COUNT, 0x01,
  D_INPUT, (D_CONSTANT),

  D_USAGE_MINIMUM, HID_KEYBOARD_A_AND_A,
  D_USAGE_MAXIMUM, HID_LAST_KEY,
  D_LOGICAL_MINIMUM, 0x00,
  D_LOGICAL_MAXIMUM, 0x01,
  D_REPORT_SIZE, 0x01,
  D_REPORT_COUNT, (HID_LAST_KEY - HID_KEYBOARD_A_AND_A),
  D_INPUT, (D_DATA|D_VARIABLE|D_ABSOLUTE),

  // Padding (3 bits) to round up the report to byte boundary.
  D_REPORT_SIZE, 0x03,
  D_REPORT_COUNT, 0x01,
  D_INPUT, (D_CONSTANT),

  D_END_COLLECTION,

};

// See Appendix B of USB HID spec
static constexpr PROGMEM byte boot_descriptor[] = {
  //  Keyboard
  D_USAGE_PAGE, D_PAGE_GENERIC_DESKTOP,
  D_USAGE, D_USAGE_KEYBOARD,

  D_COLLECTION, D_APPLICATION,
  // Modifiers
  D_USAGE_PAGE, D_PAGE_KEYBOARD,
  D_USAGE_MINIMUM, 0xe0,
  D_USAGE_MAXIMUM, 0xe7,
  D_LOGICAL_MINIMUM, 0x0,
  D_LOGICAL_MAXIMUM, 0x1,
  D_REPORT_SIZE, 0x1,
  D_REPORT_COUNT, 0x8,
  D_INPUT, (D_DATA|D_VARIABLE|D_ABSOLUTE),

  // Reserved byte
  D_REPORT_COUNT, 0x1,
  D_REPORT_SIZE, 0x8,
  D_INPUT, (D_CONSTANT),

  // LEDs
  D_REPORT_COUNT, 0x5,
  D_REPORT_SIZE, 0x1,
  D_USAGE_PAGE, D_PAGE_LEDS,
  D_USAGE_MINIMUM, 0x1,
  D_USAGE_MAXIMUM, 0x5,
  D_OUTPUT, (D_DATA|D_VARIABLE|D_ABSOLUTE),
  // Pad LEDs up to a byte
  D_REPORT_COUNT, 0x1,
  D_REPORT_SIZE, 0x3,
  D_OUTPUT, (D_CONSTANT),

  // Non-modifiers
  D_REPORT_COUNT, 0x6,
  D_REPORT_SIZE, 0x8,
  D_LOGICAL_MINIMUM, 0x0,
  D_LOGICAL_MAXIMUM, 0xff,
  D_USAGE_PAGE, D_PAGE_KEYBOARD,
  D_USAGE_MINIMUM, 0x0,
  D_USAGE_MAXIMUM, 0xff,
  D_INPUT, (D_DATA|D_ARRAY|D_ABSOLUTE),
  D_END_COLLECTION
};

Dispatcher::Dispatcher() : PluggableUSBModule(1, 1, epType) {
  static HIDSubDescriptor node(nkro_descriptor, sizeof(nkro_descriptor));
  HID().AppendDescriptor(&node);
}

// PluggableUSBModule method
int Dispatcher::getInterface(byte* interface_count) {
  *interface_count += 1; // uses 1
  HIDDescriptor hid_interface = {
    D_INTERFACE(pluggedInterface, 1,
                USB_DEVICE_CLASS_HUMAN_INTERFACE,
                HID_SUBCLASS_BOOT_INTERFACE,
                HID_PROTOCOL_KEYBOARD),
    D_HIDREPORT(sizeof(boot_descriptor)),
    D_ENDPOINT(USB_ENDPOINT_IN(pluggedEndpoint),
               USB_ENDPOINT_TYPE_INTERRUPT, USB_EP_SIZE, 0x01)
  };
  return USB_SendControl(0, &hid_interface, sizeof(hid_interface));
}

// PluggableUSBModule method
int Dispatcher::getDescriptor(USBSetup& setup) {
  // Check if this is a HID Class Descriptor request
  if (setup.bmRequestType != REQUEST_DEVICETOHOST_STANDARD_INTERFACE) {
    return 0;
  }
  if (setup.wValueH != HID_REPORT_DESCRIPTOR_TYPE) {
    return 0;
  }

  // In a HID Class Descriptor wIndex cointains the interface number
  if (setup.wIndex != pluggedInterface) {
    return 0;
  }

  // Reset the protocol on reenumeration. Normally the host should not assume the state of
  // the protocol due to the USB specs, but Windows and Linux just assumes its in report
  // mode.
  hid_protocol_ = nkro_mode;

  return USB_SendControl(TRANSFER_PGM, boot_descriptor, sizeof(boot_descriptor));
}

// PluggableUSBModule method
bool Dispatcher::setup(USBSetup& setup) {
  if (pluggedInterface != setup.wIndex) {
    return false;
  }

  byte request = setup.bRequest;
  byte request_type = setup.bmRequestType;

  if (request_type == REQUEST_DEVICETOHOST_CLASS_INTERFACE) {
    if (request == HID_GET_REPORT) {
      // TODO: HID_GetReport();
      return true;
    }
    if (request == HID_GET_PROTOCOL) {
      // TODO improve
      UEDATX = hid_protocol_;
      return true;
    }
    if (request == HID_GET_IDLE) {
      // TODO improve
      UEDATX = idle;
      return true;
    }
  }

  if (request_type == REQUEST_HOSTTODEVICE_CLASS_INTERFACE) {
    if (request == HID_SET_PROTOCOL) {
      hid_protocol_ = setup.wValueL;
      return true;
    }
    if (request == HID_SET_IDLE) {
      // We currently ignore SET_IDLE, because we don't really do anything with it, and implementing
      // it causes issues on OSX, such as key chatter. Other operating systems do not suffer if we
      // force this to zero, either.
#if 0
      idle = setup.wValueL;
#else
      idle = 0;
#endif
      return true;
    }
    if (request == HID_SET_REPORT) {
      // Check if data has the correct length afterwards
      int length = setup.wLength;

      if (setup.wValueH == HID_REPORT_TYPE_OUTPUT) {
        if (length == sizeof(leds)) {
          USB_RecvControl(&leds, length);
          return true;
        }
      }

      // Input (set HID report)
      else if (setup.wValueH == HID_REPORT_TYPE_INPUT) {
        if (length == sizeof(boot_report_)) {
          USB_RecvControl(&boot_report_, length);
          return true;
        }
      }
    }
  }

  return false;
}

void Dispatcher::init() {
  last_report_.clear();
  sendReportUnchecked_(last_report_);
}

// This is the primary function of the Dispatcher: to send new HID reports such that they
// won't cause unintended output on the host.
void Dispatcher::sendReport(const Report &new_report) {

  // First, we determine if any modifiers have changed state
  const byte new_modifiers = new_report.getModifiers();
  const byte old_modifiers = last_report_.getModifiers();

  // If any modifiers were released, we need to first send a report with any plain
  // keycodes removed to prevent getting any unintended output when keys are held long
  // enough to repeat. For example, if we have a `shift` + `C` key, holding it might
  // sometimes produce: `CCCCCc`.
  const byte released_modifiers = old_modifiers & ~new_modifiers;
  if ((released_modifiers != 0) &&
      last_report_.updatePlainReleases_(new_report)) {
    sendReportUnchecked_(last_report_);
  }

  // Next, if any modifiers were added since the previous report, we need to send those
  // first to ensure that the modifiers will be applied before any keycodes that were
  // added in the same report. Also, if a modifier release comes at the same time as a new
  // keycode press (Unshifter does this), we likewise need to send the modifier changes in
  // a separate report first. In short, modifier changes must come after key _releases_,
  // but before key _presses_.
  const byte changed_modifiers  = old_modifiers ^ new_modifiers;
  if (changed_modifiers != 0) {
    last_report_.setModifiers(new_modifiers);
    sendReportUnchecked_(last_report_);
  }

  // Last, we send a report with any keycodes that were added in the new report, if
  // any. We also update the stored previous report, if necessary.
  if (new_report != last_report_) {
    last_report_.updateFrom_(new_report);
    sendReportUnchecked_(last_report_);
  }
}

// I'm not at all convinced that it's worthwhile to check the return value, and
// the report-sending functions become more efficient if we just return void
// instead, but for the moment, pass it through.
int Dispatcher::sendReportUnchecked_(const Report &report) {
  if (hid_protocol_ == boot_mode) {
    report.translateToBootProtocol_(boot_report_);
    return USB_Send(pluggedEndpoint | TRANSFER_RELEASE,
                    &boot_report_, sizeof(boot_report_));
  }
  return HID().SendReport(HID_REPORTID_NKRO_KEYBOARD,
                          &report, sizeof(report));
}

void Report::translateToBootProtocol_(byte (&boot_report)[8]) const {
  // modifiers
  memset(boot_report, 0, sizeof(boot_report));
  byte boot_report_index{2};

  boot_report[0] = getModifiers();
  for (byte i{0}; i < arraySize(data_); ++i) {
    if (data_[i] == 0) continue;
    for (byte n{0}; n < 8; ++n) {
      if (bitRead(data_[i], n)) {
        byte keycode = ((i - 1) * 8) + n;
        boot_report[boot_report_index++] = keycode;
        if (boot_report_index == sizeof(boot_report))
          return;
      }
    }
  }
}

} // namespace keyboard {
} // namespace hid {
} // namespace kaleidoscope {
