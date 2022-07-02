//
// Input Mapping Demo
// http://scribblings-by-apoch.googlecode.com/
//
// Helper functions for file I/O
//

#pragma once

#include <istream>

/**
 * Helper for attempting to read from a file (or other istream-compatible interface)
 */
template<typename OutType>
OutType attemptRead(std::wistream& stream) {
    OutType out;
    if (!(stream >> out))
        throw std::runtime_error("Failed to read a required value");
    return out;
}