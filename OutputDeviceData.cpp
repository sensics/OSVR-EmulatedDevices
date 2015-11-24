/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
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
#include <osvr/ClientKit/Context.h>
#include <osvr/ClientKit/Interface.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

void AnalogCallback(void * /*userdata*/, const OSVR_TimeValue * /*timestamp*/,
                    const OSVR_AnalogReport *report) {
    std::cout << "Device report: Sensor: " << report->sensor << " Data: "
              << report->state << std::endl;
}

void AnalogCallback2(void * /*userdata*/, const OSVR_TimeValue * /*timestamp*/,
                     const OSVR_AnalogReport *report) {
    std::cout << "Device report: Sensor: " << report->sensor << " Data: "
              << report->state << std::endl;
}

int main() {
    osvr::clientkit::ClientContext context(
        "com.osvr.exampleclients.AnalogCallback");

    osvr::clientkit::Interface analog1 = context.getInterface("/com_osvr_emulated_devices/dev/analog/0");
    osvr::clientkit::Interface analog2 = context.getInterface("/com_osvr_emulated_devices/dev/analog/1");
    analog1.registerCallback(&AnalogCallback, NULL);
    analog2.registerCallback(&AnalogCallback2, NULL);

    // Pretend that this is your application's mainloop.
    while (true) {
        context.update();
    }

    std::cout << "Library shut down, exiting." << std::endl;
    return 0;
}
