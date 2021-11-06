#include <iostream>
#include "signal.hpp"
#include <sys/wait.h>

using std::cin, std::cout, std::cerr, std::endl;

int main() {
    Signal d_plus, d_minus;

    auto pid = fork();

    if(pid == -1) {
        cerr << "ERROR forking";
    }

    if (pid == 0) {
        // CHILD

        d_plus.send('a');
        d_plus.send('d');
        d_plus.send('i');
        d_plus.send('t');
        d_plus.send('y');
        d_plus.send('a');

        wait(&pid);
    } else {
        // PARENT

        long i=6;
        while (i-->0)
            cout << d_plus.read();

        exit(0);
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
