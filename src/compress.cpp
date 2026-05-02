// Usage: ./compress <input_file>
#include "huffman.h"
#include "bitstream.h"
#include "file_io.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <string>
#include <iomanip>

static void print_code_table(const std::unordered_map<char, std::string>& codes,
                              const std::unordered_map<char, int>& freq)
{
    std::vector<std::pair<char, std::string>> sorted(codes.begin(), codes.end());
    std::sort(sorted.begin(), sorted.end(),
              [](const auto& a, const auto& b) { return a.second.size() < b.second.size(); });

    std::cout << "\n  Char     Freq   Bits   Code\n";
    std::cout <<   "  -------  -----  ----   ----\n";
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

    std::string original;
    try {
        original = read_text_file(input_path);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    }

    if (original.empty()) { std::cerr << "[ERROR] Input file is empty.\n"; return 1; }

    auto freq_table = build_frequency_table(original);
    auto tree       = build_huffman_tree(freq_table);

    std::unordered_map<char, std::string> codes;
    generate_codes(tree.get(), "", codes);
    print_code_table(codes, freq_table);

    std::string bit_string = encode(original, codes);

    BitWriter writer;
    writer.write_bits(bit_string);
    std::string packed = writer.finish();

    try {
        write_huff_file(output_path, freq_table,
                        static_cast<uint64_t>(bit_string.size()), packed);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    }

    double ratio  = 100.0 * packed.size() / original.size();
    int    saving = static_cast<int>(original.size()) - static_cast<int>(packed.size());

    std::cout << "Input   : " << input_path  << " (" << original.size() << " bytes)\n";
    std::cout << "Output  : " << output_path << " (" << packed.size()   << " bytes)\n";
    std::cout << "Ratio   : " << std::fixed << std::setprecision(1) << ratio
              << "%  (saved " << saving << " bytes)\n";
    return 0;
}
