#pragma once

// Frame is big, so it's usually divided into multiple packets.
// The first packet contains a frame header, which registers a new frame, and the current frame ID, 
// and a packet length, which says how many sub-packets this frame contains (so there will be that much IDs).
// All packets therafter contains a macro frame ID, a micro frame ID, data length, and actual data.
class Frame {
};

