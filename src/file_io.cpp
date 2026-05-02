#include "file_io.h"
#include <fstream>
#include <stdexcept>

std::string read_binary_file(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f.is_open()) throw std::runtime_error("Cannot open: " + path);
    return std::string(std::istreambuf_iterator<char>(f),
                       std::istreambuf_iterator<char>());
}

void write_binary_file(const std::string& path, const std::string& data) {
    std::ofstream f(path, std::ios::binary);
    if (!f.is_open()) throw std::runtime_error("Cannot write: " + path);
    f.write(data.data(), static_cast<std::streamsize>(data.size()));
}

// Little-endian helpers
static void write_u32(std::string& buf, uint32_t v) {
    for (int i = 0; i < 4; ++i) buf += static_cast<char>((v >> (8 * i)) & 0xFF);
}
static void write_u64(std::string& buf, uint64_t v) {
    for (int i = 0; i < 8; ++i) buf += static_cast<char>((v >> (8 * i)) & 0xFF);
}
static uint32_t read_u32(const std::string& buf, size_t& pos) {
    if (pos + 4 > buf.size()) throw std::runtime_error("read_huff_file: truncated");
    uint32_t v = 0;
    for (int i = 0; i < 4; ++i) v |= static_cast<uint32_t>(static_cast<uint8_t>(buf[pos++])) << (8 * i);
    return v;
}
static uint64_t read_u64(const std::string& buf, size_t& pos) {
    if (pos + 8 > buf.size()) throw std::runtime_error("read_huff_file: truncated");
    uint64_t v = 0;
    for (int i = 0; i < 8; ++i) v |= static_cast<uint64_t>(static_cast<uint8_t>(buf[pos++])) << (8 * i);
    return v;
}

void write_huff_file(const std::string& path,
                     const std::unordered_map<uint8_t, int>& freq_table,
                     uint64_t bit_count,
                     const std::string& packed_data)
{
    std::string buf;
    buf += 'H'; buf += 'U'; buf += 'F'; buf += 'F';
    write_u32(buf, static_cast<uint32_t>(freq_table.size()));
    for (auto& [b, freq] : freq_table) {
        buf += static_cast<char>(b);
        write_u32(buf, static_cast<uint32_t>(freq));
    }
    write_u64(buf, bit_count);
    buf += packed_data;
    write_binary_file(path, buf);
}

HuffHeader read_huff_file(const std::string& path) {
    std::string raw = read_binary_file(path);
    if (raw.size() < 4 || raw.substr(0, 4) != "HUFF")
        throw std::runtime_error("Not a valid .huff file: " + path);

    size_t pos = 4;
    HuffHeader hdr;
    uint32_t num_entries = read_u32(raw, pos);
    for (uint32_t i = 0; i < num_entries; ++i) {
        if (pos >= raw.size()) throw std::runtime_error("read_huff_file: truncated");
        uint8_t b = static_cast<uint8_t>(raw[pos++]);
        hdr.freq_table[b] = static_cast<int>(read_u32(raw, pos));
    }
    hdr.bit_count   = read_u64(raw, pos);
    hdr.packed_data = raw.substr(pos);
    return hdr;
}
