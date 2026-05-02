// decompress.cpp — Usage:  ./decompress <file.huff> [output_file]
//
// Produces: output_file if given, otherwise output.txt.
// Verifies the output size matches the header's recorded bit count.

#include "huffman.h"
#include "bitstream.h"
#include "file_io.h"

#include <iostream>
#include <string>
#include <iomanip>

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: decompress <file.huff> [output_file]\n";
        std::cerr << "  output_file defaults to output.txt\n";
        return 1;
    }

    const std::string input_path  = argv[1];
    const std::string output_path = (argc == 3) ? argv[2] : "output.txt";

    // 1. Read .huff file (header + packed data)
    HuffHeader hdr;
    try {
        hdr = read_huff_file(input_path);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    }

    std::cout << "Decompressing : " << input_path << "\n";
    std::cout << "Packed size   : " << hdr.packed_data.size() << " bytes\n";
    std::cout << "Expected bits : " << hdr.bit_count << "\n";

    // 2. Rebuild Huffman tree from stored frequency table
    auto tree = build_huffman_tree(hdr.freq_table);
    if (!tree) {
        std::cerr << "[ERROR] Frequency table is empty — cannot build tree.\n";
        return 1;
    }

    // 3. Unpack exactly bit_count bits from the packed payload
    BitReader reader(hdr.packed_data);
    std::string bit_string;
    bit_string.reserve(hdr.bit_count);
    for (uint64_t i = 0; i < hdr.bit_count; ++i) {
        bit_string += (reader.read_bit() == 1) ? '1' : '0';
    }

    // 4. Decode
    std::string decoded = decode(bit_string, tree.get());

    // 5. Write output
    try {
        write_text_file(output_path, decoded);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    }

    std::cout << "Output file   : " << output_path << "\n";
    std::cout << "Output size   : " << decoded.size() << " bytes\n";
    std::cout << "[OK] Decompression complete.\n";

    return 0;
}
