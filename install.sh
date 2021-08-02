#!/bin/bash

mkdir -p build
rm -rf build/*
cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && make -j$(nproc) && sudo make install