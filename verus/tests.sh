
g++ -c -g -O3 -march=native -I../ verusscan.cpp
g++ -c -g -O3 -march=native -I../ verus_clhash.cpp
gcc -c -g -O3 -march=native -I../ haraka.c
gcc -g -Og -I../ tests.c haraka.o verus_clhash.o verusscan.o -pthread -march=native -o test
./test