# Console Music Player

A lightweight and efficient console-based music player for Linux, supporting MP3, Ogg Vorbis, AAC, and .m3u playlists.

## Features

* Play MP3, Ogg Vorbis, and AAC audio files.
* Load, save, shuffle, and repeat .m3u playlists.
* Full playback controls (play, pause, stop, next, previous, volume).
* Display current song information (title, artist, album, duration).
* Command-line interface with help documentation.

## Building from Source

### Prerequisites

* A C++17 compliant compiler (e.g., GCC, Clang)
* CMake (version X.X or higher)
* **FFmpeg/LibAV Development Libraries:**
    * On Debian/Ubuntu: `sudo apt-get install libavformat-dev libavcodec-dev libavutil-dev`
    * On Fedora/RHEL: `sudo dnf install ffmpeg-devel`
    * On Arch Linux: `sudo pacman -S ffmpeg`
* **PortAudio Development Libraries:**
    * On Debian/Ubuntu: `sudo apt-get install libportaudio2 libportaudio-dev`
    * On Fedora/RHEL: `sudo dnf install portaudio-devel`
    * On Arch Linux: `sudo pacman -S portaudio`

### Steps

1.  **Clone the repository:**
    ```bash
    git clone [https://github.com/your-username/console-music-player.git](https://github.com/your-username/console-music-player.git)
    cd console-music-player
    ```
2.  **Create a build directory and configure CMake:**
    ```bash
    mkdir build
    cd build
    cmake ..
    ```
3.  **Build the project:**
    ```bash
    make
    ```
4.  **Install (optional):**
    ```bash
    sudo make install
    ```
    (This typically installs the executable to `/usr/local/bin` and libraries/headers to appropriate locations.)

## Usage

After building, you can run the player from the `build` directory:

```bash
./console-music-player [options] [path/to/music_file_or_playlist.m3u]
```

### In-Player Commands

Once the player is running, you can use the following commands:

  * `load <filename>`: Load a playlist from a file.
  * `save <filename>`: Save the current playlist to a file.
  * `play`: Start playing the current song.
  * `pause`: Pause the current song.
  * `stop`: Stop the current song.
  * `next`: Play the next song in the playlist.
  * `previous`: Play the previous song in the playlist.
  * `volume <0-100>`: Set the playback volume (0-100%).
  * `getvolume`: Get the current playback volume.
  * `shuffle`: Shuffle the current playlist.
  * `repeat [on|off]`: Enable or disable playlist repeat.
  * `list`: List songs in the current playlist.
  * `info`: Display information about the current song.
  * `help`: Show available commands.
  * `q` or `quit`: Exit the player.

## Project Structure

  * `src/`: Contains all C++ source files.
  * `include/`: Header files.
  * `build/`: Directory for build artifacts.
  * `doc/`: Additional documentation.
  * `tests/`: Unit and integration tests.
  * `CMakeLists.txt`: CMake build configuration.
  * `README.md`: This file.
  * `LICENSE.md`: Project licensing information.
  * `GEMINI.md`: Detailed design and architectural overview.

## Contributing

Contributions are welcome! Please refer to `CONTRIBUTING.md` (if applicable) for guidelines.

## License

This project is licensed under the Creative Commons Attribution-NonCommercial 4.0 International Public License (CC BY-NC 4.0). See the `LICENSE.md` file for details.
