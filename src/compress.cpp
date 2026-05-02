// compress.cpp — Usage:  ./compress <input_file>
//
// Produces: <input_file>.huff
// Prints stats and the Huffman code table.

#include "huffman.h"
#include "bitstream.h"
#include "file_io.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <string>
#include <iomanip>

// Pretty-print the code table (sorted by code length)
static void print_code_table(const std::unordered_map<char, std::string>& codes,
                              const std::unordered_map<char, int>& freq)
{
    std::vector<std::pair<char, std::string>> sorted(codes.begin(), codes.end());
    std::sort(sorted.begin(), sorted.end(),
              [](const auto& a, const auto& b) {
                  return a.second.size() < b.second.size();
              });

    std::cout << "\n";
    std::cout << "  Char     Freq   Bits   Code\n";
    std::cout << "  -------  -----  ----   ----\n";
    for (auto& [ch, code] : sorted) {
        std::string display = (ch == '\n') ? "\\n"
                            : (ch == '\t') ? "\\t"
                            : (ch == '\r') ? "\\r"
                            : (ch == ' ')  ? "SPACE"
                            : std::string(1, ch);
        int f = freq.count(ch) ? freq.at(ch) : 0;
        std::cout << "  " << std::left  << std::setw(7) << display
                  << "  " << std::right << std::setw(5) << f
                  << "  " << std::setw(4) << code.size()
                  << "   " << code << "\n";
    }
    std::cout << "\n";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: compress <input_file>\n";
        return 1;
    }

    const std::string input_path  = argv[1];
    const std::string output_path = input_path + ".huff";

    // 1. Read input
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

    std::cout << "Compressing: " << input_path << "\n";
    std::cout << "Input size : " << original.size() << " bytes\n";

    // 2. Frequency table
    auto freq_table = build_frequency_table(original);

    // 3. Huffman tree
    auto tree = build_huffman_tree(freq_table);

    // 4. Generate codes
    std::unordered_map<char, std::string> codes;
    generate_codes(tree.get(), "", codes);
    print_code_table(codes, freq_table);

    // 5. Encode → bit-string
    std::string bit_string = encode(original, codes);

    // 6. Pack bits into bytes
    BitWriter writer;
    writer.write_bits(bit_string);
    std::string packed = writer.finish();

    // 7. Write .huff file (header stores freq table + bit count for decoder)
    try {
        write_huff_file(output_path, freq_table,
                        static_cast<uint64_t>(bit_string.size()), packed);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    }

    // 8. Stats
    double ratio = 100.0 * static_cast<double>(packed.size())
                         / static_cast<double>(original.size());
    int saving = static_cast<int>(original.size()) - static_cast<int>(packed.size());

    std::cout << "Output     : " << output_path << "\n";
    std::cout << "Encoded    : " << bit_string.size() << " bits\n";
    std::cout << "Packed     : " << packed.size() << " bytes\n";
    std::cout << "Ratio      : " << std::fixed << std::setprecision(1)
              << ratio << "% of original  (saved " << saving << " bytes)\n";

    return 0;
}
