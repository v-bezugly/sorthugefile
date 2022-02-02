#pragma once

#include <string>
#include <fstream>

void sort_huge_file(std::string const& input_filename, std::string const& output_filename, size_t total_memory_size);
void check_input_file(std::string const& filename);

