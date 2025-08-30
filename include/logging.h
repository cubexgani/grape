#pragma once

// The colors
extern const char *RED, *GREEN, *YELLOW,
            *BLUE, *MAGENTA, *CYAN, *RESET;

// The typefaces
extern const char *BOLD, *UNDERLINE, *DARK;

void initializeColors();
void initializeFormats();

int perrorf(const char *restrict format, ...);
int pwarnf(const char *restrict format, ...);