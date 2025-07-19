#include "CLI.hpp"
#include "../PlaylistManager/PlaylistManager.hpp"
#include "../AudioEngine/AudioEngine.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

void CLI::run() {
    PlaylistManager playlistManager;
    AudioEngine audioEngine;
    std::string input;
    std::cout << "> ";
    while (std::getline(std::cin, input) && input != "q" && input != "quit") {
        std::stringstream ss(input);
        std::string command;
        ss >> command;

        if (command == "load") {
            std::string filename;
            ss >> filename;
            playlistManager.load(filename);
            std::cout << "Playlist '" << filename << "' loaded." << std::endl;
        } else if (command == "play") {
            std::string currentSong = playlistManager.getNextFile();
            if (!currentSong.empty()) {
                audioEngine.play(currentSong);
            } else {
                std::cout << "No songs in playlist." << std::endl;
            }
        } else if (command == "next") {
            std::string nextSong = playlistManager.getNextFile();
            if (!nextSong.empty()) {
                audioEngine.play(nextSong);
            } else {
                std::cout << "No more songs in playlist." << std::endl;
            }
        } else if (command == "previous") {
            std::string previousSong = playlistManager.getPreviousFile();
            if (!previousSong.empty()) {
                audioEngine.play(previousSong);
            } else {
                std::cout << "No previous songs in playlist." << std::endl;
            }
        } else if (command == "pause") {
            audioEngine.pause();
        } else if (command == "stop") {
            audioEngine.stop();
        } else if (command == "save") {
            std::string filename;
            ss >> filename;
            playlistManager.save("../" + filename);
        } else if (command == "volume") {
            float vol;
            ss >> vol;
            audioEngine.setVolume(vol / 100.0f);
        } else if (command == "getvolume") {
            std::cout << "Current volume: " << (int)(audioEngine.getVolume() * 100) << "%" << std::endl;
        } else if (command == "shuffle") {
            playlistManager.shuffle();
        } else if (command == "repeat") {
            std::string enableStr;
            ss >> enableStr;
            if (enableStr == "on") {
                playlistManager.repeat(true);
            } else if (enableStr == "off") {
                playlistManager.repeat(false);
            } else {
                std::cout << "Usage: repeat [on|off]" << std::endl;
            }
        } else if (command == "info") {
            SongInfo info = audioEngine.getCurrentSongInfo();
            std::cout << "Title: " << info.title << std::endl;
            std::cout << "Artist: " << info.artist << std::endl;
            std::cout << "Album: " << info.album << std::endl;
            std::cout << "Duration: " << info.duration << " seconds" << std::endl;
        } else if (command == "help") {
            std::cout << "Available commands:" << std::endl;
            std::cout << "  load <filename> - Load a playlist from a file." << std::endl;
            std::cout << "  save <filename> - Save the current playlist to a file." << std::endl;
            std::cout << "  play            - Start playing the current song." << std::endl;
            std::cout << "  pause           - Pause the current song." << std::endl;
            std::cout << "  stop            - Stop the current song." << std::endl;
            std::cout << "  next            - Play the next song in the playlist." << std::endl;
            std::cout << "  previous        - Play the previous song in the playlist." << std::endl;
            std::cout << "  volume <0-100>  - Set the playback volume." << std::endl;
            std::cout << "  getvolume       - Get the current playback volume." << std::endl;
            std::cout << "  shuffle         - Shuffle the current playlist." << std::endl;
            std::cout << "  repeat [on|off] - Enable or disable playlist repeat." << std::endl;
            std::cout << "  list            - List songs in the current playlist." << std::endl;
            std::cout << "  info            - Display information about the current song." << std::endl;
            std::cout << "  q or quit       - Exit the player." << std::endl;
        } else if (command == "list") {
            std::cout << "Current playlist:" << std::endl;
            for (const auto& file : playlistManager.getFiles()) {
                std::cout << "- " << file << std::endl;
            }
        } else {
            std::cout << "Command: '" << command << "' not implemented yet." << std::endl;
        }
        std::cout << "> ";
    }
    std::cout << "Exiting." << std::endl;
}
