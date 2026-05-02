#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

// ---------------------------------------------------------------------------
// Text helpers
// ---------------------------------------------------------------------------

/// Read the entire contents of a file (text mode) into a string.
std::string read_text_file(const std::string& path);

/// Write `content` to a text file, overwriting if it exists.
void write_text_file(const std::string& path, const std::string& content);

// ---------------------------------------------------------------------------
// Binary helpers
// ---------------------------------------------------------------------------

/// Read raw bytes from a file (binary mode).
std::string read_binary_file(const std::string& path);

/// Write raw bytes to a file (binary mode).
void write_binary_file(const std::string& path, const std::string& data);

// ---------------------------------------------------------------------------
// .huff file format helpers
//
//  Header layout (all values little-endian):
//   [4]  magic: 'H','U','F','F'
//   [4]  uint32_t  num_entries   — number of unique characters
//   for each entry:
//     [1]  uint8_t   ch
//     [4]  uint32_t  frequency
//   [8]  uint64_t  bit_count    — number of valid bits in the payload
//   [*]  packed bit data
// ---------------------------------------------------------------------------

/// Write a .huff file: header (magic + freq table + bit_count) + packed bits.
void write_huff_file(const std::string& path,
                     const std::unordered_map<char, int>& freq_table,
                     uint64_t bit_count,
                     const std::string& packed_data);

struct HuffHeader {
    std::unordered_map<char, int> freq_table;
    uint64_t                      bit_count = 0;
    std::string                   packed_data;
};

/// Read a .huff file and return the header + packed data.
/// Throws std::runtime_error if the magic bytes are wrong.
HuffHeader read_huff_file(const std::string& path);
