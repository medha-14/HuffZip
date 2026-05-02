#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

// ---------------------------------------------------------------------------
// Huffman tree node
// ---------------------------------------------------------------------------
struct HuffNode {
    char     ch;
    int      freq;
    std::unique_ptr<HuffNode> left;
    std::unique_ptr<HuffNode> right;

    // Leaf node
    HuffNode(char c, int f) : ch(c), freq(f) {}

    // Internal node
    HuffNode(int f, std::unique_ptr<HuffNode> l, std::unique_ptr<HuffNode> r)
        : ch('\0'), freq(f), left(std::move(l)), right(std::move(r)) {}

    bool is_leaf() const { return !left && !right; }
};

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

/// Count character frequencies in a string.
std::unordered_map<char, int> build_frequency_table(const std::string& text);

/// Build a Huffman tree from a frequency table.
/// Returns nullptr if the table is empty.
std::unique_ptr<HuffNode> build_huffman_tree(
    const std::unordered_map<char, int>& freq_table);

/// Walk the tree and populate `codes` (char -> bit-string like "010").
void generate_codes(const HuffNode* node,
                    const std::string& prefix,
                    std::unordered_map<char, std::string>& codes);

/// Encode `text` using `codes`. Returns a string of '0'/'1' characters.
std::string encode(const std::string& text,
                   const std::unordered_map<char, std::string>& codes);

/// Decode a bit-string back to the original text using the Huffman tree.
std::string decode(const std::string& bit_string, const HuffNode* root);
