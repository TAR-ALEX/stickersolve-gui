#pragma once

#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <string>

class ConfigFile {
private:
    std::string dirnameOf(const std::string& fname)
    {
        size_t pos = fname.find_last_of("\\/");
        return (std::string::npos == pos)
            ? ""
            : fname.substr(0, pos);
    }
    void createDir() {
        boost::filesystem::path p = dirnameOf(path);
        if (!(boost::filesystem::exists(p))) { boost::filesystem::create_directories(p); }
    }

public:
    std::string path;
    double maxMemory = 9;

    ConfigFile(std::string path) : path(path) {}

    void parse() {
        createDir();
        std::ifstream file{path};
        if (!(file >> maxMemory)) { maxMemory = 9; }
        file.close();
    }

    void save() {
        createDir();
        std::fstream file{path, std::ios::out | std::ios::trunc};
        file << maxMemory << "\n";
        file.close();
    }
};