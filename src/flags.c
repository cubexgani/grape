#include <stdio.h>
#include <string.h>
#include "flags.h"
#include "logging.h"


struct flagTable {
    char shorthand;
    char *longhand;
    char *desc;
    enum FLAGS value;
};

const struct flagTable lookup[] = {
    {'c', "count", "Shows only the number of finds", FIND_COUNT},
    {'E', "extended-regex", "Uses extended regex for finding matches", EXTENDED_REGEX},
    {'F', "fixed-string", "Uses fixed string mode for finding matches", FIXED},
    {'G', "basic-regex", "Uses basic regex mode for finding matches", BASIC_REGEX},
    {'i', "ignore-case", "Ignore case while finding matches", IGNORE_CASE},
    {'n', "line-number", "Shows line number of each line where the regex matches", SHOW_LINE_NUM},
    {'h', "help", "Shows this help text", HELP},
    {'v', "invert-match", "Prints only the non-matching lines", INVERT_MATCH},
    {'o', "only-matching", "Prints only the matches", ONLY_MATCHES},
    {'l', "files-with-matches", "Prints only the files with matches in them", ONLY_MATCHING_FILES},
    {'L', "files-without-match", "Prints only the files without any match", ONLY_NON_MATCHING_FILES},
};

const long tableSize = sizeof(lookup) / sizeof(struct flagTable);

// Parses virtually all kinds of flags
int parse_flags(unsigned short *flags, char *content) {
    // This is a long form flag now
    if (content[0] == '-') return parseLongFlags(flags, content + 1);
    // Othewise, a shorthand flag
    for (int i = 0; content[i] != '\0'; i++) {
        char ch = content[i];
        // Switch case is like a hashmap, better than a loop over the entire table sooooo
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
            case 'c':
                *flags |= FIND_COUNT;
                break;
            case 'h':
                *flags |= HELP;
                break;
            case 'v':
                *flags |= INVERT_MATCH;
                break;
            case 'o':
                *flags |= ONLY_MATCHES;
                break;
            case 'l':
                // If one option is set, unset the other
                *flags &= ~ONLY_NON_MATCHING_FILES;
                *flags |= ONLY_MATCHING_FILES;
                break;
            case 'L':
                *flags &= ~ONLY_MATCHING_FILES;
                *flags |= ONLY_NON_MATCHING_FILES;
                break;
            default:
                perrorf("Invalid option: %c\n", ch);
                return 1;
        }
    }
    return 0;
}

// Need I say more
void printHelpText(unsigned short flags) {
    char warning = !(flags & HELP);

    // Depending on whether the text to be printed is a usage warning,
    // the printing function is selected
    int (*printFn)(const char *restrict, ...) = warning ? pwarnf : printf;

    //First, the usage
    printFn("Usage: grape -[");
    for (int i = 0; i < tableSize; i++) {
        printFn("%c", lookup[i].shorthand);
    }
    printFn("]... PATTERN [FILES]...\n");

    // Gatekeeping the option details
    if (warning) {
        printFn("Use --help or -h to learn more.\n");
        return;
    }

    printf("\nOptions:\n");
    for (int i = 0; i < tableSize; i++) {
        printf("-%c, --%s: %s\n", lookup[i].shorthand, lookup[i].longhand, lookup[i].desc);
    }

}

// Parses only longhand flags
int parseLongFlags(unsigned short *flags, char *content) {
    for (int i = 0; i < tableSize; i++) {
        // If content equals any of the longhands
        if (!strcmp(content, lookup[i].longhand)) {
            *flags |= lookup[i].value;
            return 0;
        }
    }
    perrorf("Invalid option: %s\n", content);
    return 1;
}