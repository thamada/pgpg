CC = gcc  
OPT_WALL = -O4 -Wall
PGPGCM = ./pgpgcm
#OPT_WALL = -Wall

test0_bits : 
	echo '---'
	mkdir -p tmp
	cpp LIST.bits >list.tmp
	$(PGPGCM) ./list.tmp 
	gcc -Wall -O4 CHECK_bits_join_part.c -o runtest_000
	rm -rf list.tmp tmp pg_module.c
	echo '---'
	mkdir -p tmp
	cpp LIST.bits >list.tmp
	$(PGPGCM) ./list.tmp 
	gcc -Wall -O4 CHECK_bits_and_or_xor_inv.c -o runtest_001
	rm -rf list.tmp tmp pg_module.c
	echo '---'
	date >testlog~
	./runtest_000 2>> testlog~
	./runtest_001 2>> testlog~
	rm -rf runtest*
	cat testlog~




