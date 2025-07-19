# GEMINI2.md - Development Log and Lessons Learned

This document summarizes the development process of the console music player, highlighting key steps, challenges encountered, and their resolutions.

## 1. Initial Project Setup

The project was initialized based on the `GEMINI.md` design document.

### 1.1 Directory Structure Creation

The following directory structure was created:
```bash
mkdir -p src/AudioEngine src/PlaylistManager src/CLI src/Utils include lib build doc tests resources
```

### 1.2 Initial File Creation

The following core files were created with initial content:
*   `README.md`
*   `LICENSE.md`
*   `src/main.cpp` (initially "Hello, World!")
*   `CMakeLists.txt` (initial basic setup)

### 1.3 Git Initialization

A Git repository was initialized, and initial files were committed.
```bash
git init
git add .
git commit -m "Initial project setup: Added README, GEMINI, LICENSE, and basic structure"
```

### 1.4 .gitignore Creation

A `.gitignore` file was created to exclude build artifacts and other unnecessary files from version control.
```gitignore
# Build artifacts
build/
*.o
*.d
*.so
*.a
*.out
*.exe
# CMake files
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
Makefile
CTestTestfile.cmake
# IDE files (example for VS Code, adjust as needed)
.vscode/
# Operating System generated files
.DS_Store
Thumbs.db
# Editor swap files
*~
*.swp
```

## 2. Build System and Dependency Resolution

Several issues were encountered during the initial build process, primarily related to missing dependencies and `CMakeLists.txt` configuration.

### 2.1 Missing CMake

**Problem:** `cmake` command not found.
**Solution:** User was instructed to install CMake.
**Lesson Learned:** Always verify core build tools are present.

### 2.2 Missing Pkg-Config

**Problem:** `PkgConfig` not found by CMake.
**Solution:** User was instructed to install `pkg-config`.
**Lesson Learned:** `pkg-config` is crucial for CMake to locate libraries.

### 2.3 Missing FFmpeg Development Libraries

**Problem:** `libavformat`, `libavcodec`, `libavutil` not found.
**Solution:** User was instructed to install FFmpeg development libraries.
**Lesson Learned:** Ensure all required library development packages are installed.

### 2.4 Missing PortAudio Development Libraries

**Problem:** `portaudio-2.0` not found.
**Solution:** User was instructed to install PortAudio development libraries. An initial attempt with `libportaudio-dev` failed, and the correct package `portaudio19-dev` was suggested.
**Lesson Learned:** Package names can vary between distributions; `apt-cache search` is useful for finding correct names.

### 2.5 `CMakeLists.txt` Source File Inclusion Issue

**Problem:** Linker error "undefined reference to `main`" after restructuring `src` directory. `file(GLOB_RECURSE SOURCES "src/*.cpp")` was not correctly including files from subdirectories.
**Solution:** Changed `file(GLOB_RECURSE ...)` to explicitly list all source files.
**Code Snippet (CMakeLists.txt fix):**
```cmake
# Add source files
set(SOURCES
    src/main.cpp
    src/AudioEngine/AudioEngine.cpp
    src/CLI/CLI.cpp
    src/PlaylistManager/PlaylistManager.cpp
)
```
**Lesson Learned:** `file(GLOB_RECURSE)` can be unreliable for source file discovery; explicit listing is more robust.

### 2.6 `libswresample` Linking Error and Deprecated FFmpeg APIs

**Problem:** `undefined reference to symbol 'swr_alloc_set_opts'` and deprecated API warnings.
**Solution:**
1.  Added `libswresample` to `pkg_check_modules` in `CMakeLists.txt`.
2.  Updated `AudioEngine.cpp` to use modern FFmpeg APIs for `swr_alloc_set_opts` and channel layout handling.
**Code Snippet (CMakeLists.txt update):**
```cmake
pkg_check_modules(FFMPEG REQUIRED libavformat libavcodec libavutil libswresample)
```
**Code Snippet (AudioEngine.cpp relevant changes for modern FFmpeg API):**
```cpp
#include <libavutil/channel_layout.h> // New include

// ... inside AudioEngine::play() ...
    data.swrContext = swr_alloc();
    AVChannelLayout out_ch_layout = AV_CHANNEL_LAYOUT_STEREO;
    av_opt_set_chlayout(data.swrContext, "in_chlayout", &data.codecContext->ch_layout, 0);
    av_opt_set_chlayout(data.swrContext, "out_chlayout", &out_ch_layout, 0);
    av_opt_set_int(data.swrContext, "in_sample_rate", data.codecContext->sample_rate, 0);
    av_opt_set_int(data.swrContext, "out_sample_rate", data.codecContext->sample_rate, 0);
    av_opt_set_sample_fmt(data.swrContext, "in_sample_fmt", data.codecContext->sample_fmt, 0);
    av_opt_set_sample_fmt(data.swrContext, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);
    swr_init(data.swrContext);
```
**Lesson Learned:** Keep FFmpeg API usage up-to-date and ensure all necessary libraries are linked.

### 2.7 Syntax Error in `AudioEngine.cpp`

**Problem:** Accidental inclusion of markdown triple quotes (`'''`) in the C++ code, leading to compilation errors.
**Solution:** Removed the erroneous characters.
**Lesson Learned:** Be extremely careful with copy-pasting and escaping, especially when dealing with code blocks in markdown.

## 3. Core Component Implementation

### 3.1 CLI Component

*   **Initial `main.cpp` integration:** `main` function now creates and runs a `CLI` object.
*   **Command Loop:** Implemented a basic command loop in `CLI::run()` to read user input.
*   **`load` command:** Added handling for a `load <filename>` command to interact with `PlaylistManager`.
*   **`play` command:** Added handling for a `play` command to interact with `AudioEngine`.
*   **`previous` command:** Added handling for a `previous` command to play the previous song in the playlist.
*   **`volume` and `getvolume` commands:** Added commands to set and retrieve the playback volume.
*   **`shuffle` command:** Added command to shuffle the playlist.
*   **`repeat` command:** Added command to enable/disable playlist repeat.
*   **`info` command:** Added command to display current song metadata.
*   **`help` command:** Added a command to display available commands and their usage.

### 3.2 PlaylistManager Component

*   **`load` method:** Implemented to parse `.m3u` files, skipping empty lines and comments (`#`).
*   **`addFile`, `getNextFile`, `getPreviousFile`:** Basic implementations for playlist management.
*   **`save` method:** Implemented to save the current playlist to a `.m3u` file.
*   **`shuffle` method:** Implemented to randomize the order of songs in the playlist.
*   **`repeat` method:** Implemented to control playlist repetition.

### 3.3 AudioEngine Component

*   **Initial (incorrect) `libsndfile` implementation:** Briefly used `libsndfile` before realizing it didn't support compressed formats.
*   **FFmpeg and PortAudio Integration:** Replaced `libsndfile` with FFmpeg for decoding and PortAudio for playback. This involved:
    *   Opening audio files with `avformat_open_input`.
    *   Finding audio streams and decoders.
    *   Setting up `AVCodecContext` and opening the codec.
    *   Initializing `SwrContext` for audio resampling (converting decoded audio to a format suitable for PortAudio).
    *   Setting up PortAudio stream with a callback function to feed decoded audio.
    *   Basic playback loop using `av_read_frame`, `avcodec_send_packet`, `avcodec_receive_frame`, and `swr_convert`.
*   **Volume Control:** Implemented volume adjustment during playback.
*   **Metadata Parsing:** Integrated FFmpeg's metadata extraction to retrieve title, artist, album, and duration.

## 4. Testing and Verification

Throughout the process, the following commands were frequently used to build and test:
```bash
cd build && cmake .. && make
cd build && make && ./musix
echo "command" | cd build && make && ./musix
```

## 5. Lessons Learned and Refinements

*   **CMake `file(GLOB_RECURSE)` unreliability:** Explicitly listing source files in `CMakeLists.txt` is more robust.
*   **FFmpeg API evolution:** Newer FFmpeg versions deprecate older APIs; it's important to use the latest recommended functions (e.g., `av_channel_layout_num_channels` vs `av_get_channel_layout_nb_channels` vs direct `ch_layout.nb_channels` access).
*   **Header vs. Source file definitions:** Structs and their members should be defined in headers, while their implementations and related FFmpeg includes belong in the source files to avoid redefinition errors.
*   **Error Handling:** Consistent error reporting (e.g., `std::cerr`) is crucial for debugging.
*   **Path Handling:** When running executables from a build directory, relative paths need careful consideration (e.g., prepending `../` for project root access).
*   **Testing with limited environment:** Even without a sound card, testing CLI commands and internal logic can be done by observing output and error messages.

This log serves as a record of the development journey, highlighting the iterative process of coding, testing, debugging, and refining the project.