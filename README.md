# sorthugefile

Два варианта сборки.

1.
mkdir build && cd build
cmake .. && make

2.
c++ main.cpp sort_huge_file.cpp chunk.cpp -std=c++14 -D_NDEBUG -O3 -lpthread -I. -o test
