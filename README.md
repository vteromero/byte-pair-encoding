##About bpe

**bpe** is a file compression tool that implements the Byte Pair Encoding (BPE) algorithm (http://en.wikipedia.org/wiki/Byte_pair_encoding) with fixed-size blocks.

###Requeriments

* gcc >= 4.7

###Compiling

You can compile by running:

    make

This will generate the binary file (`bpe`).

### Running

You can try this command to compress a file:

```bash
./bpe -c test.txt
```

You can also use other options:

```bash
# -b option specifies the block size
./bpe -c -b 4096 test.txt

# -v option shows you the reduction percentage
./bpe -c -v test.txt

# -o option is used to write the output to stdout
./bpe -c -v -o test.txt > test.txt.bpe
```

And if you want to decompress a file, you can use this command:

```bash
./bpe -d test.txt.bpe
```

*Note: test.tx is the original file and test.txt.bpe is the compressed file.*
