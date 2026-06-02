
#ifndef TREE_GRAPH_ASTAR_H
#define TREE_GRAPH_ASTAR_H

/*
___________________________________________________________________________________________________________________________________________________
A* Search — used by routeSearch when there is no direct connection to the target
___________________________________________________________________________________________________________________________________________________
*/

typedef struct AStarNode {
    int city_id;
    int g;      // cost from origin to this node
    int h;      // heuristic estimate to target
    int f;      // g + h
    struct AStarNode *parent;
} AStarNode;

#define ASTAR_MAX (NUMBER_CITIES * NUMBER_CITIES)
typedef struct {
    AStarNode *nodes[ASTAR_MAX];
    int size;
} AStarList;

static AStarNode *createAStarNode(int city_id, AStarNode *parent, int g, int h)
{
    AStarNode *node = malloc(sizeof(AStarNode));
    node->city_id = city_id;
    node->parent  = parent;
    node->g = g;
    node->h = h;
    node->f = g + h;
    return node;
}

static void addToAStarList(AStarList *list, AStarNode *node)
{
    list->nodes[list->size++] = node;
}

static AStarNode *popLowestF(AStarList *list)
{
    int best = 0;
    for (int i = 1; i < list->size; i++)
        if (list->nodes[i]->f < list->nodes[best]->f)
            best = i;

    AStarNode *node = list->nodes[best];
    list->nodes[best] = list->nodes[--list->size]; // fill gap with last element
    return node;
}

static int isInAStarList(AStarList *list, int city_id)
{
    for (int i = 0; i < list->size; i++)
        if (list->nodes[i]->city_id == city_id) return 1;
    return 0;
}

static void freeAStarList(AStarList *list)
{
    for (int i = 0; i < list->size; i++) free(list->nodes[i]);
    list->size = 0;
}

/*
    A* search from origin to target.
    Heuristic: h(city) = minEdge in the whole graph (admissible lower bound),
               h(target) = 0.
    Returns a standalone RoadMap linked list for the found path (leg-relative
    costs, starting at 0), and sets *subChainEnd to its last node.
    Returns NULL if no path exists.
*/
static struct RoadMap *aStarSearch(int origin_city_id, int target_city_id, struct RoadMap **subChainEnd)
{
    // Heuristic: minimum edge weight in the whole matrix (admissible)
    int minEdge = INT_MAX;
    for (int i = 0; i < NUMBER_CITIES; i++)
        for (int j = 0; j < NUMBER_CITIES; j++)
            if (adjacency_matrix[i][j] != 0 && adjacency_matrix[i][j] < minEdge)
                minEdge = adjacency_matrix[i][j];

    #define H(city) ((city) == target_city_id ? 0 : minEdge)

    AStarList openList   = {0};
    AStarList closedList = {0};

    addToAStarList(&openList, createAStarNode(origin_city_id, NULL, 0, H(origin_city_id)));

    AStarNode *finalNode = NULL;

    while (openList.size > 0)
    {
        AStarNode *current = popLowestF(&openList);

        if (current->city_id == target_city_id)
        {
            finalNode = current;
            break;
        }

        addToAStarList(&closedList, current);

        for (int nb = 0; nb < NUMBER_CITIES; nb++)
        {
            if (adjacency_matrix[current->city_id][nb] == 0) continue;
            if (isInAStarList(&closedList, nb))               continue;

            int newG = current->g + adjacency_matrix[current->city_id][nb];
            int newH = H(nb);

            if (!isInAStarList(&openList, nb))
                addToAStarList(&openList, createAStarNode(nb, current, newG, newH));
        }
    }

    #undef H

    if (!finalNode)
    {
        freeAStarList(&openList);
        freeAStarList(&closedList);
        *subChainEnd = NULL;
        return NULL;
    }

    // Reconstruct path into arrays before freeing A* nodes
    int pathLen = 0;
    for (AStarNode *t = finalNode; t != NULL; t = t->parent) pathLen++;

    int pathCities[NUMBER_CITIES];
    int pathG[NUMBER_CITIES];

    AStarNode *t = finalNode;
    for (int i = pathLen - 1; i >= 0; i--)
    {
        pathCities[i] = t->city_id;
        pathG[i]      = t->g;
        t = t->parent;
    }

    freeAStarList(&closedList);
    freeAStarList(&openList);
    free(finalNode);

    // Build standalone RoadMap list (leg-relative costs, origin = 0)
    struct RoadMap *pathHead = NULL;
    struct RoadMap *pathTail = NULL;

    for (int i = 0; i < pathLen; i++)
    {
        struct RoadMap *node = malloc(sizeof(struct RoadMap));
        node->city_id    = pathCities[i];
        node->total_cost = pathG[i];
        node->next       = NULL;

        if (pathHead == NULL) { pathHead = pathTail = node; }
        else                  { pathTail->next = node; pathTail = node; }
    }

    *subChainEnd = pathTail;
    return pathHead;
}

#endif /* TREE_GRAPH_ASTAR_H */
