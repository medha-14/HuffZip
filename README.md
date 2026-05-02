# huffman-compressor

A robust, lossless file compressor using Huffman coding. Compresses **any file type** into a compact `.huff` binary format and decompresses them back with byte-perfect integrity.

## Features
- **Universal Support**: Works on binary files (images, executables) as well as text.
- **Byte-Perfect**: Guarantees lossless compression and decompression.
- **Deterministic**: Implements stable Huffman tree construction to ensure consistent output across different runs.
- **Production Ready**: Clean C++17 implementation with robust error handling and binary I/O.

## Install

```bash
git clone <repo-url>
cd huffman_file_compresser
make install
```

This builds the project and copies `compress` and `decompress` into `~/.local/bin`.

If `~/.local/bin` is not in your PATH, add this to your `~/.bashrc` (or `~/.zshrc`):

```bash
export PATH="$HOME/.local/bin:$PATH"
```

Then reload your shell:

```bash
source ~/.bashrc
```

## Usage

### Command Line Tools
```bash
# Compress any file → produces file.huff
compress data.bin

# Decompress → produces original file by default
decompress data.bin.huff

# Decompress to a specific file
decompress data.bin.huff restored.bin
```

### Demonstration Tool
You can also run a standalone demo that compresses and decompresses a file in memory to verify correctness:
```bash
make
./build/demo
```

## Uninstall

```bash
make uninstall
```

## Build only (without installing)

```bash
make
# binaries are at build/compress and build/decompress
```

## How it works

1. **Count Frequencies**: Counts the occurrences of every byte value (0-255).
2. **Deterministic Tree**: Builds a Huffman tree using a stable priority queue. Frequent bytes get short bit-codes; rare ones get long codes.
3. **Binary Packing**: Encodes the data as a bit stream, packing 8 bits into each physical byte to achieve compression.
4. **Binary Header**: Stores a frequency table in the `.huff` header so the decompressor can rebuild the identical tree.

The decompressor reads the header, rebuilds the tree, unpacks the bits, and restores the original bytes exactly.

## File format (`.huff`)

```
[4B]      Magic: HUFF
[4B]      Number of unique byte entries
[5B × N]  Frequency Table: byte (1B) + frequency (4B)
[8B]      Total bit count
[*]       Packed bit data
```
