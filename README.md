Comp308-Final-Project
=====================
Instructions

Linux:
mkdir build
cd build
cmake ..
make
cd ..
./bin/final

Windows (msys/mingw):
mkdir build
cd build
cmake .. -G "MSYS Makefiles"
make
cd ..
copy build\external\zlib\libzlib.dll .
bin\final.exe

The application must be run from the project root due to asset locations