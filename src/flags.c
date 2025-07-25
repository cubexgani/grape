#include <stdio.h>
#include "../include/flags.h"
#include "../include/colors.h"

int parse_flags(unsigned char *flags, char *content) {
    // I deliberately didnt use string for flags bcs its purpose is entirely different.
    
    for (int i = 0; content[i] != '\0'; i++) {
        char ch = content[i];
        switch(ch) {
            case 'F':
                *flags |= FIXED;
                break;
            case 'G':
                *flags |= BASIC_REGEX;
                break;
            case 'E':
                *flags |= EXTENDED_REGEX;
                break;
            case 'i':
                *flags |= IGNORE_CASE;
                break;
            case 'n':
                *flags |= SHOW_LINE_NUM;
                break;
            default:
                printf(ANSI_COLOR_RED "Invalid option: %c\n" ANSI_COLOR_RESET, ch);
                return -1;
        }
    }
    return 0;
}