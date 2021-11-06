#pragma once

#include "common.hpp"
#include <iostream>
#include <optional>

using std::cout, std::cerr, std::endl;

template <typename T> using Opt = std::optional<T>;

class Computer {
    Signal &d_plus, &d_minus;
    Opt<Device_Type> device_type;
    D_Lines_State j_state, k_state; // depends on device type

  public:
    Computer(Signal &d_plus, Signal &d_minus)
        : d_plus(d_plus), d_minus(d_minus), device_type(std::nullopt) {}

    // basically, block until there is some signal on d_plus and d_minus
    void wait_for_device() {
        cout << "Waiting for a device..." << endl;
        auto state = get_state_from_signals(d_plus.read(), d_minus.read());

        if (state == DIFF_1) {
            device_type = LowSpeed;

            cout << "Found a Low Speed USB device" << endl;
        } else if (state == DIFF_0) {
            device_type = FullSpeed;

            cout << "Found a Full Speed USB device" << endl;
        } else {
            // Ignore ! Not valid 'initial' signals from a valid usb device
            device_type = std::nullopt;

            cerr << "[Warning] Received invalid start signals from d_plus and "
                    "d_minus. Ignoring..."
                 << endl;
        }
    }
};
