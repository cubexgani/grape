#include "../include/display.h"

#include "../include/colors.h"
#include "../include/textformat.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Can't really decide between whether this should be a macro or a function lol
#define SHOWLINE if (showLineNum) printf(ANSI_COLOR_BLUE "%d" ANSI_COLOR_RESET ": ", lineNum);

int displayFindLines(char *line, RangeList *ranges, unsigned short showLineNum, int lineNum, 
                        char showFileName, char *fileName) {
    if (ranges->head == NULL) return 1;
    if (!displayFileNames(showFileName, fileName)) printf(": ");
    SHOWLINE
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
    // Seek to EOL
    while(*line_temp != '\0') line_temp++;
    if (*(line_temp - 1) != '\n') printf("\n");     // Print newline if there's no newline in original line
    return 0;
}

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

        printf(ANSI_COLOR_MAGENTA BOLD UNDERLINE "%.*s\n" ANSI_COLOR_RESET, 
            end - start + 1, line + start);
        temp = temp->next;
    }
    return 0;
}
int displayFileNames(char showFileName, char *fileName) {
    if (!showFileName) return 1;
    printf(ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET, fileName);
    return 0;
}