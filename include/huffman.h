#pragma once
#include <string>
#include <unordered_map>
#include <memory>

struct HuffNode {
    char ch;
    int  freq;
    std::unique_ptr<HuffNode> left;
    std::unique_ptr<HuffNode> right;

    HuffNode(char c, int f) : ch(c), freq(f) {}
    HuffNode(int f, std::unique_ptr<HuffNode> l, std::unique_ptr<HuffNode> r)
        : ch('\0'), freq(f), left(std::move(l)), right(std::move(r)) {}

    bool is_leaf() const { return !left && !right; }
};

std::unordered_map<char, int> build_frequency_table(const std::string& text);

std::unique_ptr<HuffNode> build_huffman_tree(
    const std::unordered_map<char, int>& freq_table);

void generate_codes(const HuffNode* node,
                    const std::string& prefix,
                    std::unordered_map<char, std::string>& codes);

std::string encode(const std::string& text,
                   const std::unordered_map<char, std::string>& codes);

std::string decode(const std::string& bit_string, const HuffNode* root);
