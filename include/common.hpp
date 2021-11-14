#pragma once

#include "enums.hpp"
#include "signal.hpp"

#include <iostream>
using std::cout, std::endl;

// NRZI (Not Return to Zero Inverted) encoding
D_Lines_State get_next_state_to_send(D_Lines_State last_sent,
                                     uint8_t bit_to_encode) {
    // invariant = bit_to_encode must be either 0 or 1
    if (bit_to_encode == 1) {
        return last_sent; // no change
    } else {
        // change
        return last_sent == DIFF_1 ? DIFF_0 : DIFF_1;
    }
}

D_Lines_State read_line_state(Signal &d_plus, Signal &d_minus) {
    uint8_t d_plus_bit{ d_plus.read() }, d_minus_bit{d_minus.read()};
    printf("Recieved: d+ = %d, d- = %d\n", d_plus_bit, d_minus_bit);

    // invariant = bits must be either 0 or 1
    if (d_plus_bit == d_minus_bit) {
        return d_plus_bit == 0 ? SE0 : SE1;
    } else {
        return d_plus_bit == 0 ? DIFF_0 : DIFF_1;
    }
}

bool send_diff_one(Signal &d_plus, Signal &d_minus) {
    return d_plus.send(1) && d_minus.send(0);
}

bool send_diff_zero(Signal &d_plus, Signal &d_minus) {
    return d_plus.send(0) && d_minus.send(1);
}

bool send_single_ended_zero(Signal &d_plus, Signal &d_minus) {
    return d_plus.send(0) && d_minus.send(0);
}

bool send_single_ended_one(Signal &d_plus, Signal &d_minus) {
    return d_plus.send(1) && d_minus.send(1);
}
