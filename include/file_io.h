#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>

using namespace std;

string read_binary_file(const string& path);
void        write_binary_file(const string& path, const string& data);

// .huff format: magic(4) + num_entries(4) + [byte(1)+freq(4)]*N + bit_count(8) + packed_data
void write_huff_file(const string& path,
                     const unordered_map<uint8_t, int>& freq_table,
                     uint64_t bit_count,
                     const string& packed_data);

struct HuffHeader {
    unordered_map<uint8_t, int> freq_table;
    uint64_t                         bit_count = 0;
    string                      packed_data;
};

HuffHeader read_huff_file(const string& path);
