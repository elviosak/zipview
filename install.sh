#!/bin/bash

cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr && cmake --build build -- -j$(nproc) && sudo cmake --install build
