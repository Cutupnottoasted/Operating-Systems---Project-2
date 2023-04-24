#ifndef LOGGER_H
#define LOGGER_H
#include <fstream> 

void log_file(const std::string &text)
{
    std::ofstream log_file("log_file.txt", std::ios_base::out | std::ios_base::app); 
    log_file << text << std::endl << std::endl; 
}

void reset() { std::ofstream reset("log_file.txt", std::ios::trunc); }

#endif