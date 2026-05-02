#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <cstdint>

using namespace std;

struct HuffNode {
    uint8_t ch;
    int     freq;
    unique_ptr<HuffNode> left;
    unique_ptr<HuffNode> right;

    HuffNode(uint8_t c, int f) : ch(c), freq(f) {}
    HuffNode(int f, unique_ptr<HuffNode> l, unique_ptr<HuffNode> r)
        : ch(0), freq(f), left(move(l)), right(move(r)) {}

    bool is_leaf() const { return !left && !right; }
};

using FreqTable = unordered_map<uint8_t, int>;
using CodeTable = unordered_map<uint8_t, string>;

FreqTable build_frequency_table(const string& data);

unique_ptr<HuffNode> build_huffman_tree(const FreqTable& freq_table);

void generate_codes(const HuffNode* node,
                    const string& prefix,
                    CodeTable& codes);

string encode(const string& data, const CodeTable& codes);

string decode(const string& bit_string, const HuffNode* root);
