#!/bin/bash

python3 procesarFichero.py
g++ -fopenmp geneticoSimple_05.cpp -o geneticoSimple_05 -lm
./geneticoSimple_05
