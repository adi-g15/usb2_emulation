#pragma once

#include "enums.hpp"
#include "common.hpp"
#include <CRC.h>
#include <cstdint>
#include <vector>
#include <bitset>

using Packet = std::vector<bool>;

// Reference: USB 2.0 specification Page 196

// PID bits will be sent on the USB in LSB-first (bit 0) order
// PID shown here on left side is in MSB first order (0th bit at last)
// clang-format off
enum PID: uint8_t {
    // Type_Name

    TOKEN_OUT = 1,        /*0001*/
    TOKEN_IN = 9,         /*1001*/
    TOKEN_SOF = 5,        /*0101*/
    TOKEN_SETUP = 13,     /*1101*/

    DATA_DATA0 = 3,       /*0011*/
    DATA_DATA1 = 11,      /*1011*/
    DATA_DATA2 = 7,       /*0111*/
    DATA_MDATA = 15,      /*1111*/

    HANDSHAKE_ACK = 2,    /*0010*/
    HANDSHAKE_NAK = 10,   /*1010*/
    HANDSHAKE_STALL = 14, /*1110*/
    HANDSHAKE_NYET = 6,   /*0110*/

    SPECIAL_PRE = 12,      /*1100*/
    SPECIAL_ERR = 12,      /*1100*/ /*reuses PRE value*/
    SPECIAL_SPLIT = 8,    /*1000*/
    SPECIAL_PING = 4,     /*0100*/
    SPECIAL_RESERVED = 0, /*0000*/
};
// clang-format on

// class Packet {
//     public:
//     Packet(vector<bool> packet_bits /*including SOP & EOP*/) {}
// };

/**
 * \param addr -> 7 bit address
 * \param endp -> 4 bit endpoint
 *
 * \note last state doesn't matter, since SOP always starts with a K state
 */
Packet create_token_in_packet(D_Lines_State j_state, D_Lines_State k_state, uint8_t address, uint8_t endp) {
        Packet pkt;
        // just adding an idle state, next will be K, signifying SOP (Start of Packet)
        pkt.push_back(j_state);

        // (Ref: 8.3, USB2 specification) The Start-of-Packet (SOP) delimiter is part of the SYNC field
        // SYNC start - 3KJ + 2K
        pkt.push_back(k_state);
        pkt.push_back(j_state);
        pkt.push_back(k_state);
        pkt.push_back(j_state);
        pkt.push_back(k_state);
        pkt.push_back(j_state);

        pkt.push_back(k_state);
        pkt.push_back(k_state);
        // SYNC end

        // Should be pushed in LSB-first order
        // bitset[0] is initialised with the LSB
        // (Ref: https://devdocs.io/cpp/utility/bitset/bitset)
        std::bitset<4> pid(PID::TOKEN_IN);
        auto last_state = k_state;  // ie. = the end of SYNC
        for(int i=0; i<4; ++i) {
            // we can't directly send 1 or 0, we have to sent in NRZI encoding,
            // ie. in either J or K, depending on last state send
            last_state = get_next_state_to_send(last_state, pid[i]);
            pkt.push_back( last_state );
        }
        // Now, send the 4 bit pid complemented, for a sanity check only
        for(int i=0; i<4; ++i) {
            last_state = get_next_state_to_send(last_state, !pid[i]);
            pkt.push_back( last_state );
        }

        std::bitset<7> addr( address & 0x7f );    // taking only the lowest significant 7 bits, this may cause problem with >= 1<<8 devices (only on same bus likely, dont know on that part)
            // 0x7f = 127 = 0111 1111 = 1<<6|1<<5|1<<4|1<<3|1<<4|1<<2|1<<1|1
        for(int i=0; i<7; ++i) {
            last_state = get_next_state_to_send(last_state, addr[i]);
            pkt.push_back( last_state );
        }

        std::bitset<4> endpoint( endp & 0x0f ); // taking only lowest significant 4 bits
        for (int i=0; i<4; ++i) {
            last_state = get_next_state_to_send(last_state, endpoint[i]);
            pkt.push_back( last_state );
        }

        // Token CRC -> Considers ADDR, ENDP fields; Generator polynomial is
        // 10100, If all token bits are received without error, the five-bit residual at the receiver will be 01100
        CRC::crc        

        return pkt;
}

