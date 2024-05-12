#pragma once

#include <fstream>
#include <iostream>
#include <string>

class ConfigFile {
public:
    std::string path;
    double maxMemory = 9;

    ConfigFile(std::string path) : path(path) {}

    void parse() {
        std::fstream file{path, std::ios::in | std::ios::app};
        if(!(file >> maxMemory)){
            maxMemory = 9;
        }
        file.close();
    }

    void save() {
        std::fstream file{path, std::ios::out | std::ios::trunc};
        file << maxMemory;
        file.close();
    }
};