// Usage: ./decompress <file.huff> [output_file]
#include "huffman.h"
#include "bitstream.h"
#include "file_io.h"

#include <iostream>
#include <string>

static std::string default_output(const std::string& huff_path) {
    const std::string suffix = ".huff";
    if (huff_path.size() > suffix.size() &&
        huff_path.substr(huff_path.size() - suffix.size()) == suffix)
        return huff_path.substr(0, huff_path.size() - suffix.size());
    return huff_path + ".out";
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: decompress <file.huff> [output_file]\n";
        return 1;
    }

    const std::string input_path  = argv[1];
    const std::string output_path = (argc == 3) ? argv[2] : default_output(input_path);

    HuffHeader hdr;
    try {
        hdr = read_huff_file(input_path);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    }

    auto tree = build_huffman_tree(hdr.freq_table);
    if (!tree) { std::cerr << "[ERROR] Empty frequency table.\n"; return 1; }

    BitReader reader(hdr.packed_data);
    std::string bit_string;
    bit_string.reserve(hdr.bit_count);
    for (uint64_t i = 0; i < hdr.bit_count; ++i)
        bit_string += (reader.read_bit() == 1) ? '1' : '0';

    std::string decoded = decode(bit_string, tree.get());

    try {
        write_binary_file(output_path, decoded);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    }

    std::cout << "Output : " << output_path << " (" << decoded.size() << " bytes)\n";
    std::cout << "[OK] Done.\n";
    return 0;
}
