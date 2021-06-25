gcc huffman.c -o huffman && ./huffman test_data/alice_in_wonderland.txt out
gcc uncompress.c -o unc && ./unc out > res
diff res test_data/alice_in_wonderland.txt
./huffman test_data/img.bmp out
./unc out > res
diff res test_data/img.bmp
rm out res huffman unc
