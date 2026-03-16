
# ORC recorder

This program is to record the text on selected region of screen or an application (esp. a video player) for Windows.  

Most other orc applications grab text on screen and store it to clipboard, so when we want to record the subtitles in a video, we have to




## Project Structure
```
cpp-cmake-demo/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── src/
│   └── basic_funcs_for_ocr_screen.hpp, dll_ocr_screen.cpp, test_dll_ocr_screen.cpp, 
        dll_ocr_screen.lua, ocr_screen.patterns.config, ocr_screen.words.config, ocr_tesser.config, ocr_screen_v1.1.v1ahk
    └── Images/
        └── example08.png
├── include/
│   └── 
└── lib/
    └── 

```

## Prerequisites for building the project
```
This project is tested in windows 10 with MinGW. 
mingw-w64-x86_64-toolchain ：
    mingw-w64-x86_64-gcc（15.1.0-8） 
    mingw-w64-x86_64-crt-git（13.0.0.r107.ge07d54f4b-1）gdiplus gdi32 ole32 uxtheme urlmon shlwapi
mingw-w64-x86_64-tesseract-ocr 5.5.1-3
mingw-w64-x86_64-opencv 4.5.5
mingw-w64-x86_64-leptonica 1.85.0-1
lua （5.3.4 和 5.4.8-1）
-------------------
AutoHotkey （v1.1.34.04）
-------------------

-------------------


```

## Prerequisites for running the built program
```
tesseract traineddata
https://tesseract-ocr.github.io/tessdoc/Data-Files.html

```

## Build Instructions

### Prerequisites
- CMake 3.10 or higher
- C++17 compatible compiler

### Build Steps
```bash
# Configure project
cmake -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release -S . -B ./build/release -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=install/release

# enter the build directory
cd build/release

# Build project
cmake --build . --target all --

# Run executable
./test_dll_ocr_screen.exe
./ocr_screen_v1.1.exe

# Install program
cmake --build . --target install


```

## License
MIT License
