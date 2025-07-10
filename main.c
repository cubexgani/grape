#include <stdio.h>
#include <ctype.h>
#include "colors.h"
#include <string.h>
#include <stdlib.h>
#include "rangelist.h"
#include <regex.h>
#define string char*

enum FLAGS {
    FIXED =             1 << 0,
    BASIC_REGEX =       1 << 1,
    EXTENDED_REGEX =    1 << 2,
    IGNORE_CASE =       1 << 3
};


void grape(FILE *, string, char);
void grape_fixed(FILE *, string, char);
void grape_regex(FILE *, string, char);
int parse_flags(char *, string);
int displayFinds(string, RangeList *);
void warning(string);
void error(string);

int main(int argc, char** argv) {
    // An 8 (effectively 7) bit character for storing flags as bits
    char flags;
    int idx = 1;
    // Remember kids, argv[argc] == NULL.
    // And the number of flags in input are variable.
    for (; idx < argc; idx++) {
        char *str = argv[idx];
        if (str[0] != '-') break;   // no more flags
        if (str[1] == '-' && str[2] == '\0') {
            idx++;
            break;   // end of flags --
        }
        int status = parse_flags(&flags, str + 1);
        if (status == -1) return 0;
    }

    string substring = argv[idx++];
    if (substring == NULL) {
        printf(ANSI_COLOR_YELLOW "Usage: %s [-EFGi] pattern [filename]" ANSI_COLOR_RESET "\n", argv[0]);
        return 0;
    }
    FILE *fp = argv[idx] ? fopen(argv[idx], "r") : stdin;
    if (fp == NULL) {
        printf(ANSI_COLOR_RED "Where the hell is %s twin\n" ANSI_COLOR_RESET, argv[idx]);
    }
    grape(fp, substring, flags);
}

void grape(FILE *fp, string substr, char flags) {
    char fixed = flags & FIXED;
    char regex = flags & BASIC_REGEX;
    if (fixed && regex) {
        error("2 conflicting options set at once");
    }
    else if (fixed) {
        grape_fixed(fp, substr, flags);
    }
    else {
        grape_regex(fp, substr, flags);
    }
}

void grape_fixed(FILE *fp, string substr, char flags) {
    char ignoreCase = flags & IGNORE_CASE;  // Will be either 0 or some non 0 value
    int sublen = strlen(substr);
    RangeList *ranges = malloc(sizeof(RangeList));
    ranges->head = NULL;
    char line[400];
    while ((fgets(line, 400, fp)) != NULL) {
        // yes 400 is a reasonable limit, I know
        int sind = 0, eind = 0, currInd = 0;
        Range r;
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
                // Prolly gonna use either the above or the below one
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
        // Print the current line with highlighting and all
        int sts = displayFinds(line, ranges);
        if (!sts) continue;
        // Empty the linked list before next line rolls in
        clear(ranges);
    }
    free(ranges);
}

void grape_regex(FILE *fp, string toMatch, char flags) {
    regex_t compiled;
    int extended = flags & EXTENDED_REGEX;
    int ignorecase = flags & IGNORE_CASE;
    int perms = extended && ignorecase ? REG_EXTENDED | REG_ICASE : 
                extended ? REG_EXTENDED : ignorecase ? REG_ICASE : 0;
    
    int compErr = regcomp(&compiled, toMatch, perms);
    if (compErr) {
        char errbuf[100];
        regerror(compErr, &compiled, errbuf, sizeof(errbuf));
        printf(ANSI_COLOR_RED "%s\n" ANSI_COLOR_RED, errbuf);
        return;
    }
    long subexprs = compiled.re_nsub;

    RangeList *ranges = malloc(sizeof(RangeList));
    ranges->head = NULL;
    char line[400];
    while(fgets(line, 400, fp) != NULL) {
        char *lptr = line;
        // Ok this one is more of a char pointer than a string sooo
        
        int matchSts = 0; //assuming no match error occurs
        
        // final offset of range wrt beginning of line
        int finOff = 0;
        while(*lptr != '\0') {
            regmatch_t matches[subexprs + 1];
            // execute matching on current string scope
            matchSts = regexec(&compiled, lptr, subexprs + 1, matches, 0);
            if (matchSts == REG_NOMATCH) break;

            // offset of range wrt string scope lptr
            int currOff = 0;
            for (long i = 0; i <= subexprs; i++) {
                Range r;
                
                r.startInd = finOff + matches[i].rm_so;
                r.endInd = finOff + matches[i].rm_eo - 1;
                add(r, ranges);
                currOff = matches[i].rm_eo;
            }
            lptr = lptr + currOff;
            finOff += currOff;
            // well now the indices are in sorted order
        }
        int sts = displayFinds(line, ranges);
        if (!sts) continue;
        clear(ranges);

    }
    
    free(ranges);
    regfree(&compiled);
}

int displayFinds(string line, RangeList *ranges) {
    if (ranges->head == NULL) return 0;
    char *line_temp = line;
    int pos = 0;
    Node *temp = ranges->head;
        
    while (temp) {
        Range curr = temp->range;
        int start = curr.startInd, end = curr.endInd, plaintextDistance = start - pos;

        printf("%.*s" ANSI_COLOR_MAGENTA "%.*s" ANSI_COLOR_RESET, 
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

int parse_flags(char *flags, string content) {
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
            default:
                printf(ANSI_COLOR_RED "Invalid option: %c\n" ANSI_COLOR_RESET, ch);
                return -1;
        }
    }
    return 0;
}


void warning(string warnstr) {
    printf(ANSI_COLOR_YELLOW "%s\n" ANSI_COLOR_RESET, warnstr);
}
void error(string errstr) {
    printf(ANSI_COLOR_RED "%s\n" ANSI_COLOR_RESET, errstr);
}