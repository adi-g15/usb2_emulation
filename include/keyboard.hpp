#pragma once

#include "common.hpp"
#include "signal.hpp"
#include <iostream>

using std::cout, std::cerr, std::endl;

class KeyBoard {
    const Device_Type device_type = FullSpeed; // low speed device by default

    Signal d_plus, d_minus;
    D_Lines_State
        last_sent /* = J state*/; // used to determine which state to sent to
                                  // send a 1 or 0, accoding to NRZI

  public:
    KeyBoard() {}

    // not literally 'wait' in actual cases
    void start_sending_signals() {
        cout << "Sending signals to let the other side know my speed" << endl;

        d_plus.lock_and_discard();
        d_minus.lock_and_discard();
        if (device_type == LowSpeed) {
            if (!send_diff_one(d_plus, d_minus)) {
                throw std::logic_error(
                    "Unable to send signal... Maybe check the cable :)");
            }
        } else {
            if (!send_diff_zero(d_plus, d_minus)) {
                throw std::logic_error(
                    "Unable to send signal... Maybe check the cable :)");
            }
        }
    }

    Signal &d_plus_ref() { return d_plus; }
    Signal &d_minus_ref() { return d_minus; }
};
