#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>

std::string read_text_file(const std::string& path);
void write_text_file(const std::string& path, const std::string& content);

std::string read_binary_file(const std::string& path);
void write_binary_file(const std::string& path, const std::string& data);

// .huff format: magic(4) + num_entries(4) + [char(1)+freq(4)]*N + bit_count(8) + packed_data
void write_huff_file(const std::string& path,
                     const std::unordered_map<char, int>& freq_table,
                     uint64_t bit_count,
                     const std::string& packed_data);

struct HuffHeader {
    std::unordered_map<char, int> freq_table;
    uint64_t                      bit_count = 0;
    std::string                   packed_data;
};

HuffHeader read_huff_file(const std::string& path);
