clear
make clean
make

./huffman -e 32 ../tests/tlotr.html tlotr.huff
./huffman -d 1 tlotr.huff tlotr.html
diff -s tlotr.html ../tests/tlotr.html
rm tlotr.*
