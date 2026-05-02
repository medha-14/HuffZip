#pragma once
#include <string>
#include <cstdint>

// Packs bits into bytes, MSB first.
class BitWriter {
public:
    void write_bit(int bit);
    void write_bits(const std::string& bit_string);
    std::string finish(); // flush last partial byte
private:
    std::string buffer_;
    uint8_t current_byte_ = 0;
    int bit_count_ = 0;
};

// Reads bits from a packed byte string, MSB first.
class BitReader {
public:
    explicit BitReader(const std::string& data);
    int  read_bit();       // returns 0 or 1
    bool exhausted() const;
private:
    const std::string& data_;
    size_t byte_pos_ = 0;
    int    bit_pos_  = 7;
};
