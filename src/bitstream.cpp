#include "bitstream.h"
#include <stdexcept>

void BitWriter::write_bit(int bit) {
    current_byte_ = static_cast<uint8_t>((current_byte_ << 1) | (bit & 1));
    if (++bit_count_ == 8) {
        buffer_ += static_cast<char>(current_byte_);
        current_byte_ = 0;
        bit_count_    = 0;
    }
}

void BitWriter::write_bits(const std::string& bit_string) {
    for (char c : bit_string) write_bit(c == '1' ? 1 : 0);
}

std::string BitWriter::finish() {
    if (bit_count_ > 0) {
        buffer_ += static_cast<char>(current_byte_ << (8 - bit_count_));
        current_byte_ = 0;
        bit_count_    = 0;
    }
    return buffer_;
}

BitReader::BitReader(const std::string& data)
    : data_(data), byte_pos_(0), bit_pos_(7) {}

int BitReader::read_bit() {
    if (exhausted()) throw std::runtime_error("BitReader: read past end");
    int bit = (static_cast<uint8_t>(data_[byte_pos_]) >> bit_pos_) & 1;
    if (--bit_pos_ < 0) { bit_pos_ = 7; ++byte_pos_; }
    return bit;
}

bool BitReader::exhausted() const {
    return byte_pos_ >= data_.size();
}
