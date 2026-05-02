// Usage: ./compress <input_file>
#include "huffman.h"
#include "bitstream.h"
#include "file_io.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <sstream>

static std::string byte_label(uint8_t b) {
    if (b == '\n') return "\\n";
    if (b == '\r') return "\\r";
    if (b == '\t') return "\\t";
    if (b == ' ')  return "SPACE";
    if (b >= 32 && b < 127) return std::string(1, static_cast<char>(b));
    // Non-printable: show as 0xXX
    std::ostringstream ss;
    ss << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)b;
    return ss.str();
}

static void print_code_table(const CodeTable& codes, const FreqTable& freq) {
    std::vector<std::pair<uint8_t, std::string>> sorted(codes.begin(), codes.end());
    std::sort(sorted.begin(), sorted.end(),
              [](const auto& a, const auto& b) { return a.second.size() < b.second.size(); });

    std::cout << "\n  Byte     Freq   Bits   Code\n";
    std::cout <<   "  -------  -----  ----   ----\n";
    for (auto& [b, code] : sorted) {
        int f = freq.count(b) ? freq.at(b) : 0;
        std::cout << "  " << std::left  << std::setw(7) << byte_label(b)
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

    std::string data;
    try {
        data = read_binary_file(input_path);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    }

    if (data.empty()) { std::cerr << "[ERROR] Input file is empty.\n"; return 1; }

    auto freq_table = build_frequency_table(data);
    auto tree       = build_huffman_tree(freq_table);

    CodeTable codes;
    generate_codes(tree.get(), "", codes);
    print_code_table(codes, freq_table);

    std::string bit_string = encode(data, codes);

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

    double ratio  = 100.0 * packed.size() / data.size();
    int    saving = static_cast<int>(data.size()) - static_cast<int>(packed.size());

    std::cout << "Input   : " << input_path  << " (" << data.size()   << " bytes)\n";
    std::cout << "Output  : " << output_path << " (" << packed.size() << " bytes)\n";
    std::cout << "Ratio   : " << std::fixed << std::setprecision(1) << ratio
              << "%  (saved " << saving << " bytes)\n";
    return 0;
}
