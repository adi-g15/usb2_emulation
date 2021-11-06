#include <sys/socket.h>
#include <errno.h>
#include <cstdint>
#include <unistd.h>
#include <stdexcept>

// Just a wrapper over pipe, or queue, depends
class Signal {
    int _signal_line[2];
    int keyboard_pid;   // pid of keyboard process

  public:
    Signal() {
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

    void lock() { /*TODO*/ }
    void unlock() { /*TODO*/ }

    // ONLY one process must be writing to it
    void send(uint8_t byte) {
        // Why not ::send ?
        // Ans. With a zero flags argument, send() is equivalent to write(2)
        if( 
            ::write( _signal_line[0], (void*)(&byte), sizeof(byte) )
            == -1
        ) {
            // A simple program can use C++ exceptions right ? 😢
            throw std::logic_error("Failed to write");
        }
    }

    uint8_t read() {
        uint8_t buffer;
        if( 
            ::read( _signal_line[1], (void*)(&buffer), sizeof(buffer) )
            == -1
        ) {
            throw std::logic_error("Failed to read");
        }

        return buffer;
    }
};
