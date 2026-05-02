#include "bitstream.h"

#include <stdexcept>

// ---------------------------------------------------------------------------
// BitWriter
// ---------------------------------------------------------------------------
void BitWriter::write_bit(int bit) {
    // Pack bits MSB-first into each byte
    current_byte_ = static_cast<uint8_t>(
        (current_byte_ << 1) | (bit & 1));
    ++bit_count_;

    if (bit_count_ == 8) {
        buffer_ += static_cast<char>(current_byte_);
        current_byte_ = 0;
        bit_count_    = 0;
    }
}

void BitWriter::write_bits(const std::string& bit_string) {
    for (char c : bit_string) {
        write_bit(c == '1' ? 1 : 0);
    }
}

std::string BitWriter::finish() {
    if (bit_count_ > 0) {
        // Shift remaining bits to the MSB side, pad with zeros on the right
        current_byte_ = static_cast<uint8_t>(
            current_byte_ << (8 - bit_count_));
        buffer_ += static_cast<char>(current_byte_);
        current_byte_ = 0;
        bit_count_    = 0;
    }
    return buffer_;
}

// ---------------------------------------------------------------------------
// BitReader
// ---------------------------------------------------------------------------
BitReader::BitReader(const std::string& data)
    : data_(data), byte_pos_(0), bit_pos_(7) {}

int BitReader::read_bit() {
    if (exhausted()) {
        throw std::runtime_error("BitReader: attempt to read past end of data");
    }
    uint8_t byte = static_cast<uint8_t>(data_[byte_pos_]);
    int bit = (byte >> bit_pos_) & 1;

    if (--bit_pos_ < 0) {
        bit_pos_ = 7;
        ++byte_pos_;
    }
    return bit;
}

bool BitReader::exhausted() const {
    return byte_pos_ >= data_.size();
}
