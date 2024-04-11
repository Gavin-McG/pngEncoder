debug: pngEncoder.cpp imageInfo.cpp color.cpp crc32.cpp adler32.cpp utilities.cpp bitstream.cpp huffman.cpp lz77.cpp deflate.cpp
	g++ -std=c++20 -o pngEncoder -g3 -Wall -Werror -pedantic pngEncoder.cpp imageInfo.cpp color.cpp crc32.cpp adler32.cpp utilities.cpp bitstream.cpp huffman.cpp lz77.cpp deflate.cpp

main: pngEncoder.cpp imageInfo.cpp color.cpp crc32.cpp adler32.cpp utilities.cpp bitstream.cpp huffman.cpp lz77.cpp deflate.cpp
	g++ -std=c++20 -o pngEncoder -O3 -Wall -Werror -pedantic pngEncoder.cpp imageInfo.cpp color.cpp crc32.cpp adler32.cpp utilities.cpp bitstream.cpp huffman.cpp lz77.cpp deflate.cpp