all:
	cmake -S . -B build
	cmake --build build
	./build/src/Debug/GBA-Emulator.exe

build:
	cmake --build build
	
run:
	./build/src/Debug/GBA-Emulator.exe