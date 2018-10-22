clear
make clean
make

echo "Data / Type / Size"
echo "=================="
echo "OS / pdf / 9.2Mb"
./huffman -e 1 tests/OS.pdf OS.huff
./huffman -e 2 tests/OS.pdf OS.huff
./huffman -e 4 tests/OS.pdf OS.huff
./huffman -e 8 tests/OS.pdf OS.huff
./huffman -e 16 tests/OS.pdf OS.huff
./huffman -e 32 tests/OS.pdf OS.huff
./huffman -e 64 tests/OS.pdf OS.huff
echo "=================="

echo "Metaxas / mpeg / 64Mb"
./huffman -e 1 tests/metaxas-keller-Bell.mpeg meta.huff
./huffman -e 2 tests/metaxas-keller-Bell.mpeg meta.huff
./huffman -e 4 tests/metaxas-keller-Bell.mpeg meta.huff
./huffman -e 8 tests/metaxas-keller-Bell.mpeg meta.huff
./huffman -e 16 tests/metaxas-keller-Bell.mpeg meta.huff
./huffman -e 32 tests/metaxas-keller-Bell.mpeg meta.huff
./huffman -e 64 tests/metaxas-keller-Bell.mpeg meta.huff
echo "=================="

echo "Wiki / 7z / 103Mb"
./huffman -e 1 tests/wiki_dump.xml wiki.huff
./huffman -e 2 tests/wiki_dump.xml wiki.huff
./huffman -e 4 tests/wiki_dump.xml wiki.huff
./huffman -e 8 tests/wiki_dump.xml wiki.huff
./huffman -e 16 tests/wiki_dump.xml wiki.huff
./huffman -e 32 tests/wiki_dump.xml wiki.huff
./huffman -e 64 tests/wiki_dump.xml wiki.huff
