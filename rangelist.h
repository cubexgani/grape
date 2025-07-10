// To hold the range of the finds
typedef struct {
    int startInd;
    int endInd;
} Range;

// Each node to hold a range
typedef struct node_t {
    Range range;
    struct node_t *next;
} Node;

// A list of them range nodes
typedef struct {
    Node *head;
} RangeList;

void add(Range, RangeList *);
void display(RangeList *);
void clear(RangeList *);