debug: pngEncoder.cpp imageInfo.cpp color.cpp chunks.cpp crc32.cpp adler32.cpp utilities.cpp bitstream.cpp huffman.cpp lz77.cpp
	g++ -o pngEncoder -g3 -Wall -Werror -pedantic pngEncoder.cpp imageInfo.cpp color.cpp chunks.cpp crc32.cpp adler32.cpp utilities.cpp bitstream.cpp huffman.cpp lz77.cpp

main: pngEncoder.cpp imageInfo.cpp color.cpp chunks.cpp crc32.cpp adler32.cpp utilities.cpp bitstream.cpp huffman.cpp lz77.cpp
	g++ -o pngEncoder -O3 -Wall -Werror -pedantic pngEncoder.cpp imageInfo.cpp color.cpp chunks.cpp crc32.cpp adler32.cpp utilities.cpp bitstream.cpp huffman.cpp lz77.cpp