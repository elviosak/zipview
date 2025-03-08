PREFIX = /usr

.DEFAULT_GOAL = release

release:
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${PREFIX} -S . -B ./build
	cmake --build ./build --config Release --target all -j$(nproc)

debug:
	cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=${PREFIX} -S . -B ./build-debug
	cmake --build ./build-debug --config Debug --target all -j$(nproc)

clear:
	rm -rf build

clear-debug:
	rm -rf build-debug

all:
	make clear
	make release

install:
	cmake --install ./build