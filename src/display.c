#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "display.h"
#include "colors.h"
#include "textformat.h"

// Can't really decide between whether this should be a macro or a function lol
#define SHOWLINE if (showLineNum) { \
     printf("%s%d%s: ", BLUE, lineNum, RESET); \
}

// Displays entire lines with the matches highlighted
int displayFindLines(char *line, RangeList *ranges, unsigned short showLineNum, int lineNum, 
                        char showFileName, char *fileName) {
    if (ranges->head == NULL) return 1;

    // If the file name can be displayed, display it. If it is displayed, print a : afterwards
    if (!displayFileNames(showFileName, fileName)) printf(": ");
    SHOWLINE
    char *line_temp = line;
    int pos = 0;
    Node *temp = ranges->head;
        
    while (temp) {
        Range curr = temp->range;
        int start = curr.startInd, end = curr.endInd, plaintextDistance = start - pos;

        printf("%.*s", plaintextDistance, line_temp);
        printf("%s%s%s", MAGENTA, BOLD, UNDERLINE);
        printf("%.*s", end - start + 1, line_temp + plaintextDistance);
        printf("%s", RESET);
        pos = end + 1;
        line_temp = line + pos;
        temp = temp->next;
    }
    printf("%s", line_temp);
    // Seek to EOL
    while(*line_temp != '\0') line_temp++;
    if (*(line_temp - 1) != '\n') printf("\n");     // Print newline if there's no newline in original line
    return 0;
}

// Displays the lines with no match in them
int displayInvert(char *line, unsigned short showLineNum, int lineNum, 
                    char showFileName, char *fileName) {
    int sts = displayFileNames(showFileName, fileName);
    if (!sts) printf(": ");
    SHOWLINE
    printf("%s", line);
    int end = strlen(line) - 1;
    if (line[end] != '\n') printf("\n");
    return 0;
}

// Displays only the matches
int displayFinds(char *line, RangeList *ranges, unsigned short showLineNum, int lineNum,
                    char showFileName, char *fileName) {
    if (ranges->head == NULL) return 1;
    int sts = displayFileNames(showFileName, fileName);
    if (!sts) printf(": ");
    SHOWLINE
    
    Node *temp = ranges->head;
    
    while (temp) {
        Range curr = temp->range;
        int start = curr.startInd, end = curr.endInd;
        printf("%s%s%s", MAGENTA, BOLD, UNDERLINE);
        printf("%.*s\n", end - start + 1, line + start);
        printf("%s", RESET);
        temp = temp->next;
    }
    return 0;
}


int displayFileNames(char showFileName, char *fileName) {
    if (!showFileName) return 1;
    printf("%s%s%s", GREEN, fileName, RESET);
    return 0;
}