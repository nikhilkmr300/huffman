# huffman

Tool to compress file using Huffman coding.

## Description

Huffman coding is a simple greedy approach for lossless data compression. It generates variable length codes, where more frequent characters get shorter codes and vice versa. For a description of the algorithm, refer [Wikipedia](https://en.wikipedia.org/wiki/Huffman_coding).

## Installation

You can download the executables `compress` and `decompress` as is from the repository.

If, however, you want to compile from source, you will need to have installed [Boost](https://www.boost.org/), [Cereal](https://uscilab.github.io/cereal/) and a version of the GNU C++ compiler (`g++`) capable of handling the C++17 standard.

## Example

Following is an example of how to use the application on a sample text file, say `sample.txt`.

### Encoding

To generate the encoded version of the file `sample.txt` and the corresponding decoding table file `decodetable.xml`, run
```
./compress sample.txt encoded.txt decodetable.xml
```

Say the sizes of `sample.txt`, `encoded.txt` and `decodetable.xml` are X, Y and Z KB, respectively.
You can find the compression ratio using the formula
$$\text{Compression Ratio} = \frac{X}{Y + Z}$$

If you want to read the contents of `encoded.txt`, you can use a hex viewer like `hexdump`.

### Decoding

To decompress the encoded file, run
```
./decompress encoded.txt decoded.txt decodetable.xml
```

Note: The code handles only ASCII characters, so decompressing an encoding of some `sample.txt` with non-ASCII characters might result in a corresponding `decoded.txt` that is smaller than `sample.txt`. In such a case, there might be some loss of data, but all the ASCII characters will be decoded fine.

If you want to hide the screaming warnings about non-ASCII characters not being accepted, redirect the `stderr` output to `/dev/null` as
```
./compress sample.txt encoded.txt decodetable.xml 2> /dev/null
```