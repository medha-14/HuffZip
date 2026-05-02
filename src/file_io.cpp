#include "file_io.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstring>   // memcpy

// ---------------------------------------------------------------------------
// Text helpers
// ---------------------------------------------------------------------------
std::string read_text_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file: '" + path + "'");
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void write_text_file(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Cannot write file: '" + path + "'");
    file << content;
}

// ---------------------------------------------------------------------------
// Binary helpers
// ---------------------------------------------------------------------------
std::string read_binary_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file: '" + path + "'");
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void write_binary_file(const std::string& path, const std::string& data) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Cannot write file: '" + path + "'");
    file.write(data.data(), static_cast<std::streamsize>(data.size()));
}

// ---------------------------------------------------------------------------
// Little-endian encode/decode helpers (internal)
// ---------------------------------------------------------------------------
static void write_u32(std::string& buf, uint32_t v) {
    buf += static_cast<char>( v        & 0xFF);
    buf += static_cast<char>((v >>  8) & 0xFF);
    buf += static_cast<char>((v >> 16) & 0xFF);
    buf += static_cast<char>((v >> 24) & 0xFF);
}

static void write_u64(std::string& buf, uint64_t v) {
    for (int i = 0; i < 8; ++i)
        buf += static_cast<char>((v >> (8 * i)) & 0xFF);
}

static uint32_t read_u32(const std::string& buf, size_t& pos) {
    if (pos + 4 > buf.size())
        throw std::runtime_error("read_huff_file: unexpected end of header");
    uint32_t v = 0;
    for (int i = 0; i < 4; ++i)
        v |= static_cast<uint32_t>(static_cast<uint8_t>(buf[pos++])) << (8 * i);
    return v;
}

static uint64_t read_u64(const std::string& buf, size_t& pos) {
    if (pos + 8 > buf.size())
        throw std::runtime_error("read_huff_file: unexpected end of header");
    uint64_t v = 0;
    for (int i = 0; i < 8; ++i)
        v |= static_cast<uint64_t>(static_cast<uint8_t>(buf[pos++])) << (8 * i);
    return v;
}

// ---------------------------------------------------------------------------
// .huff format
// ---------------------------------------------------------------------------
void write_huff_file(const std::string& path,
                     const std::unordered_map<char, int>& freq_table,
                     uint64_t bit_count,
                     const std::string& packed_data)
{
    std::string buf;
    buf.reserve(4 + 4 + freq_table.size() * 5 + 8 + packed_data.size());

    // Magic
    buf += 'H'; buf += 'U'; buf += 'F'; buf += 'F';

    // Number of entries
    write_u32(buf, static_cast<uint32_t>(freq_table.size()));

    // Frequency table
    for (auto& [ch, freq] : freq_table) {
        buf += static_cast<char>(ch);
        write_u32(buf, static_cast<uint32_t>(freq));
    }

    // Bit count
    write_u64(buf, bit_count);

    // Packed data
    buf += packed_data;

    write_binary_file(path, buf);
}

HuffHeader read_huff_file(const std::string& path) {
    std::string raw = read_binary_file(path);
    size_t pos = 0;

    // Magic check
    if (raw.size() < 4 || raw.substr(0, 4) != "HUFF")
        throw std::runtime_error("Not a valid .huff file: '" + path + "'");
    pos = 4;

    HuffHeader hdr;

    // Number of entries
    uint32_t num_entries = read_u32(raw, pos);

    // Frequency table
    for (uint32_t i = 0; i < num_entries; ++i) {
        if (pos >= raw.size())
            throw std::runtime_error("read_huff_file: truncated frequency table");
        char ch = raw[pos++];
        uint32_t freq = read_u32(raw, pos);
        hdr.freq_table[ch] = static_cast<int>(freq);
    }

    // Bit count
    hdr.bit_count = read_u64(raw, pos);

    // Payload
    hdr.packed_data = raw.substr(pos);

    return hdr;
}
