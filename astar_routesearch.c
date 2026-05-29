/*
===================================================================================================================================================
 * A* Search — adapted for adjacency-matrix city graph
 *
 * Key differences from the grid-based A* you found:
 *   - No x,y coordinates  →  city_id replaces Point
 *   - Neighbours come from adjacency_matrix (not 8 fixed directions)
 *   - Edge cost = adjacency_matrix[a][b]  (not always +1)
 *   - Heuristic = global minimum edge weight (admissible: any remaining
 *     hop costs AT LEAST this much, so we never over-estimate)
 *   - Returns struct RoadMap* instead of Point*
 *
 * NOTE: function name A*_algorithm is invalid C  →  use aStarSearch()
===================================================================================================================================================
*/

#include <limits.h>   /* INT_MAX */

/*
___________________________________________________________________________________________________________________________________________________
A* Internal Structures  (only used inside this file, invisible to the rest)
___________________________________________________________________________________________________________________________________________________
*/

/* Working node — lives only during aStarSearch(), freed before it returns */
typedef struct AStarNode {
    int city_id;
    int g;                    /* exact cost from origin to this node       */
    int h;                    /* heuristic estimate: this node → target    */
    int f;                    /* f = g + h  (priority key)                 */
    struct AStarNode *parent; /* back-pointer for path reconstruction      */
} AStarNode;

/* Simple array-based open / closed list.
   Upper bound: in the worst case we add one entry per edge → N*N entries. */
#define ASTAR_MAX (NUMBER_CITIES * NUMBER_CITIES)
typedef struct {
    AStarNode *nodes[ASTAR_MAX];
    int size;
} AStarList;

/*
___________________________________________________________________________________________________________________________________________________
A* Helper Functions
___________________________________________________________________________________________________________________________________________________
*/

/* Allocate and initialise one A* node */
AStarNode *createAStarNode(int city_id, AStarNode *parent, int g, int h)
{
    AStarNode *node = malloc(sizeof(AStarNode));
    node->city_id = city_id;
    node->parent  = parent;
    node->g = g;
    node->h = h;
    node->f = g + h;
    return node;
}

/* Append node pointer to list */
void addToAStarList(AStarList *list, AStarNode *node)
{
    list->nodes[list->size++] = node;
}

/* Remove and return the node with the lowest f value (the A* priority step) */
AStarNode *popLowestF(AStarList *list)
{
    int best = 0;
    for (int i = 1; i < list->size; i++)
        if (list->nodes[i]->f < list->nodes[best]->f)
            best = i;

    AStarNode *node = list->nodes[best];
    list->nodes[best] = list->nodes[--list->size]; /* fill the gap */
    return node;
}

/* Returns 1 if city_id is already in the list, 0 otherwise */
int isInAStarList(AStarList *list, int city_id)
{
    for (int i = 0; i < list->size; i++)
        if (list->nodes[i]->city_id == city_id) return 1;
    return 0;
}

/* Free every node in the list (does NOT follow parent pointers) */
void freeAStarList(AStarList *list)
{
    for (int i = 0; i < list->size; i++) free(list->nodes[i]);
    list->size = 0;
}

/*
___________________________________________________________________________________________________________________________________________________
aStarSearch()  —  the main function
___________________________________________________________________________________________________________________________________________________

 Finds the OPTIMAL (lowest-cost) path from origin_city_id to target_city_id
 using the A* algorithm on the adjacency matrix.

 Heuristic  h(n):
   h = 0                    when n == target  (we are there, no more cost)
   h = minEdge              otherwise
   where minEdge = the smallest non-zero value in the entire adjacency matrix.
   This is ADMISSIBLE: any single remaining hop costs at least minEdge,
   so we never over-estimate, and A* is guaranteed to return the optimal path.

 Parameters:
   origin_city_id   starting city
   target_city_id   destination city
   leg_cost         OUT: total cost of the path found (0 if no path)

 Returns:
   Pointer to the HEAD of a newly allocated RoadMap linked list
   representing the path  origin → ... → target.
   total_cost in each node = cumulative cost from origin (leg-relative,
   starts at 0).  The caller (routeSearch) adds the global initial_cost.
   Returns NULL if no path exists.
___________________________________________________________________________________________________________________________________________________
*/
struct RoadMap *aStarSearch(int origin_city_id, int target_city_id, int *leg_cost)
{
    /* ── Step 1: precompute heuristic constant (min edge in whole matrix) ── */
    int minEdge = INT_MAX;
    for (int i = 0; i < NUMBER_CITIES; i++)
        for (int j = 0; j < NUMBER_CITIES; j++)
            if (adjacency_matrix[i][j] != 0 && adjacency_matrix[i][j] < minEdge)
                minEdge = adjacency_matrix[i][j];

    /* Macro so h(city) is readable inline */
    #define H(city) ((city) == target_city_id ? 0 : minEdge)

    /* ── Step 2: initialise open and closed lists ── */
    AStarList openList   = {0};
    AStarList closedList = {0};

    addToAStarList(&openList,
        createAStarNode(origin_city_id, NULL, 0, H(origin_city_id)));

    AStarNode *finalNode = NULL;

    /* ── Step 3: main A* loop ── */
    while (openList.size > 0)
    {
        /* Always expand the node with the lowest f = g + h */
        AStarNode *current = popLowestF(&openList);

        /* Reached destination → path found */
        if (current->city_id == target_city_id) {
            finalNode = current;
            break;
        }

        /* Mark current as settled */
        addToAStarList(&closedList, current);

        /* Expand neighbours through the adjacency matrix */
        for (int nb = 0; nb < NUMBER_CITIES; nb++)
        {
            if (adjacency_matrix[current->city_id][nb] == 0) continue; /* no edge    */
            if (isInAStarList(&closedList, nb))               continue; /* already done */

            int newG = current->g + adjacency_matrix[current->city_id][nb];
            int newH = H(nb);
            int newF = newG + newH;

            /* Add to open list if not there yet, or if we found a cheaper path */
            if (!isInAStarList(&openList, nb) || newF < current->f)
                addToAStarList(&openList, createAStarNode(nb, current, newG, newH));
        }
    }

    #undef H

    /* ── Step 4: handle no-path case ── */
    if (!finalNode) {
        printf("Warning: aStarSearch found no path from %s to %s\n",
               citiesInfo[origin_city_id].city_name,
               citiesInfo[target_city_id].city_name);
        freeAStarList(&openList);
        freeAStarList(&closedList);
        *leg_cost = 0;
        return NULL;
    }

    /* ── Step 5: reconstruct path into arrays BEFORE freeing A* nodes ── */
    /*    (we must do this first — freeAStarList will destroy the parent chain) */
    int pathLen = 0;
    for (AStarNode *t = finalNode; t != NULL; t = t->parent) pathLen++;

    int pathCities[NUMBER_CITIES]; /* city IDs in order: origin → target */
    int pathG[NUMBER_CITIES];      /* cumulative leg cost at each step    */

    AStarNode *t = finalNode;
    for (int i = pathLen - 1; i >= 0; i--) {
        pathCities[i] = t->city_id;
        pathG[i]      = t->g;
        t = t->parent;
    }

    *leg_cost = finalNode->g;

    /* ── Step 6: free ALL A* working memory ── */
    /*    Order matters: closedList first (contains the parent chain),        */
    /*    then openList (remaining candidates), then finalNode (not in either) */
    freeAStarList(&closedList);
    freeAStarList(&openList);
    free(finalNode);

    /* ── Step 7: build and return RoadMap linked list from path arrays ── */
    struct RoadMap *pathHead = NULL;
    struct RoadMap *pathTail = NULL;

    for (int i = 0; i < pathLen; i++)
    {
        struct RoadMap *node = malloc(sizeof(struct RoadMap));
        node->city_id    = pathCities[i];
        node->total_cost = pathG[i];  /* leg-relative cost (0 at origin) */
        node->next       = NULL;

        if (pathHead == NULL) { pathHead = pathTail = node; }
        else                  { pathTail->next = node; pathTail = node; }
    }

    return pathHead;
}

/*
___________________________________________________________________________________________________________________________________________________
Integration inside routeSearch()  —  replace the else block with this
___________________________________________________________________________________________________________________________________________________

    The direct-connection check (if branch) stays exactly as you had it.
    A* takes over for the else case and finds the FULL remaining path at once,
    so we use continue to skip the single-step addToRoadMap at the bottom
    of the while loop (A* already appended all intermediate nodes).

    Your routeSearch while loop should look like this:

    ─────────────────────────────────────────────────────────────────────────
    while (current_city_id != target_city_id)
    {
        if (adjacency_matrix[current_city_id][target_city_id] != 0)
        {
            // Direct connection: one step, handled by the existing code
            partial_cost   = adjacency_matrix[current_city_id][target_city_id];
            current_city_id = target_city_id;
        }
        else
        {
            // ── A* finds the complete optimal path in one call ──
            int leg_cost = 0;
            struct RoadMap *subPath = aStarSearch(current_city_id,
                                                   target_city_id,
                                                   &leg_cost);
            if (subPath == NULL) return final_cost;  // no path found

            // subPath starts with current_city_id, which is ALREADY
            // in the main road map → skip it and free its node
            struct RoadMap *curr = subPath->next;
            free(subPath);

            // Append the remaining nodes (with globally accumulated cost)
            while (curr != NULL)
            {
                struct RoadMap *nextNode = curr->next;
                final_cost   = initial_cost + curr->total_cost;
                curr->total_cost = final_cost;   // convert leg-relative → global
                curr->next   = NULL;
                (*roadEnd)->next = curr;
                *roadEnd     = curr;
                curr         = nextNode;
            }

            current_city_id = target_city_id;  // tells while to exit
            continue;  // skip the single-step addToRoadMap below
        }

        // Only reached by the direct-connection branch
        final_cost += partial_cost;
        addToRoadMap(roadEnd, current_city_id, final_cost);
    }
    ─────────────────────────────────────────────────────────────────────────
*/
