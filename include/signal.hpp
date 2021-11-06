#pragma once

#include <atomic>
#include <cstdint>
#include <errno.h>
#include <mutex>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

// Just a wrapper over pipe, or queue, depends
class Signal {
    int _signal_line[2];
    int keyboard_pid; // pid of keyboard process = the process that constructed
                      // this signal line
    std::atomic_int locked_writer_pid; // the pid that will be writing
    std::atomic_flag reads_blocked;

  public:
    Signal() : keyboard_pid(getpid()), locked_writer_pid(-1) {
        /**
         * The  socketpair() function shall create an unbound pair of connected
       sockets in a specified domain, of a specified type, under the protocol
       optionally specified by the protocol argument. The two sockets shall  be
       identical.  The file  descriptors  used  in  referencing the created
       sockets shall be returned in socket_vector[0] and socket_vector[1]
        */
        if (socketpair(AF_LOCAL, SOCK_STREAM, 0 /*auto*/, _signal_line) != 0) {
            perror("Error while signal construction: ");
        }
    }

    // this shows 'intent', that the other one is writing, so we should read,
    // instead of writing now
    inline bool is_locked_by_the_other() {
        return locked_writer_pid != getpid();
    }

    // Good Practise: In most cases, discard all messages first, before sending
    // yours
    //                So, that no 'old messages' are read by the other expecting
    //                your messages, but instead keeps reading their own
    //                messages
    inline void _lock() { // prefer lock_and_read instead
        locked_writer_pid.store(getpid());
    }
    // or lock AND ignore (old messages)
    // TODO: Instead of discarding, handle the messages, else this may lead to
    // missing some messages
    void lock_and_discard() {
        uint8_t buffer[1024];
        _lock();

        reads_blocked.test_and_set();
        while (::recv(_signal_line[1], (void *)buffer, sizeof(buffer),
                      MSG_DONTWAIT) > 0) {
        }
        reads_blocked.clear();
    }
    inline void unlock() { locked_writer_pid.store(-1); }

    // ONLY one process must be writing to it
    bool send(uint8_t byte) {
#if defined(__cpp_lib_atomic_flag_test)
        while (reads_blocked.test()) {
        }
#else
#error "Requires C++20 for a 'small' addition to the atomic_flag"
#endif

        // the process must get exclusive write access first, and only it can
        // write
        if (locked_writer_pid == -1 || locked_writer_pid != getpid()) {
            return false;
        }

        // Why not ::send ?
        // Ans. With a zero flags argument, send() is equivalent to write(2)
        if (::write(_signal_line[0], (void *)(&byte), sizeof(byte)) == -1) {
            // A simple program can use C++ exceptions right ? 😢
            throw std::logic_error("Failed to write");
        }

        return true; // wrote
    }

    uint8_t peek() {
        uint8_t buffer;
        if (::recv(_signal_line[1], (void *)(&buffer), sizeof(buffer),
                   MSG_PEEK) == -1) {
            throw std::logic_error("Failed to peek");
        }

        return buffer;
    }

    uint8_t read() {
        uint8_t buffer;
        if (::read(_signal_line[1], (void *)(&buffer), sizeof(buffer)) == -1) {
            throw std::logic_error("Failed to read");
        }

        return buffer;
    }
};
