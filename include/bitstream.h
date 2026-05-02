#pragma once
#include <string>
#include <cstdint>

using namespace std;

// Packs bits into bytes, MSB first.
class BitWriter {
public:
    void write_bit(int bit);
    void write_bits(const string& bit_string);
    string finish(); // flush last partial byte
private:
    string buffer_;
    uint8_t current_byte_ = 0;
    int bit_count_ = 0;
};

// Reads bits from a packed byte string, MSB first.
class BitReader {
public:
    explicit BitReader(const string& data);
    int  read_bit();       // returns 0 or 1
    bool exhausted() const;
private:
    const string& data_;
    size_t byte_pos_ = 0;
    int    bit_pos_  = 7;
};
