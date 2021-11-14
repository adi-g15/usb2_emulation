#include "computer.hpp"
#include "keyboard.hpp"
#include <iostream>
#include <sys/wait.h>

using std::cin, std::cout, std::cerr, std::endl;

int main() {
    KeyBoard kb;

    auto &d_plus = kb.d_plus_ref();
    auto &d_minus = kb.d_minus_ref();

    auto computer_pid = fork();

    if (computer_pid == -1) {
        cerr << "ERROR forking";
    }

    if (computer_pid != 0) {
        try {
            // PARENT (Keyboard)
            kb.start_sending_signals();

            wait(&computer_pid);
        } catch (std::exception &e) {
            cerr << e.what();
        }
    } else {
        try {
            // CHILD (Computer)
            Computer c(d_plus, d_minus);
            c.wait_for_device();
            c.request_exclusive_write();    // so that both keyboard & pc dont send signals on the same line simultaneous
            c.send_in_token();

            // Key down, key up: ./include/linux/keyboard.h
            // Different keys: There is a pattern there: uapi/linux/keyboard.h

            exit(0);
        } catch (std::exception &e) {
            cerr << e.what();
        }
    }

    // One keyboard process, this main will be the computer
    // Common lines for both
    // Both will keep blocking for a next signal on the signal line
    // Once, something received... if already inside a packet, read it
    // Else, look for a SOP
    // If already inside a packet, check for EOP too
    // Max buffer of previous signals being 7 is enough (for basic, no perf
    // consideration)
}
