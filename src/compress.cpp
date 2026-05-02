// Usage: ./compress <input_file>
#include "huffman.h"
#include "bitstream.h"
#include "file_io.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <sstream>

using namespace std;

static string byte_label(uint8_t b) {
    if (b == '\n') return "\\n";
    if (b == '\r') return "\\r";
    if (b == '\t') return "\\t";
    if (b == ' ')  return "SPACE";
    if (b >= 32 && b < 127) return string(1, static_cast<char>(b));
    // Non-printable: show as 0xXX
    ostringstream ss;
    ss << "0x" << hex << uppercase << setw(2) << setfill('0') << (int)b;
    return ss.str();
}

static void print_code_table(const CodeTable& codes, const FreqTable& freq) {
    vector<pair<uint8_t, string>> sorted(codes.begin(), codes.end());
    sort(sorted.begin(), sorted.end(),
              [](const auto& a, const auto& b) { return a.second.size() < b.second.size(); });

    cout << "\n  Byte     Freq   Bits   Code\n";
    cout <<   "  -------  -----  ----   ----\n";
    for (auto& [b, code] : sorted) {
        int f = freq.count(b) ? freq.at(b) : 0;
        cout << "  " << left  << setw(7) << byte_label(b)
                  << "  " << right << setw(5) << f
                  << "  " << setw(4) << code.size()
                  << "   " << code << "\n";
    }
    cout << "\n";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: compress <input_file>\n";
        return 1;
    }

    const string input_path  = argv[1];
    const string output_path = input_path + ".huff";

    string data;
    try {
        data = read_binary_file(input_path);
    } catch (const exception& e) {
        cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    }

    if (data.empty()) { cerr << "[ERROR] Input file is empty.\n"; return 1; }

    auto freq_table = build_frequency_table(data);
    auto tree       = build_huffman_tree(freq_table);

    CodeTable codes;
    generate_codes(tree.get(), "", codes);
    print_code_table(codes, freq_table);

    string bit_string = encode(data, codes);

    BitWriter writer;
    writer.write_bits(bit_string);
    string packed = writer.finish();

    try {
        write_huff_file(output_path, freq_table,
                        static_cast<uint64_t>(bit_string.size()), packed);
    } catch (const exception& e) {
        cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    }

    double ratio  = 100.0 * packed.size() / data.size();
    int    saving = static_cast<int>(data.size()) - static_cast<int>(packed.size());

    cout << "Input   : " << input_path  << " (" << data.size()   << " bytes)\n";
    cout << "Output  : " << output_path << " (" << packed.size() << " bytes)\n";
    cout << "Ratio   : " << fixed << setprecision(1) << ratio
              << "%  (saved " << saving << " bytes)\n";
    return 0;
}
