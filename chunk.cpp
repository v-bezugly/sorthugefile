#include <chunk.hpp>


Chunk::Chunk(string const& filename, const void *buffer, size_t count):
        _filename(filename),
        _file(nullptr),
        _value(0)
{
    _file = ::fopen(filename.c_str(), "w+b");
    if(!_file) throw system_error(errno, generic_category(), _filename);

    write(buffer, count);
    seek(0);
}

Chunk::Chunk(Chunk const& other): _filename(other._filename), _value(other._value)
{
    _file = other._file;
}

Chunk::Chunk(Chunk &&other) noexcept:
        _filename(move(other._filename)),
        _file(other._file),
        _value(other._value)
{
    
    other._file = nullptr;
    _value = 0;
}

Chunk& Chunk::operator=(const Chunk& other)
{
    _filename  = other._filename;
    _file      = other._file;
    _value     = other._value;

    return *this;
}

bool Chunk::read_next_value() const
{
    return 1 == ::fread(&_value, sizeof(_value), 1, _file);
}

size_t Chunk::write(const void *buffer, size_t count)
{
    return ::fwrite(buffer, sizeof(uint32_t), count, _file);
}

int Chunk::seek(long int offset, int origin)
{
    return fseek(_file, offset, origin);
}

void Chunk::remove()
{
    ::remove(_filename.c_str());
}
