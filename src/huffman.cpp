#include "huffman.h"

#include <queue>
#include <stdexcept>

// ---------------------------------------------------------------------------
// Step 1: frequency table
// ---------------------------------------------------------------------------
std::unordered_map<char, int> build_frequency_table(const std::string& text) {
    std::unordered_map<char, int> freq;
    for (char c : text) {
        freq[c]++;
    }
    return freq;
}

// ---------------------------------------------------------------------------
// Step 2: build Huffman tree using a min-heap
// ---------------------------------------------------------------------------
std::unique_ptr<HuffNode> build_huffman_tree(
    const std::unordered_map<char, int>& freq_table)
{
    if (freq_table.empty()) return nullptr;

    // Comparator: min-heap by frequency (ties broken by char value for determinism)
    auto cmp = [](const HuffNode* a, const HuffNode* b) {
        if (a->freq != b->freq) return a->freq > b->freq;
        return a->ch > b->ch;
    };
    std::priority_queue<HuffNode*, std::vector<HuffNode*>, decltype(cmp)> pq(cmp);

    // Seed the heap with leaf nodes
    for (auto& [ch, f] : freq_table) {
        pq.push(new HuffNode(ch, f));
    }

    // Special case: single unique character — give it a single-bit code
    if (pq.size() == 1) {
        HuffNode* only = pq.top(); pq.pop();
        auto root = std::make_unique<HuffNode>(
            only->freq,
            std::unique_ptr<HuffNode>(only),
            nullptr
        );
        return root;
    }

    // Build tree bottom-up
    while (pq.size() > 1) {
        HuffNode* left  = pq.top(); pq.pop();
        HuffNode* right = pq.top(); pq.pop();

        HuffNode* parent = new HuffNode(
            left->freq + right->freq,
            std::unique_ptr<HuffNode>(left),
            std::unique_ptr<HuffNode>(right)
        );
        pq.push(parent);
    }

    return std::unique_ptr<HuffNode>(pq.top());
}

// ---------------------------------------------------------------------------
// Step 3: generate codes (DFS)
// ---------------------------------------------------------------------------
void generate_codes(const HuffNode* node,
                    const std::string& prefix,
                    std::unordered_map<char, std::string>& codes)
{
    if (!node) return;

    if (node->is_leaf()) {
        // Handle single-character edge case: assign "0" if prefix is empty
        codes[node->ch] = prefix.empty() ? "0" : prefix;
        return;
    }

    generate_codes(node->left.get(),  prefix + "0", codes);
    generate_codes(node->right.get(), prefix + "1", codes);
}

// ---------------------------------------------------------------------------
// Step 4: encode
// ---------------------------------------------------------------------------
std::string encode(const std::string& text,
                   const std::unordered_map<char, std::string>& codes)
{
    std::string result;
    result.reserve(text.size() * 4);  // rough estimate
    for (char c : text) {
        auto it = codes.find(c);
        if (it == codes.end()) {
            throw std::runtime_error(
                std::string("encode: no code for character '") + c + "'");
        }
        result += it->second;
    }
    return result;
}

// ---------------------------------------------------------------------------
// Step 5: decode
// ---------------------------------------------------------------------------
std::string decode(const std::string& bit_string, const HuffNode* root) {
    if (!root) return "";

    std::string result;
    const HuffNode* cur = root;

    for (char bit : bit_string) {
        // Single-character tree: every '0' maps to the one leaf
        if (root->is_leaf()) {
            result += root->ch;
            continue;
        }

        if (bit == '0') {
            cur = cur->left.get();
        } else {
            cur = cur->right.get();
        }

        if (!cur) {
            throw std::runtime_error("decode: invalid bit sequence");
        }

        if (cur->is_leaf()) {
            result += cur->ch;
            cur = root;  // reset to root for next character
        }
    }

    return result;
}
