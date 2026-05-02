#include "huffman.h"
#include <algorithm>
#include <queue>
#include <vector>
#include <stdexcept>

std::unordered_map<char, int> build_frequency_table(const std::string& text) {
    std::unordered_map<char, int> freq;
    for (char c : text) freq[c]++;
    return freq;
}

std::unique_ptr<HuffNode> build_huffman_tree(
    const std::unordered_map<char, int>& freq_table)
{
    if (freq_table.empty()) return nullptr;

    // Use a seq number to break ties deterministically regardless of map
    // iteration order. Nodes inserted first (lower seq) go left when
    // frequencies are equal, giving a stable tree across separate runs.
    struct Entry {
        int freq;
        int seq;
        HuffNode* node;
        bool operator>(const Entry& o) const {
            if (freq != o.freq) return freq > o.freq;
            return seq > o.seq;
        }
    };

    // Sort by char so insertion order is deterministic
    std::vector<std::pair<char,int>> sorted(freq_table.begin(), freq_table.end());
    std::sort(sorted.begin(), sorted.end(),
              [](const auto& a, const auto& b) { return (unsigned char)a.first < (unsigned char)b.first; });

    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> pq;
    int seq = 0;
    for (auto& [ch, f] : sorted)
        pq.push({f, seq++, new HuffNode(ch, f)});

    // Single unique character edge case
    if (pq.size() == 1) {
        HuffNode* only = pq.top().node; pq.pop();
        return std::make_unique<HuffNode>(
            only->freq, std::unique_ptr<HuffNode>(only), nullptr);
    }

    while (pq.size() > 1) {
        auto [lf, ls, left]  = pq.top(); pq.pop();
        auto [rf, rs, right] = pq.top(); pq.pop();
        pq.push({lf + rf, seq++,
                 new HuffNode(lf + rf,
                              std::unique_ptr<HuffNode>(left),
                              std::unique_ptr<HuffNode>(right))});
    }

    return std::unique_ptr<HuffNode>(pq.top().node);
}

void generate_codes(const HuffNode* node,
                    const std::string& prefix,
                    std::unordered_map<char, std::string>& codes)
{
    if (!node) return;
    if (node->is_leaf()) {
        codes[node->ch] = prefix.empty() ? "0" : prefix;
        return;
    }
    generate_codes(node->left.get(),  prefix + "0", codes);
    generate_codes(node->right.get(), prefix + "1", codes);
}

std::string encode(const std::string& text,
                   const std::unordered_map<char, std::string>& codes)
{
    std::string result;
    result.reserve(text.size() * 4);
    for (char c : text) {
        auto it = codes.find(c);
        if (it == codes.end())
            throw std::runtime_error(std::string("encode: no code for '") + c + "'");
        result += it->second;
    }
    return result;
}

std::string decode(const std::string& bit_string, const HuffNode* root) {
    if (!root) return "";

    std::string result;
    const HuffNode* cur = root;

    for (char bit : bit_string) {
        if (root->is_leaf()) { result += root->ch; continue; }
        cur = (bit == '0') ? cur->left.get() : cur->right.get();
        if (!cur) throw std::runtime_error("decode: invalid bit sequence");
        if (cur->is_leaf()) { result += cur->ch; cur = root; }
    }
    return result;
}
