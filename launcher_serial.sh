#!/bin/bash

python3 procesarFichero.py
g++ -fopenmp geneticoSimple_serial.cpp -o geneticoSimple_serial -lm
./geneticoSimple_serial
