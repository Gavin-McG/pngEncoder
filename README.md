# PNG Encoder/Decoder

A lightweight PNG encoder and decoder written entirely in C++ using only the C++ Standard Library (STL).  
The project implements the PNG file format from scratch, including custom chunk handling, image filtering, and a fully manual DEFLATE/GZIP compression system.  
It supports both image creation/encoding and decoding for educational and experimental purposes.

---

## Features

- Pure C++ STL implementation (no external libraries or zlib)  
- Full PNG encoding and decoding pipeline  
- Custom DEFLATE and Huffman compression implementation  
- Supports RGB and RGBA truecolor images  
- Multiple PNG filter modes (None, Sub, Up, Average, Paeth)  
- Optional debug output for Huffman, LZ77, and filter stages  
- Non-interlaced encoding and decoding  
- CLI tools for encoding, decoding, and testing PNG variants

---

## Intended Use

This project is designed primarily as an educational reference implementation of the PNG format.  
It can be used to explore:
- PNG file structure and chunk organization  
- Deflate compression and Huffman coding  
- PNG filtering methods and reconstruction  
- Image encoding/decoding at the binary level  

---

## Usage

### Encoding

./pngencode [options]

markdown
Copy code

**Options:**

| Flag | Description | Default |
|------|--------------|----------|
| `-o`, `--output <file>` | Output PNG file name | `default.png` |

**Example:**
./pngencode -o output.png

yaml
Copy code

This will encode a generated test image and save it to `output.png`.

---

### Decoding

./pngdecode [options]

markdown
Copy code

**Options:**

| Flag | Description | Default |
|------|--------------|----------|
| `-i`, `--input <file>` | Input PNG file name | `default.png` |
| `-o`, `--output <file>` | Output PNG file name | `default.png` |
| `-h`, `--debug-huffman` | Enable Huffman tree debug output | Off |
| `-l`, `--debug-lz77` | Enable LZ77 compression debug output | Off |
| `-f`, `--debug-filters` | Enable PNG filter debug output | Off |
| `-d`, `--debug` | Enable all debug modes | Off |

**Example:**
./pngdecode -i input.png -o output.png -d

yaml
Copy code

This reads `input.png`, decodes it, optionally applies filters, and writes the reconstructed image to `output.png` with verbose debug information.

---

## Debug Options

Debug output can be selectively enabled for different subsystems:
- **Huffman Debug:** prints Huffman tree construction and code table information  
- **LZ77 Debug:** shows LZ77 match finding and distance/length encoding  
- **Filter Debug:** displays the effect of different PNG filters on image data  
- **Global Debug (`-d`):** enables all the above at once  

---

## Limitations

- Does not support interlaced (Adam7) or indexed-color PNGs  
- Focused on correctness and educational value rather than performance  
- Tested primarily with 8-bit truecolor and truecolor+alpha images  

## License

This project is released under the MIT License.  
You are free to use, modify, and extend it for your own projects or educational purposes.
