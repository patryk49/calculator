#!/bin/bash

g++ main.cpp -o calculator -g \
	-std=c++20 -Iinclude -fno-exceptions \
	-Wall -Wextra -Wno-attributes -Wno-dangling-else \
#	-lsfml-graphics -lsfml-window -lsfml-system
