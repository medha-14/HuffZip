#include "huffman.h"
#include "bitstream.h"
#include "file_io.h"

#include <iostream>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <cassert>
#include <vector>
#include <cstdint>

using namespace std;

// ---------------------------------------------------------------------------
// Pretty-print the code table
// ---------------------------------------------------------------------------
static void print_code_table(const unordered_map<uint8_t, string>& codes) {
    cout << "\n--- Huffman Code Table ---\n";
    cout << "Char  | Freq would be here | Code\n";
    cout << "------+--------------------+------\n";
    // Sort by code length for readability
    vector<pair<uint8_t, string>> sorted(codes.begin(), codes.end());
    sort(sorted.begin(), sorted.end(),
              [](const auto& a, const auto& b) {
                  return a.second.size() < b.second.size();
              });
    for (auto& [ch, code] : sorted) {
        string display = (ch == '\n') ? "\\n"
                            : (ch == '\t') ? "\\t"
                            : (ch == ' ')  ? "SPACE"
                            : string(1, ch);
        cout << "  " << display << "  |                    | " << code << "\n";
    }
    cout << "--------------------------\n\n";
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main() {
    // -----------------------------------------------------------------
    // 1. Read input
    // -----------------------------------------------------------------
    const string input_path = "input.txt";
    string original;
    try {
        original = read_binary_file(input_path);
    } catch (const exception& e) {
        cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    }

    if (original.empty()) {
        cerr << "[ERROR] Input file is empty.\n";
        return 1;
    }

    cout << "=== Huffman Compression Demo ===\n";
    cout << "Input file  : " << input_path << "\n";
    cout << "Input size  : " << original.size() << " bytes\n";

    // -----------------------------------------------------------------
    // 2. Count frequencies
    // -----------------------------------------------------------------
    auto freq_table = build_frequency_table(original);
    cout << "Unique chars: " << freq_table.size() << "\n";

    // -----------------------------------------------------------------
    // 3. Build Huffman tree
    // -----------------------------------------------------------------
    auto tree = build_huffman_tree(freq_table);

    // -----------------------------------------------------------------
    // 4. Generate codes
    // -----------------------------------------------------------------
    unordered_map<uint8_t, string> codes;
    generate_codes(tree.get(), "", codes);
    print_code_table(codes);

    // -----------------------------------------------------------------
    // 5. Encode  (bit-string of '0'/'1' chars)
    // -----------------------------------------------------------------
    string bit_string = encode(original, codes);
    cout << "Encoded     : " << bit_string.size() << " bits\n";

    // Pack into actual bytes
    BitWriter writer;
    writer.write_bits(bit_string);
    string packed = writer.finish();
    // Store padding info so decoder knows how many trailing bits to ignore
    int padding = static_cast<int>(
        (8 - (bit_string.size() % 8)) % 8);
    cout << "Packed size : " << packed.size() << " bytes  (padding=" << padding << " bits)\n";

    double ratio = static_cast<double>(packed.size()) /
                   static_cast<double>(original.size());
    cout << "Compression : " << ratio * 100.0 << "% of original\n";

    // -----------------------------------------------------------------
    // 6. Decode  (verify correctness)
    // -----------------------------------------------------------------
    // Reconstruct the exact bit_string from packed bytes, minus padding
    BitReader reader(packed);
    string recovered_bits;
    recovered_bits.reserve(bit_string.size());
    size_t total_bits = bit_string.size();   // we know this from the encode step
    for (size_t i = 0; i < total_bits; ++i) {
        recovered_bits += (reader.read_bit() == 1) ? '1' : '0';
    }

    string decoded = decode(recovered_bits, tree.get());

    // -----------------------------------------------------------------
    // 7. Verify
    // -----------------------------------------------------------------
    cout << "\n--- Verification ---\n";
    if (decoded == original) {
        cout << "[PASS] Decoded output matches original input exactly.\n";
        write_binary_file("output.txt", decoded);
        cout << "Output written to output.txt\n";
    } else {
        cerr << "[FAIL] Decoded output DOES NOT match original!\n";
        cerr << "  Original length : " << original.size() << "\n";
        cerr << "  Decoded  length : " << decoded.size()  << "\n";
        return 1;
    }

    return 0;
}
