#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

#include "logging.h"
#include "colors.h"

const char *RED, *GREEN, *YELLOW,
            *BLUE, *MAGENTA, *CYAN, *RESET;
const char *BOLD, *UNDERLINE, *DARK;

// Set the color values according to output redirection
void initializeColors() {
    // 1 is the file descriptor of stdout.
    // If stdout is not connected to the terminal, isatty gives 0, else 1
    int isTTY = isatty(1);
    RED = isTTY ?       "\x1b[31m" : "";
    GREEN = isTTY ?     "\x1b[32m" : "";
    YELLOW = isTTY ?    "\x1b[33m" : "";
    BLUE = isTTY ?      "\x1b[34m" : "";
    MAGENTA = isTTY ?   "\x1b[35m" : "";
    CYAN = isTTY ?      "\x1b[36m" : "";
    RESET = isTTY ?     "\x1b[0m" : "";
}

// Set the text format values according to output redirection
void initializeFormats() {
    int isTTY = isatty(1);
    BOLD = isTTY ?          "\x1b[1m" : "";
    UNDERLINE = isTTY ?     "\x1b[4m" : "";
    DARK = isTTY ?          "\x1b[2m" : "";
}

// Display text as error in stderr
int perrorf(const char *restrict format, ...) {
    va_list args;
    fprintf(stderr, "%s", RED);
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "%s", RESET);
    return 0;
}

// Display text as warning in stderr
int pwarnf(const char *restrict format, ...) {
    va_list args;
    fprintf(stderr, "%s", YELLOW);
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "%s", RESET);
    return 0;
}