#pragma once

#include "colors.h"
#include <stdio.h>

void initializeColors();
void initializeFormats();

int perrorf(const char *restrict format, ...);
int pwarnf(const char *restrict format, ...);