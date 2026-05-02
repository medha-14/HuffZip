#include "huffman.h"
#include <algorithm>
#include <queue>
#include <vector>
#include <stdexcept>

using namespace std;

FreqTable build_frequency_table(const string& data) {
    FreqTable freq;
    for (uint8_t b : data) freq[b]++;
    return freq;
}

unique_ptr<HuffNode> build_huffman_tree(const FreqTable& freq_table) {
    if (freq_table.empty()) return nullptr;

    struct Entry {
        int freq, seq;
        HuffNode* node;
        bool operator>(const Entry& o) const {
            return freq != o.freq ? freq > o.freq : seq > o.seq;
        }
    };

    // Sort by byte value for deterministic seeding regardless of map order
    vector<pair<uint8_t,int>> sorted(freq_table.begin(), freq_table.end());
    sort(sorted.begin(), sorted.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });

    priority_queue<Entry, vector<Entry>, greater<Entry>> pq;
    int seq = 0;
    for (auto& [b, f] : sorted)
        pq.push({f, seq++, new HuffNode(b, f)});

    if (pq.size() == 1) {
        HuffNode* only = pq.top().node; pq.pop();
        return make_unique<HuffNode>(
            only->freq, unique_ptr<HuffNode>(only), nullptr);
    }

    while (pq.size() > 1) {
        auto [lf, ls, left]  = pq.top(); pq.pop();
        auto [rf, rs, right] = pq.top(); pq.pop();
        pq.push({lf + rf, seq++,
                 new HuffNode(lf + rf,
                              unique_ptr<HuffNode>(left),
                              unique_ptr<HuffNode>(right))});
    }

    return unique_ptr<HuffNode>(pq.top().node);
}

void generate_codes(const HuffNode* node,
                    const string& prefix,
                    CodeTable& codes)
{
    if (!node) return;
    if (node->is_leaf()) {
        codes[node->ch] = prefix.empty() ? "0" : prefix;
        return;
    }
    generate_codes(node->left.get(),  prefix + "0", codes);
    generate_codes(node->right.get(), prefix + "1", codes);
}

string encode(const string& data, const CodeTable& codes) {
    string result;
    result.reserve(data.size() * 4);
    for (uint8_t b : data) {
        auto it = codes.find(b);
        if (it == codes.end())
            throw runtime_error("encode: no code for byte");
        result += it->second;
    }
    return result;
}

string decode(const string& bit_string, const HuffNode* root) {
    if (!root) return "";

    string result;
    const HuffNode* cur = root;

    for (char bit : bit_string) {
        if (root->is_leaf()) { result += static_cast<char>(root->ch); continue; }
        cur = (bit == '0') ? cur->left.get() : cur->right.get();
        if (!cur) throw runtime_error("decode: invalid bit sequence");
        if (cur->is_leaf()) { result += static_cast<char>(cur->ch); cur = root; }
    }
    return result;
}
