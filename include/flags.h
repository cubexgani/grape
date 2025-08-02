// Include not more than once
// I could just use include guards by defining macros but I dont want to be unc
#pragma once

enum FLAGS {
    FIXED =                 1 << 0,
    BASIC_REGEX =           1 << 1,
    EXTENDED_REGEX =        1 << 2,
    IGNORE_CASE =           1 << 3,
    SHOW_LINE_NUM =         1 << 4,
    FIND_COUNT =            1 << 5,
    HELP =                  1 << 6,
    INVERT_MATCH =          1 << 7,
    ONLY_MATCHES =          1 << 8,
    ONLY_MATCHING_FILES =   1 << 9,
    ONLY_NON_MATCHING_FILES =   1 << 10,
};

int parse_flags(unsigned short *, char *);
int parseLongFlags(unsigned short *, char *);
void printHelpText(unsigned short);