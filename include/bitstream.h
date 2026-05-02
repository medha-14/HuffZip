#pragma once

#include <string>
#include <cstdint>

// ---------------------------------------------------------------------------
// BitWriter  – packs individual bits into bytes (MSB first within each byte).
// After writing all bits, call finish() to flush the last partial byte.
// ---------------------------------------------------------------------------
class BitWriter {
public:
    void write_bit(int bit);          // bit must be 0 or 1
    void write_bits(const std::string& bit_string); // '0'/'1' string
    std::string finish();             // returns the packed byte string

private:
    std::string buffer_;   // accumulated full bytes
    uint8_t     current_byte_  = 0;
    int         bit_count_     = 0;  // how many bits are filled in current_byte_
};

// ---------------------------------------------------------------------------
// BitReader  – reads individual bits from a packed byte string (MSB first).
// ---------------------------------------------------------------------------
class BitReader {
public:
    explicit BitReader(const std::string& data);

    int  read_bit();        // returns 0 or 1; throws if exhausted
    bool exhausted() const;

private:
    const std::string& data_;
    size_t  byte_pos_ = 0;
    int     bit_pos_  = 7;  // MSB first
};
