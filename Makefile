# -S . = Telling source code is in the current folder (root)
# -B = Telling it to build inside the build folder
all:
	cmake -S . -B build
	cmake --build build
	./build/src/Debug/GBA-Emulator.exe

build:
	cmake -S . -B build
	cmake --build build

clean:
	cmake --build build --target clean
	
run:
	./build/src/Debug/GBA-Emulator.exe