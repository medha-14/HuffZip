CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Iinclude
SRCS     = src/huffman.cpp src/bitstream.cpp src/file_io.cpp
PREFIX   ?= $(HOME)/.local
BINDIR   = $(PREFIX)/bin

.PHONY: all install uninstall clean

all: build/compress build/decompress

build/compress: src/compress.cpp $(SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $^ -o $@

build/decompress: src/decompress.cpp $(SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $^ -o $@

install: all
	@mkdir -p $(BINDIR)
	cp build/compress   $(BINDIR)/compress
	cp build/decompress $(BINDIR)/decompress
	@chmod +x $(BINDIR)/compress $(BINDIR)/decompress
	@echo ""
	@echo "Installed to $(BINDIR)"
	@echo ""
	@if echo "$$PATH" | grep -q "$(BINDIR)"; then \
		echo "  compress <file>       — compress any text file"; \
		echo "  decompress <file.huff> [out] — decompress it back"; \
	else \
		echo "  Add this to your ~/.bashrc or ~/.zshrc:"; \
		echo ""; \
		echo "    export PATH=\"$(BINDIR):\$$PATH\""; \
		echo ""; \
		echo "  Then reload your shell:  source ~/.bashrc"; \
	fi
	@echo ""

uninstall:
	rm -f $(BINDIR)/compress $(BINDIR)/decompress
	@echo "Uninstalled."

clean:
	rm -rf build/
