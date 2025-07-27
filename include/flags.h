enum FLAGS {
    FIXED =             1 << 0,
    BASIC_REGEX =       1 << 1,
    EXTENDED_REGEX =    1 << 2,
    IGNORE_CASE =       1 << 3,
    SHOW_LINE_NUM =     1 << 4,
    FIND_COUNT =        1 << 5,
    HELP =              1 << 6,
    INVERT_MATCH =      1 << 7,
};

struct flagTable;

int parse_flags(unsigned char *, char *);
int parseLongFlags(unsigned char *, char *);
void printHelpText(unsigned char);