# zipview

Simple manga/comic book reader with continuous scrolling.

Opens `*.zip` and `*.cbz` files.

## Usage

Click "Open" button to browse and select a file, drop a file in the program window or with cli:
```
zipview "/path/to/file.zip"
```

Scroll with mouse wheel or click to focus the image and use Up/Down arrow keys or Page Up/Page Down.

![video](examples/video.mp4)

## Requirements
- CMake
- base-devel (gcc, make)
- Git
- Qt5
- [QuaZip](https://github.com/stachenov/quazip)

### Archlinux
```bash
sudo pacman --needed -S base-devel cmake git qt5-base quazip
```

## Installation
```bash
git clone https://github.com/slidinghotdog/zipview.git
mkdir -p zipview/build
cd zipview/build
cmake .. && make -j$(nproc) && sudo make install
```



## [Pepper & Carrot](https://www.peppercarrot.com/) included in examples folder (episode 1)

Open source comic licensed under [Creative Commons Attribution 4.0 International license](https://creativecommons.org/licenses/by/4.0/)

### Attribution to:

Art: David Revoy.
Scenario: David Revoy.
English (original version) Proofreading: Alex Gryson, Amireeti.


### Credit for the universe of Pepper&Carrot:

Creation: David Revoy.
Lead Maintainer: Craig Maloney.
Writers: Craig Maloney, Nicolas Artance, Scribblemaniac, Valvin.
Correctors: Alex Gryson, CGand, Hali, Moini, Willem Sonke.