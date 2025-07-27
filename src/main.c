#include <stdio.h>
#include <ctype.h>
#include "../include/colors.h"
#include <string.h>
#include <stdlib.h>
#include "../include/rangelist.h"
#include <regex.h>
#include "../include/flags.h"
#include "../include/textformat.h"
#include "../include/logging.h"

/* A macro for displaying the lines (matching or not matching depending upon the flag)
or just not doing anything if we just want the find count */
#define DISPLAY(lchar) \
if (!dispCount) { \
    if (invertMatch && ranges->head == NULL) { \
        if (showFileName) \
            printf(ANSI_COLOR_GREEN "%s: " ANSI_COLOR_RESET, fileName); \
        printf("%s", line); \
        if (lchar != '\n') printf("\n"); \
        continue; \
    } \
    else if (!invertMatch) { \
        int sts = displayFinds(line, ranges, flags, lineNum, showFileName, fileName); \
        if (!sts) continue; \
    } \
}


int grape(char **, int, char *, unsigned char);
int grape_fixed(FILE *, char *, unsigned char, char, char *);
int grape_regex(FILE *, char *, unsigned char, char, char *);
int displayFinds(char *, RangeList *, unsigned char, int, char, char *);

int main(int argc, char** argv) {
    // An 8 (effectively 7) bit character for storing flags as bits
    unsigned char flags;
    int idx = 1;
    // Remember kids, argv[argc] == NULL.
    // And the number of flags in input are variable.
    for (; idx < argc; idx++) {
        char *str = argv[idx];
        if (str[0] != '-') break;   // no more flags
        if (str[0] == '-' && str[1] == '\0') break;     //substring -
        if (str[1] == '-' && str[2] == '\0') {
            idx++;
            break;   // end of flags --
        }
        int status = parse_flags(&flags, str + 1);
        if (status == -1) return 0;
    }
    if (flags & HELP) {
        printHelpText(flags);
        return 0;
    }

    char *substring = argv[idx++];
    if (substring == NULL) {
        printHelpText(flags);
        return 0;
    }
    FILE *fp = argv[idx] ? fopen(argv[idx], "r") : stdin;
    if (fp == NULL) {
        printf(ERROR("Where the hell is %s twin"), argv[idx]);
        return -1;
    }
    grape(argv + idx, argc - idx, substring, flags);
}

int grape(char *files[], int filesLen, char *substr, unsigned char flags) {
    char fixed = flags & FIXED;
    char regex = flags & BASIC_REGEX;
    if (fixed && regex) {
        printf(ERROR("2 conflicting options set at once"));
        return 1;
    }
    int (*grapeFn)(FILE *, char *, unsigned char, char, char *) = fixed ? grape_fixed : grape_regex;
    // For some reason I'm suddenly thinking a lot about memory usage
    char showFileName = filesLen <= 1 ? 0 : 1;
    if (filesLen == 0) {
        FILE *fp = stdin;
        grapeFn(fp, substr, flags, showFileName, "(standard input)");
        return 0;
    }
    for (int i = 0; i < filesLen; i++) {
        FILE *fp;
        char *filename;

        // For more expressive code, I had to include the != 0 part
        if (strcmp(files[i], "-") != 0) {
            fp = fopen(files[i], "r");
            filename = files[i];
        }
        else {
            fp = stdin;
            filename = "(standard input)";
        }
        if (fp == NULL) {
            printf(ERROR("Where the hell is %s twin"), files[i]);
            return -1;
        }
        grapeFn(fp, substr, flags, showFileName, filename);
    }
    return 0;
}

int grape_fixed(FILE *fp, char *substr, unsigned char flags, char showFileName, char *fileName) {
    int lineNum = 0;
    int findSum = 0;
    char ignoreCase = flags & IGNORE_CASE;  // Will be either 0 or some non 0 value
    char dispCount = flags & FIND_COUNT;
    char invertMatch = flags & INVERT_MATCH;

    int sublen = strlen(substr);
    
    RangeList *ranges = malloc(sizeof(RangeList));
    ranges->head = NULL;
    ranges->len = 0;
    
    // An 8kb buffer might be overkill, idk
    char line[BUFSIZ];
    while ((fgets(line, BUFSIZ, fp)) != NULL) {
        // yes 400 is a reasonable limit, I know
        int sind = 0, eind = 0, currInd = 0;
        Range r;
        lineNum++;
        // start and end indices of substring, and current index in substring
        int i;
        for (i = 0; line[i] != '\0'; i++) {
            /*
            2 pointers, for line and for substring, move together as long as the characters at
            their current positions are equal. If different, the substring index gets reset, and 
            compared again
            */
            if (substr[currInd] == '\0') {
                eind = i - 1;
                sind = eind - sublen + 1;
                r.startInd = sind;
                r.endInd = eind;
                add(r, ranges);
                currInd = 0;
            }
            char line_ch = ignoreCase ? tolower(line[i]) : line[i];
            char pat_ch = ignoreCase ? tolower(substr[currInd]) : substr[currInd];
            if (line_ch == pat_ch) currInd++;
            else {
                currInd = 0;
                pat_ch = ignoreCase ? tolower(substr[currInd]) : substr[currInd];
                if (line_ch == pat_ch) currInd++;
            }
        }
        if (currInd == sublen) {
            Range rng = {i - 1 - sublen, i - 1};
            add(rng, ranges);
        }
        
        DISPLAY(line[i - 1]);

        findSum += ranges->len;
        // Empty the linked list before next line rolls in
        clear(ranges);
    }
    if (dispCount) printf("Count: %d\n", findSum);
    free(ranges);
    return 0;
}

int grape_regex(FILE *fp, char *toMatch, unsigned char flags, char showFileName, char *fileName) {
    regex_t compiled;

    char extended = flags & EXTENDED_REGEX;
    char ignorecase = flags & IGNORE_CASE;
    char dispCount = flags & FIND_COUNT;
    char invertMatch = flags & INVERT_MATCH;
    int perms = extended && ignorecase ? REG_EXTENDED | REG_ICASE : 
                extended ? REG_EXTENDED : ignorecase ? REG_ICASE : 0;
    
    int compErr = regcomp(&compiled, toMatch, perms);
    if (compErr) {
        char errbuf[100];
        regerror(compErr, &compiled, errbuf, sizeof(errbuf));
        printf(ERROR("%s"), errbuf);
        return 1;
    }
    long subexprs = compiled.re_nsub;

    RangeList *ranges = malloc(sizeof(RangeList));
    ranges->head = NULL;
    ranges->len = 0;
    char line[BUFSIZ];
    int lineNum = 0;
    int findSum = 0;
    while(fgets(line, BUFSIZ, fp) != NULL) {
        char *lptr = line;
        
        int matchSts = 0; //assuming no match error occurs
        
        // final offset of range wrt beginning of line
        int finOff = 0;
        lineNum++;
        while(*lptr != '\0') {
            regmatch_t matches[subexprs + 1];
            // execute matching on current char *scope
            matchSts = regexec(&compiled, lptr, subexprs + 1, matches, 0);
            if (matchSts == REG_NOMATCH) break;

            // offset of range wrt char *scope lptr
            int currOff;
            Range r;
            
            r.startInd = finOff + matches[0].rm_so;
            r.endInd = finOff + matches[0].rm_eo - 1;
            add(r, ranges);
            currOff = matches[0].rm_eo;
            lptr = lptr + currOff;
            finOff += currOff;
            // well now the indices are in sorted order
        }
        
        while (*lptr != '\0') lptr++;
        DISPLAY(*(lptr - 1));
        
        findSum += ranges->len;
        clear(ranges);

    }
    if (dispCount) printf("Count: %d\n", findSum);
    free(ranges);
    regfree(&compiled);
    return 0;
}

int displayFinds(char *line, RangeList *ranges, unsigned char flags, int lineNum, char showFilename, char *fileName) {
    if (ranges->head == NULL) return 0;
    if (showFilename) printf(ANSI_COLOR_GREEN "%s: " ANSI_COLOR_RESET, fileName);
    if (flags & SHOW_LINE_NUM) printf(ANSI_COLOR_BLUE "%d: " ANSI_COLOR_RESET, lineNum);
    char *line_temp = line;
    int pos = 0;
    Node *temp = ranges->head;
        
    while (temp) {
        Range curr = temp->range;
        int start = curr.startInd, end = curr.endInd, plaintextDistance = start - pos;

        printf("%.*s" ANSI_COLOR_MAGENTA BOLD UNDERLINE "%.*s" ANSI_COLOR_RESET, 
            plaintextDistance, line_temp, end - start + 1, line_temp + plaintextDistance);
        pos = end + 1;
        line_temp = line + pos;
        temp = temp->next;
    }
    printf("%s", line_temp);
    while(*line_temp != '\0') line_temp++;
    if (*(line_temp - 1) != '\n') printf("\n");
    return 1;
}