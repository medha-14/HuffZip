// Usage: ./decompress <file.huff> [output_file]
#include "huffman.h"
#include "bitstream.h"
#include "file_io.h"

#include <iostream>
#include <string>

using namespace std;

static string default_output(const string& huff_path) {
    const string suffix = ".huff";
    if (huff_path.size() > suffix.size() &&
        huff_path.substr(huff_path.size() - suffix.size()) == suffix)
        return huff_path.substr(0, huff_path.size() - suffix.size());
    return huff_path + ".out";
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        cerr << "Usage: decompress <file.huff> [output_file]\n";
        return 1;
    }

    const string input_path  = argv[1];
    const string output_path = (argc == 3) ? argv[2] : default_output(input_path);

    HuffHeader hdr;
    try {
        hdr = read_huff_file(input_path);
    } catch (const exception& e) {
        cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    }

    auto tree = build_huffman_tree(hdr.freq_table);
    if (!tree) { cerr << "[ERROR] Empty frequency table.\n"; return 1; }

    BitReader reader(hdr.packed_data);
    string bit_string;
    bit_string.reserve(hdr.bit_count);
    for (uint64_t i = 0; i < hdr.bit_count; ++i)
        bit_string += (reader.read_bit() == 1) ? '1' : '0';

    string decoded = decode(bit_string, tree.get());

    try {
        write_binary_file(output_path, decoded);
    } catch (const exception& e) {
        cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    }

    cout << "Output : " << output_path << " (" << decoded.size() << " bytes)\n";
    cout << "[OK] Done.\n";
    return 0;
}
