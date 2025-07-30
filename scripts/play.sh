#! /usr/bin/bash

g++ play.cpp models/jogo.cpp -o jogo -lncurses
./jogo

rm -f jogo