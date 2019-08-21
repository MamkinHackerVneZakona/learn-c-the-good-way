//
// Created by Ruslan Feizerakhmanov on 2019-07-22.
//

#ifndef LEARN_C_THE_GOOD_WAY_UTIL_H
#define LEARN_C_THE_GOOD_WAY_UTIL_H

#include <cstdio>
#include <cstdlib>

#include <glad/glad.h>
#include <iostream>

#define __max__(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

void readFile(const char *name, char *res);

int loadProgram(const char *vertLoc, const char *fragLoc);

#endif //LEARN_C_THE_GOOD_WAY_UTIL_H
