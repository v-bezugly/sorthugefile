/***********************************************************************************************
т.к. одно из условий было сильно алгоритм не усложнять, то я сделал только базовые оптимизации.
1. распараллелил сортировку при разделении большого файла на чанки.
2. оптимизировал поиск минимального значения с помощью приоритетной очереди при объединении чанков.
но при желании тут еще есть что оптимизировать.
************************************************************************************************
*/

#include <worker.hpp>
#include <chunk.hpp>
#include <filename_generator.hpp>
#include <sort_huge_file.hpp>

#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <cstdio>
#include <string>
#include <utility>
#include <queue>

using namespace std;

vector<Chunk> split_to_sorted_chunks(string const& filename, size_t total_memory_size);
void merge_chunks(vector<Chunk>& chunks, string const& filename);
std::streampos get_file_size(ifstream& s);

void sort_huge_file(string const& input_filename, 
                    string const& output_filename, 
                    size_t total_memory_size)
{
    auto chunks = split_to_sorted_chunks(input_filename, total_memory_size);

    merge_chunks(chunks, output_filename);

    for(auto& chunk: chunks) chunk.remove();
}

vector<Chunk> split_to_sorted_chunks(string const& filename, size_t total_memory_size)
{
    ifstream input_file(filename, ios::binary);

    auto file_size = get_file_size(input_file);
    auto threads_count = std::thread::hardware_concurrency();
    threads_count = (0 == threads_count) ? 1 : threads_count;
    decltype(file_size) chunk_memory_size = total_memory_size / threads_count;

    vector<Chunk> chunks;

    Workers workers(threads_count);

    FilenameGenerator filename_generator;

    mutex input_file_locker;
    mutex chunks_locker;
    for(; file_size > 0; file_size -= chunk_memory_size)
    {
        chunk_memory_size = min(file_size, static_cast<decltype(file_size)>(chunk_memory_size));
        workers.push([&input_file_locker, &chunks_locker, &input_file, &chunks, chunk_memory_size, &filename_generator]()
        {
            vector<uint32_t> buff(chunk_memory_size/sizeof(uint32_t));

            {
                lock_guard<mutex> lock(input_file_locker);
                input_file.read(reinterpret_cast<char*>(buff.data()), buff.size()*sizeof(uint32_t));
            }

            sort(buff.begin(), buff.end(), less<uint32_t>());

            Chunk chunk(filename_generator(), buff.data(), buff.size());

            chunk.read_next_value();

            {
                lock_guard<mutex> lock(chunks_locker);
                chunks.push_back(move(chunk));
            }
        });
    }

    workers.run();

    return chunks;
}

void merge_chunks(vector<Chunk>& chunks, string const& filename)
{
    ofstream output_file(filename, std::ios::binary);

    auto comp = [](Chunk const &l, Chunk const &r) { return l.value() > r.value(); };

    std::priority_queue<Chunk, std::vector<Chunk>, decltype(comp)>
                            values(chunks.begin(), chunks.end(), comp);

    while(!values.empty())
    {
        Chunk curr = move(values.top());
        values.pop();
        output_file.write(reinterpret_cast<const char*>(&curr.value()), sizeof(curr.value()));

        if(curr.read_next_value()) {
            values.emplace(curr);
        }
    }
}

void check_input_file(string const& filename)
{
    ifstream f(filename, ios::binary);
    if(!f.is_open()) throw system_error(errno, generic_category(), filename);

    auto file_size = get_file_size(f);    
    if(file_size % sizeof(uint32_t) != 0) 
        throw std::runtime_error("fail of input file size (file size % sizeof(uint32_t) == 0)");
}

std::streampos get_file_size(std::ifstream& s)
{
    auto curr_pos = s.tellg();
    s.seekg (0, s.end);
    auto size = s.tellg();
    s.seekg (curr_pos, s.beg);

    return size;
}
