#pragma once

#include <string>
#include <cstdio>
#include <exception>
#include <system_error>

using namespace std;

class Chunk
{
public:
    Chunk(string const& filename, const void *buffer, size_t count);
    Chunk(Chunk const& other);
    Chunk(Chunk &&other) noexcept;
    Chunk& operator=(const Chunk& other);
    bool read_next_value() const;
    uint32_t const& value() const { return _value; }
    void remove();    
protected:
    int seek(long int offset, int origin = SEEK_SET);
    size_t write(const void *buffer, size_t count);
protected:
    mutable string  _filename;
    mutable FILE *_file;
    mutable uint32_t _value;
};