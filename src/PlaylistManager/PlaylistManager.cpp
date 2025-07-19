#include "PlaylistManager.hpp"
#include <fstream>
#include <iostream>

void PlaylistManager::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open playlist file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        addFile(line);
    }
}

void PlaylistManager::addFile(const std::string& filename) {
    _files.push_back(filename);
}

void PlaylistManager::save(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for saving playlist: " << filename << std::endl;
        return;
    }

    file << "#EXTM3U\n";
    for (const auto& track : _files) {
        // For simplicity, we're not adding #EXTINF tags here.
        // This could be a future enhancement if metadata parsing is implemented.
        file << track << "\n";
    }
    file.close();
    std::cout << "Playlist saved to: " << filename << std::endl;
}

std::string PlaylistManager::getNextFile() {
    if (_files.empty()) {
        return "";
    }
    if (_repeatEnabled && _currentIndex != -1) {
        return _files[_currentIndex];
    }
    _currentIndex = (_currentIndex + 1) % _files.size();
    return _files[_currentIndex];
}

std::string PlaylistManager::getPreviousFile() {
    if (_files.empty()) {
        return "";
    }
    if (_repeatEnabled && _currentIndex != -1) {
        return _files[_currentIndex];
    }
    _currentIndex = (_currentIndex - 1 + _files.size()) % _files.size();
    return _files[_currentIndex];
}

void PlaylistManager::shuffle() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(_files.begin(), _files.end(), g);
    _currentIndex = 0; // Reset current index after shuffling
    std::cout << "Playlist shuffled." << std::endl;
}

void PlaylistManager::repeat(bool enable) {
    _repeatEnabled = enable;
    std::cout << "Repeat " << (enable ? "enabled" : "disabled") << "." << std::endl;
}
