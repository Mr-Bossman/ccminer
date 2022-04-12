
g++ -c -g -O3 -march=native -I../ verusscan.cpp
g++ -c -g -O3 -march=native -I../ verus_clhash.cpp
gcc -pedantic -Wno-unused-variable -Wall -g -O2 -I../ tests.c haraka.c verus_clhash.o verusscan.o -pthread -march=native -o test
./test $1 $2