# PNG Encoder/Decoder

A lightweight PNG encoder and decoder written entirely in C++ using only the C++ Standard Library (STL).  
This project provides a simple, educational implementation of the PNG format, including chunk structure, filtering, and a custom DEFLATE compression algorithm.

---

## Features

- Pure C++ STL implementation with no external libraries or zlib dependencies  
- Custom DEFLATE/GZIP compression implemented from scratch  
- Supports RGB and RGBA images  
- Does not support interleaved or indexed color types  
- Chunk-based encoding and decoding compliant with PNG specifications  
- Command-line interface with output file control (`--output` or `-o`)

---

## Intended Use

This project is designed as an educational and experimental PNG codec, demonstrating how image data, filtering, and compression interact at the binary level.  
It is useful for learning about:
- PNG file structure and chunk parsing  
- Bitstream compression and Huffman tree construction  
- Low-level image encoding and decoding

---

## Usage

./pngcodec [options]

markdown
Copy code

**Options:**

| Flag | Description | Default |
|------|--------------|----------|
| `-o`, `--output <file>` | Output PNG file name | `default.png` |

**Example:**
./pngcodec -o output.png

yaml
Copy code

---

## Project Structure

src/
├── main.cpp # Command-line entry point
├── imageInfo.cpp # Image data management
├── dynamicTree.cpp # Custom DEFLATE/Huffman implementation
include/
├── imageInfo.h
├── dynamicTree.h

yaml
Copy code

---

## Limitations

- No support for interleaved (Adam7) or indexed-color PNGs  
- Supports only non-interlaced truecolor and truecolor+alpha  
- Focused on clarity and correctness rather than performance

---

## License

This project is released under the MIT License.  
You are free to use, modify, and extend it for your own projects or educational purposes.
