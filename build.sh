#!/bin/bash
cd $(dirname "$BASH_SOURCE[0]")

mkdir -p bin
gcc -o bin/main src/main.cpp && bin/main