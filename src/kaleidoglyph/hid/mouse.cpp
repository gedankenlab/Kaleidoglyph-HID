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

#include "kaleidoglyph/hid/mouse.h"

#include <kaleidoglyph/utils.h>
#include "DescriptorPrimitives.h"

namespace kaleidoglyph {
namespace hid {
namespace mouse {

static byte const mouse_descriptor[] PROGMEM = {
  /*  Mouse relative */
  D_USAGE_PAGE, D_PAGE_GENERIC_DESKTOP,           // USAGE_PAGE (Generic Desktop)
  D_USAGE, D_USAGE_MOUSE,                         //  USAGE (Mouse)
  D_COLLECTION, D_APPLICATION,                    //   COLLECTION (Application)
  D_REPORT_ID, HID_REPORTID_MOUSE,                //    REPORT_ID (Mouse)

  /* 8 Buttons */
  D_USAGE_PAGE, D_PAGE_BUTTON,                    //    USAGE_PAGE (Button)
  D_USAGE_MINIMUM, 0x01,                          //     USAGE_MINIMUM (Button 1)
  D_USAGE_MAXIMUM, 0x08,                          //     USAGE_MAXIMUM (Button 8)
  D_LOGICAL_MINIMUM, 0x00,                        //     LOGICAL_MINIMUM (0)
  D_LOGICAL_MAXIMUM, 0x01,                        //     LOGICAL_MAXIMUM (1)
  D_REPORT_COUNT, 0x08,                           //     REPORT_COUNT (8)
  D_REPORT_SIZE, 0x01,                            //     REPORT_SIZE (1)
  D_INPUT, (D_DATA|D_VARIABLE|D_ABSOLUTE),        //     INPUT (Data,Var,Abs)

  /* X, Y, Wheel */
  D_USAGE_PAGE, D_PAGE_GENERIC_DESKTOP,           //    USAGE_PAGE (Generic Desktop)
  D_USAGE, 0x30,                                  //     USAGE (X)
  D_USAGE, 0x31,                                  //     USAGE (Y)
  D_USAGE, 0x38,                                  //     USAGE (Wheel)
  D_LOGICAL_MINIMUM, 0x81,                        //     LOGICAL_MINIMUM (-127)
  D_LOGICAL_MAXIMUM, 0x7f,                        //     LOGICAL_MAXIMUM (127)
  D_REPORT_SIZE, 0x08,                            //     REPORT_SIZE (8)
  D_REPORT_COUNT, 0x03,                           //     REPORT_COUNT (3)
  D_INPUT, (D_DATA|D_VARIABLE|D_RELATIVE),        //     INPUT (Data,Var,Rel)

  /* Horizontal wheel */
  D_USAGE_PAGE, D_PAGE_CONSUMER,                  //    USAGE_PAGE (Consumer)
  D_PAGE_ORDINAL, 0x38, 0x02,                     //     PAGE (AC Pan)
  D_LOGICAL_MINIMUM, 0x81,                        //     LOGICAL_MINIMUM (-127)
  D_LOGICAL_MAXIMUM, 0x7f,                        //     LOGICAL_MAXIMUM (127)
  D_REPORT_SIZE, 0x08,                            //     REPORT_SIZE (8)
  D_REPORT_COUNT, 0x01,                           //     REPORT_COUNT (1)
  D_INPUT, (D_DATA|D_VARIABLE|D_RELATIVE),        //     INPUT (Data,Var,Rel)

  /* End */
  D_END_COLLECTION                                // END_COLLECTION
};


void Report::pressButtons(byte buttons) {
  buttons_ = buttons;
}

void Report::moveCursor(int8_t x_delta, int8_t y_delta) {
  x_delta_ = x_delta;
  y_delta_ = y_delta;
}

void Report::scrollVertical(int8_t v_delta) {
  v_delta_ = v_delta;
}

void Report::scrollHorizontal(int8_t h_delta) {
  h_delta_ = h_delta;
}

Dispatcher::Dispatcher() {
  static HIDSubDescriptor node(mouse_descriptor, sizeof(mouse_descriptor));
  HID().AppendDescriptor(&node);
}

void Dispatcher::init() {
  Report empty_report;
  sendReport(empty_report);
}

void Dispatcher::sendReport(const Report& report) {
  HID().SendReport(HID_REPORTID_MOUSE, &report, sizeof(report));
}

// ----------------------------------------------------------------------------
namespace absolute {

static byte const absolute_mouse_descriptor[] PROGMEM = {
  D_USAGE_PAGE, D_PAGE_GENERIC_DESKTOP,           // USAGE_PAGE (Generic Desktop)
  D_USAGE, D_USAGE_MOUSE,                         // USAGE (Mouse)
  D_COLLECTION, D_APPLICATION,                    // COLLECTION (Application)

  // 8 buttons
  D_USAGE_PAGE, D_PAGE_BUTTON,        //     USAGE_PAGE (Button)
  D_USAGE_MINIMUM, 0x01,              //     USAGE_MINIMUM (Button 1)
  D_USAGE_MAXIMUM, 0x08,              //     USAGE_MAXIMUM (Button 8)
  D_LOGICAL_MINIMUM, 0x00,            //     LOGICAL_MINIMUM (0)
  D_LOGICAL_MAXIMUM, 0x01,            //     LOGICAL_MAXIMUM (1)
  D_REPORT_COUNT, 0x08,               //     REPORT_COUNT (8)
  D_REPORT_SIZE, 0x01,                //     REPORT_SIZE (1)
  D_INPUT, (D_DATA|D_VARIABLE|D_ABSOLUTE),

  // x & y coordinates
  D_USAGE_PAGE, D_PAGE_GENERIC_DESKTOP,       // USAGE_PAGE (Generic Desktop)
  D_USAGE, 0x30,                      	      // USAGE (X)
  D_USAGE, 0x31,                              // USAGE (Y)
  D_MULTIBYTE(D_LOGICAL_MINIMUM), 0x00, 0x00, // Logical Minimum (0)
  D_MULTIBYTE(D_LOGICAL_MAXIMUM), 0xFF, 0x7f, // Logical Maximum (32767)
  D_REPORT_SIZE, 0x10,			      // Report Size (16),
  D_REPORT_COUNT, 0x02,		 	      // Report Count (2),
  D_INPUT, (D_DATA|D_VARIABLE|D_ABSOLUTE),    // Input (Data, Variable, Absolute)

  // scroll wheel
  D_USAGE, 0x38,                      	    //     USAGE (Wheel)
  D_LOGICAL_MINIMUM, 0x81,                  //     LOGICAL_MINIMUM (-127)
  D_LOGICAL_MAXIMUM, 0x7f,                  //     LOGICAL_MAXIMUM (127)
  D_REPORT_SIZE, 0x08,                      //     REPORT_SIZE (8)
  D_REPORT_COUNT, 0x01,                     //     REPORT_COUNT (1)
  D_INPUT, (D_DATA|D_VARIABLE|D_RELATIVE),

  D_END_COLLECTION                                // END_COLLECTION
};

void Report::pressButtons(byte buttons) {
  buttons_ = buttons;
}

void Report::moveCursorTo(uint16_t x, uint16_t y) {
  x_ = x;
  y_ = y;
}

Dispatcher::Dispatcher() : PluggableUSBModule(1, 1, epType) {}

// PluggableUSBModule method
int Dispatcher::getInterface(byte* interface_count) {
  *interface_count += 1; // uses 1
  HIDDescriptor hid_interface = {
    D_INTERFACE(pluggedInterface, 1,
                USB_DEVICE_CLASS_HUMAN_INTERFACE,
                HID_SUBCLASS_BOOT_INTERFACE,
                HID_PROTOCOL_KEYBOARD),
    D_HIDREPORT(sizeof(absolute_mouse_descriptor)),
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

  return USB_SendControl(TRANSFER_PGM,
                         absolute_mouse_descriptor,
                         sizeof(absolute_mouse_descriptor));
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
      // TODO Send8(protocol)
      return true;
    }
  }

  if (request_type == REQUEST_HOSTTODEVICE_CLASS_INTERFACE) {
    if (request == HID_SET_PROTOCOL) {
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
    if (request == HID_SET_REPORT) {}
  }

  return false;
}

void Dispatcher::init() {
  PluggableUSB().plug(this);
}

void Dispatcher::sendReport(Report const &report) {
  USB_Send(pluggedEndpoint | TRANSFER_RELEASE, &report, sizeof(report));
}

} // namespace absolute

} //
} //
} //
