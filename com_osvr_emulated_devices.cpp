/** @date 2015

@author
Sensics, Inc.
<http://sensics.com/osvr>
*/

// Copyright 2015 Sensics Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/AnalogInterfaceC.h>

// Generated JSON header file
#include "com_osvr_emulated_devices.h"
#include <hidapi.h>
#include <vendor/HIDAPIpp/hidapipp.h>

// Standard includes
#include <iostream>
#include <memory>
#include <ctime>
#include <windows.h>

/// @todo read VID/PID from device descriptor or separate file
#define VID 0x03eb
#define PID 0x2421

// Anonymous namespace to avoid symbol collision
namespace {

// typedef std::shared_ptr<OpenSpatialServiceController> ControllerPtr;

OSVR_MessageType gestureMessage;

class EmulatedDevices {
  public:
    EmulatedDevices(OSVR_PluginRegContext ctx, hidapi::SharedDevice *dev)
        : m_hidDev(dev)

    {
        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        osvrDeviceAnalogConfigure(opts, &m_analog, 3);

        /// Create the sync device token with the options
        m_dev.initSync(ctx, "dev", opts);

        /// Send JSON descriptor
        m_dev.sendJsonDescriptor(com_osvr_emulated_devices_json);

        /// Register update callback
        m_dev.registerUpdateCallback(this);
    }

    OSVR_ReturnCode update() {

        auto result = m_hidDev->read();
        if (hidapi::had_error(result)) {
            std::cerr << "Error reading response from device" << std::endl;
        } else {
            auto const &data = hidapi::get_data(result);
            OSVR_AnalogState vals[3];
            vals[0] = (WORD(data[1]) << 8) + data[0];
            vals[1] = (WORD(data[3]) << 8) + data[2];
            vals[2] = (WORD(data[9]) << 8) + data[8];
            osvrDeviceAnalogSetValues(m_dev, m_analog, vals, 3);
        }
        return OSVR_RETURN_SUCCESS;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_AnalogDeviceInterface m_analog;
    hidapi::SharedDevice *m_hidDev;
};

class HardwareDetection {
  public:
    HardwareDetection() : m_found(false) {}

    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {

        std::cout << "PLUGIN: Got a hardware detection request" << std::endl;

        // if the device has been discovered we don't need to go thru hardware
        // detection again
        if (!m_found) {

            auto dev_path = std::string{};
            for (auto cur_dev : hidapi::Enumeration()) {
                if (cur_dev->vendor_id == VID && cur_dev->product_id == PID) {
                    dev_path.assign(cur_dev->path);
                }
            }
            if (dev_path.empty()) {
                std::cout << "PLUGIN: Device not detected" << std::endl;
                return OSVR_RETURN_FAILURE;
            }

            dev = hidapi::SharedDevice{dev_path};
            /// Enable blocking mode on this device
            hid_set_nonblocking(dev.get(), 0);

            std::cout << "PLUGIN: We have detected emulated device! "
                      << std::endl;

            m_found = true;
            osvr::pluginkit::registerObjectForDeletion(
                ctx, new EmulatedDevices(ctx, &dev));
        }

        return OSVR_RETURN_SUCCESS;
    }

    bool m_found;
    hidapi::Library lib;
    hidapi::SharedDevice dev;
};
} // namespace

OSVR_PLUGIN(com_osvr_emulated_devices) {

    osvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerHardwareDetectCallback(new HardwareDetection());

    return OSVR_RETURN_SUCCESS;
}