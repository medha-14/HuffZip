#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <cstdint>

struct HuffNode {
    uint8_t ch;
    int     freq;
    std::unique_ptr<HuffNode> left;
    std::unique_ptr<HuffNode> right;

    HuffNode(uint8_t c, int f) : ch(c), freq(f) {}
    HuffNode(int f, std::unique_ptr<HuffNode> l, std::unique_ptr<HuffNode> r)
        : ch(0), freq(f), left(std::move(l)), right(std::move(r)) {}

    bool is_leaf() const { return !left && !right; }
};

using FreqTable = std::unordered_map<uint8_t, int>;
using CodeTable = std::unordered_map<uint8_t, std::string>;

FreqTable build_frequency_table(const std::string& data);

std::unique_ptr<HuffNode> build_huffman_tree(const FreqTable& freq_table);

void generate_codes(const HuffNode* node,
                    const std::string& prefix,
                    CodeTable& codes);

std::string encode(const std::string& data, const CodeTable& codes);

std::string decode(const std::string& bit_string, const HuffNode* root);
