#pragma once

#include <string>
#include <vector>
#include <random> // For std::random_device, std::mt19937
#include <algorithm> // For std::shuffle

class PlaylistManager {
public:
    void load(const std::string& filename);
    void save(const std::string& filename);
    
    void addFile(const std::string& filename);
    void removeFile(int index);

    std::string getNextFile();
    std::string getPreviousFile();

private:
    std::vector<std::string> _files;
    int _currentIndex = -1;
    bool _repeatEnabled = false;

public:
    std::vector<std::string> getFiles() const { return _files; }
    void shuffle();
    void repeat(bool enable);
};