#pragma once

#include "common.hpp"
#include "packet.hpp"
#include <iostream>
#include <optional>
#include <vector>
#include <bitset>

using std::cout, std::cerr, std::endl, std::vector;

template <typename T> using Opt = std::optional<T>;

// The host and all functions must perform a complete decoding of all received PID fields. Any PID received with a failed check field or which decodes to a non-defined value is assumed to be corrupted and it, as well as the remainder of the packet, is ignored by the packet receiver.
// Ref: USB 2 specification, pg 195
class Computer {
    Signal &d_plus, &d_minus;
    
    Opt<Device_Type> device_type;
    int device_id = -1; // actually this is assigned 'by' the computer to each usb device

    D_Lines_State j_state, k_state; // depends on device type

  public:
    Computer(Signal &d_plus, Signal &d_minus)
        : d_plus(d_plus), d_minus(d_minus), device_type(std::nullopt) {}

    // basically, block until there is some signal on d_plus and d_minus
    void wait_for_device() {
        cout << "[pc] Waiting for a device..." << endl;
        auto state = read_line_state(d_plus, d_minus);

        if (state == DIFF_1) {
            device_type = LowSpeed;
            j_state = DIFF_0;
            k_state = DIFF_1;

            cout << "[pc] Found a Low Speed USB device" << endl;
        } else if (state == DIFF_0) {
            device_type = FullSpeed;
            j_state = DIFF_1;
            k_state = DIFF_0;

            cout << "[pc] Found a Full Speed USB device" << endl;
        } else {
            // Ignore ! Not valid 'initial' signals from a valid usb device
            device_type = std::nullopt;

            cerr << "[pc] [Warning] Received invalid start signals from d_plus and "
                    "d_minus. Ignoring..."
                 << endl;
        }
    }

    void request_exclusive_write() {
        d_plus.lock_and_discard();
        d_minus.lock_and_discard();
    }

    void send_in_token() {
        auto packet = create_token_in_packet(j_state, k_state, device_id, 1/*HARDCODING endpoint as 1*/);
    }
};

