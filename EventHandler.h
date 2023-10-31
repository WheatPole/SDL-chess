#pragma once
#include "Board.h"

#ifndef GLOBAL
#define GLOBAL

extern bool quit;

#endif

void handle(SDL_Event& event, Board& chess);