
# ORC recorder

This program is to record the text on selected region of screen or an application (esp. a video player) for Windows.  

Most other orc applications grab text on screen and store it to clipboard, so ORC recorder is made to simiplify the procedure. Using ORC recorder, we can record the subtitles in a video with just one hotkey. Furthermore, ORC recorder provides the option to specify the character color so that the precision of ocr recognition can be improved in specific cases.

## How to Install

[GitHub Releases](https://github.com/bioxun/ocr_recorder/releases/latest)

## How to use



## Build Instructions

Get the code:

- git clone https://github.com/bioxun/ocr_recorder.git

### Prerequisites

- CMake 3.10 or higher

- MinGW

  c++ in MinGW is used to make Dlls (region_selector.dll, dll_ocr_recorder.dll). 

- Packages in MinGW
  mingw-w64-x86_64-toolchain
  mingw-w64-x86_64-gcc
  mingw-w64-x86_64-tesseract-ocr
  mingw-w64-x86_64-opencv(>=4.5.5 and <4.12.0)
  mingw-w64-x86_64-leptonica
  mingw-w64-x86_64-lua 

- AutoHotKey v1
AutoHotkey v1 is required to build the main executable file, which calls the Dlls (region_selector.dll, dll_ocr_recorder.dll). 

- traineddata of tesseract

  Tesseract is the OCR engine of OCR recorder, so traineddata of tesseract is required, and it can be found at https://tesseract-ocr.github.io/tessdoc/Data-Files.html .

- specify the right path of AutoHotkey in CMakeLists.txt

- specify the right path of tessdata in lua files (dll_ocr_recorder.lua, ocr_recorder.lua, test_dll_ocr_recorder.lua). 

### Build Steps
```bash
# Configure project
cmake -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release -S . -B ./build/release -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=install/release

# enter the build directory
cd build/release

# Build project
cmake --build . --target all --

# Run executable
./test_dll_ocr_recorder.exe
./ocr_screen.exe

```



## License

MIT License
