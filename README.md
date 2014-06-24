##About bpe

**bpe** is a file compression tool that implements the Byte Pair Encoding (BPE) algorithm (http://en.wikipedia.org/wiki/Byte_pair_encoding) with fixed-size blocks.

###Requeriments

Because **bpe** uses some features of c++11, you have to install a gcc version greater or equal to 4.3.

###Compiling

You can compile by running:

    make

This will generate the binary file (`bpe`).

### Running

You can try this command to compress a file:

```bash
./bpe -c test.txt test.txt.bpe
```

You can also use other options:

```bash
# -b option specifies the block size
./bpe -b 4096 -c test.txt test.txt.bpe

# -v option show you the reduction percentage
./bpe -v -c test.txt test.txt.bpe
```

And if you want to decompress a file, you can use this command:

```bash
./bpe -d test.txt.bpe test.txt
```

*Note: test.tx is the original file and test.txt.bpe is the compressed file.*