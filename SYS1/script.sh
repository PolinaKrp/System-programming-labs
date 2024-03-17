mkdir A
cd A
head -c 256 /dev/urandom > 1.txt
truncate -s 256 2.txt
tr '\000' '\111' < 2.txt > 2.tmp && mv 2.tmp 2.txt
mkdir B
cd B
truncate -s 256 3.txt
mkdir C
cd C
ln -s 1.txt 4.txt
ln 1.txt 5.txt