all:
	cmake -S . -B build
	cmake --build build
	./build/src/Debug/GBA-Emulator.exe

build:
	cmake -S . -B build
	cmake --build build

clean:
	rm -rf build
	
run:
	./build/src/Debug/GBA-Emulator.exe