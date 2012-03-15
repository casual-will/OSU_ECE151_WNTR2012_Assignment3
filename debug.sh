#!/bin/bash
gcc main.c -std=c99 -g -o debug
gdb ./debug
