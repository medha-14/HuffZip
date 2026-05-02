#include "huffman.h"
#include "bitstream.h"
#include "file_io.h"

#include <iostream>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <cassert>

// ---------------------------------------------------------------------------
// Pretty-print the code table
// ---------------------------------------------------------------------------
static void print_code_table(const std::unordered_map<char, std::string>& codes) {
    std::cout << "\n--- Huffman Code Table ---\n";
    std::cout << "Char  | Freq would be here | Code\n";
    std::cout << "------+--------------------+------\n";
    // Sort by code length for readability
    std::vector<std::pair<char, std::string>> sorted(codes.begin(), codes.end());
    std::sort(sorted.begin(), sorted.end(),
              [](const auto& a, const auto& b) {
                  return a.second.size() < b.second.size();
              });
    for (auto& [ch, code] : sorted) {
        std::string display = (ch == '\n') ? "\\n"
                            : (ch == '\t') ? "\\t"
                            : (ch == ' ')  ? "SPACE"
                            : std::string(1, ch);
        std::cout << "  " << display << "  |                    | " << code << "\n";
    }
    std::cout << "--------------------------\n\n";
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main() {
    // -----------------------------------------------------------------
    // 1. Read input
    // -----------------------------------------------------------------
    const std::string input_path = "input.txt";
    std::string original;
    try {
        original = read_text_file(input_path);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    }

    if (original.empty()) {
        std::cerr << "[ERROR] Input file is empty.\n";
        return 1;
    }

    std::cout << "=== Huffman Compression Demo ===\n";
    std::cout << "Input file  : " << input_path << "\n";
    std::cout << "Input size  : " << original.size() << " bytes\n";

    // -----------------------------------------------------------------
    // 2. Count frequencies
    // -----------------------------------------------------------------
    auto freq_table = uency_table(original);
    std::cout << "Unique chars: " << freq_table.size() << "\n";

    // -----------------------------------------------------------------
    // 3. Build Huffman tree
    // -----------------------------------------------------------------
    auto tree = build_huffman_tree(freq_table);

    // -----------------------------------------------------------------
    // 4. Generate codes
    // -----------------------------------------------------------------
    std::unordered_map<char, std::string> codes;
    generate_codes(tree.get(), "", codes);
    print_code_table(codes);

    // -----------------------------------------------------------------
    // 5. Encode  (bit-string of '0'/'1' chars)
    // -----------------------------------------------------------------
    std::string bit_string = encode(original, codes);
    std::cout << "Encoded     : " << bit_string.size() << " bits\n";

    // Pack into actual bytes
    BitWriter writer;
    writer.write_bits(bit_string);
    std::string packed = writer.finish();
    // Store padding info so decoder knows how many trailing bits to ignore
    int padding = static_cast<int>(
        (8 - (bit_string.size() % 8)) % 8);
    std::cout << "Packed size : " << packed.size() << " bytes  (padding=" << padding << " bits)\n";

    double ratio = static_cast<double>(packed.size()) /
                   static_cast<double>(original.size());
    std::cout << "Compression : " << ratio * 100.0 << "% of original\n";

    // -----------------------------------------------------------------
    // 6. Decode  (verify correctness)
    // -----------------------------------------------------------------
    // Reconstruct the exact bit_string from packed bytes, minus padding
    BitReader reader(packed);
    std::string recovered_bits;
    recovered_bits.reserve(bit_string.size());
    size_t total_bits = bit_string.size();   // we know this from the encode step
    for (size_t i = 0; i < total_bits; ++i) {
        recovered_bits += (reader.read_bit() == 1) ? '1' : '0';
    }

    std::string decoded = decode(recovered_bits, tree.get());

    // -----------------------------------------------------------------
    // 7. Verify
    // -----------------------------------------------------------------
    std::cout << "\n--- Verification ---\n";
    if (decoded == original) {
        std::cout << "[PASS] Decoded output matches original input exactly.\n";
        write_text_file("output.txt", decoded);
        std::cout << "Output written to output.txt\n";
    } else {
        std::cerr << "[FAIL] Decoded output DOES NOT match original!\n";
        std::cerr << "  Original length : " << original.size() << "\n";
        std::cerr << "  Decoded  length : " << decoded.size()  << "\n";
        return 1;
    }

    return 0;
}
