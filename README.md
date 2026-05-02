# HuffZip

A lossless file compressor using Huffman coding. Compresses text files into a compact `.huff` binary format and decompresses them back perfectly.

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

```bash
# Compress a file → produces file.huff
compress notes.txt

# Decompress → produces output.txt by default
decompress notes.txt.huff

# Decompress to a specific file
decompress notes.txt.huff restored.txt
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

1. Counts character frequencies in the input file
2. Builds a Huffman tree — frequent characters get short codes, rare ones get long codes
3. Encodes the file as a bit stream and packs it into bytes
4. Stores the frequency table in the `.huff` header so the decompressor can rebuild the tree

The decompressor reads the header, rebuilds the identical tree, unpacks the bits, and decodes the original text exactly.

## File format (`.huff`)

```
[4B]  magic: HUFF
[4B]  number of unique characters
[5B × N]  frequency table: char (1B) + frequency (4B)
[8B]  total bit count
[*]   packed bit data
```
