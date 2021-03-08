.SILENT:

all: compress.cpp decompress.cpp huffman.cpp huffman.hpp
	g++ -std=c++17 huffman.cpp compress.cpp -lboost_system -o compress
	g++ -std=c++17 huffman.cpp decompress.cpp -lboost_system -o decompress

compress: huffman.o compress.cpp
	g++ -std=c++17 huffman.cpp compress.cpp -lboost_system -o compress

decompress: huffman.o decompress.cpp
	g++ -std=c++17 huffman.cpp decompress.cpp -lboost_system -o decompress

huffman.o: huffman.cpp huffman.hpp
	g++ -std=c++17 -c huffman.cpp

clean:
	rm -f *.xml
	rm -f *_encoded.txt
	rm -f *_decoded.txt
	rm -f compress
	rm -f decompress