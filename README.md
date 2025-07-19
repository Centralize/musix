# Console Music Player

A lightweight and efficient console-based music player for Linux, supporting MP3, Ogg Vorbis, AAC, and .m3u playlists.

## Features

* Play MP3, Ogg Vorbis, and AAC audio files.
* Load and manage .m3u playlists.
* Basic playback controls (play, pause, stop, next, previous, volume).
* Display current song information.

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

### Command-line Options (Examples - To be implemented)

  * `-p <path>` or `--play <path>`: Start playing a specific file or playlist.
  * `-v <volume>` or `--volume <volume>`: Set initial volume (0-100).
  * `-l <path>` or `--load <path>`: Load a playlist without starting playback.

### In-Player Commands (Examples - To be implemented)

Once the player is running, you can use single-character commands:

  * `p` or `space`: Play/Pause
  * `s`: Stop
  * `n`: Next track
  * `b`: Previous track
  * `+`: Increase volume
  * `-`: Decrease volume
  * `q` or `Ctrl+C`: Quit
  * `h` or `?`: Show help

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
