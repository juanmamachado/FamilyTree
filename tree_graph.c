/*
===================================================================================================================================================
 * Universitat Autònoma de Barcelona - UAB
 *  Bachelor's Degree in Artificial Intelligence
 *      Fundamentals of Programming II
 *          Practical Project II - Family Tree
 *              Team:
 *                  Name          Surname          NIU
 *                  --------------------------------------
 *                  Javier        Olivé Padrós     1794767
 *                  Juanma        Machado Millan   1797427
===================================================================================================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef MEDIUM
    #include "medium.h"
    #define SIZE_NAME "medium"
#elif defined(LARGE)
    #include "large.h"
    #define SIZE_NAME "large"
#else
    #include "small.h"
    #define SIZE_NAME "small"
#endif

#include "tree_graph_astar.c"

/*
___________________________________________________________________________________________________________________________________________________
Segment log — lightweight linked list of RoadMap* pointers marking where each partial route starts.
The end of segment i is the node just before the start of segment i+1; the last segment ends at roadEnd.
No separate struct needed: we reuse a simple pointer-node.
___________________________________________________________________________________________________________________________________________________
*/

struct PartialRoadMap
{
    struct RoadMap   *start;
    struct PartialRoadMap *next;
};

struct PartialRoadMap *partialRoadMapListStart = NULL;
struct PartialRoadMap *partialRoadMapListEnd   = NULL;

void addToPartialRoadMapList(struct RoadMap *start)
{
    struct PartialRoadMap *mark = malloc(sizeof(struct PartialRoadMap));
    mark->start = start;
    mark->next  = NULL;

    if (partialRoadMapListStart == NULL)
        partialRoadMapListStart = partialRoadMapListEnd = mark;
    else
    {
        partialRoadMapListEnd->next = mark;
        partialRoadMapListEnd       = mark;
    }
}

void deletePartialRoadMapList()
{
    struct PartialRoadMap *cur = partialRoadMapListStart;
    while (cur != NULL)
    {
        struct PartialRoadMap *next = cur->next;
        free(cur);
        cur = next;
    }
    partialRoadMapListStart = NULL;
    partialRoadMapListEnd   = NULL;
}

/*
___________________________________________________________________________________________________________________________________________________
World Journey Graph
___________________________________________________________________________________________________________________________________________________
*/

struct RoadMap* createRoadNode(int city_id, int total_cost)
{
    struct RoadMap *node = malloc(sizeof(struct RoadMap));
    node->city_id    = city_id;
    node->total_cost = total_cost;
    node->next       = NULL;
    return node;
}

void initRoadMap(int startCity, struct RoadMap **roadStart, struct RoadMap **roadEnd)
{
    *roadStart = createRoadNode(startCity, 0);
    *roadEnd   = *roadStart;
}

void addToRoadMap(struct RoadMap **roadEnd, int city_id, int total_cost)
{
    struct RoadMap *node = createRoadNode(city_id, total_cost);
    (*roadEnd)->next = node;
    *roadEnd         = node;
}

// Print each partial route using the segment log.
// The end of each segment is derived from the next segment's start (or roadEnd for the last).
void printPartialRoutes(struct RoadMap *roadEnd)
{
    printf("Partial road map:\n");
    struct PartialRoadMap *mark = partialRoadMapListStart;
    while (mark != NULL)
    {
        struct RoadMap *cur  = mark->start;
        // End is the start of the next segment, or roadEnd if this is the last
        struct RoadMap *stop = (mark->next != NULL) ? mark->next->start : roadEnd;
        int cost = stop->total_cost - cur->total_cost;

        while (cur != stop)
        {
            printf("%s-", citiesInfo[cur->city_id].city_name);
            cur = cur->next;
        }
        printf("%s %d\n", citiesInfo[cur->city_id].city_name, cost);
        mark = mark->next;
    }
}

void printRoadMap(struct RoadMap *roadStart)
{
    struct RoadMap *cur = roadStart;
    printf("Total Road Map:\n");
    while (cur->next != NULL)
    {
        printf("%s-", citiesInfo[cur->city_id].city_name);
        cur = cur->next;
    }
    printf("%s\n", citiesInfo[cur->city_id].city_name);
    printf("Total cost: %d\n", cur->total_cost);
}

void deleteAllRoadMap(struct RoadMap **roadStart, struct RoadMap **roadEnd)
{
    struct RoadMap *cur = *roadStart;
    while (cur != NULL)
    {
        struct RoadMap *next = cur->next;
        free(cur);
        cur = next;
    }
    *roadStart = NULL;
    *roadEnd   = NULL;
}

/*
    Route search from origin to target.
    - Direct connection available → take it in one step.
    - No direct connection → delegate to aStarSearch, then splice its result
      into the main road map (converting leg-relative costs to global costs).
    Appends all intermediate cities to the road map via roadEnd.
    Records the leg in partialRouteLog for later printing.
    Returns the new accumulated total cost.
*/
int routeSearch(int origin, int target, struct RoadMap **roadEnd)
{
    struct RoadMap *legStart = *roadEnd;  // last node before this call (start of this leg visually)

    int initial_cost = (*roadEnd)->total_cost;
    int final_cost   = initial_cost;

    if (adjacency_matrix[origin][target] != 0)
    {
        // Direct connection: single hop
        final_cost += adjacency_matrix[origin][target];
        addToRoadMap(roadEnd, target, final_cost);
    }
    else
    {
        // No direct connection: use A* to find the best path
        struct RoadMap *subChainEnd = NULL;
        struct RoadMap *subChain    = aStarSearch(origin, target, &subChainEnd);

        if (subChain == NULL)
        {
            printf("WARNING: no path found from %s to %s\n",
                   citiesInfo[origin].city_name, citiesInfo[target].city_name);
            return final_cost;
        }

        // subChain starts with origin (already in the main road map) → skip it
        struct RoadMap *curr = subChain->next;
        free(subChain);

        // Splice remaining nodes into the main road map, converting costs to global
        while (curr != NULL)
        {
            struct RoadMap *next = curr->next;
            final_cost      = initial_cost + curr->total_cost; // leg-relative → global
            curr->total_cost = final_cost;
            curr->next       = NULL;
            (*roadEnd)->next = curr;
            *roadEnd         = curr;
            curr = next;
        }
    }

    // Record leg for later printing (no printf here)
    addToPartialRoadMapList(legStart);
    return final_cost;
}

/*
___________________________________________________________________________________________________________________________________________________
Ancestor's Tree
___________________________________________________________________________________________________________________________________________________
*/

struct FamilyTreeNode* createTreeNode(int city_id)
{
    struct FamilyTreeNode *node = malloc(sizeof(struct FamilyTreeNode));
    node->city_id = city_id;
    strcpy(node->motherName, citiesInfo[city_id].mother_name);
    strcpy(node->fatherName, citiesInfo[city_id].father_name);
    node->mother_parents = NULL;
    node->father_parents = NULL;
    return node;
}

// BFS: level by level, mother side before father side at each level
struct FamilyTreeNode* buildBFS(int startCity, struct RoadMap **roadEnd, int *currentCity)
{
    struct FamilyTreeNode *root = createTreeNode(startCity);

    struct FamilyTreeNode *queue[NUMBER_CITIES];
    int front = 0, rear = 0;
    queue[rear++] = root;

    while (front < rear)
    {
        struct FamilyTreeNode *cur = queue[front++];

        int mCity = citiesInfo[cur->city_id].mother_parents_city_id;
        int fCity = citiesInfo[cur->city_id].father_parents_city_id;

        if (mCity != -1)
        {
            routeSearch(*currentCity, mCity, roadEnd);
            *currentCity = mCity;
            cur->mother_parents = createTreeNode(mCity);
            queue[rear++] = cur->mother_parents;
        }

        if (fCity != -1)
        {
            routeSearch(*currentCity, fCity, roadEnd);
            *currentCity = fCity;
            cur->father_parents = createTreeNode(fCity);
            queue[rear++] = cur->father_parents;
        }
    }
    return root;
}

// DFS: full mother branch first, then father branch
struct FamilyTreeNode* buildDFS(int city_id, struct RoadMap **roadEnd, int *currentCity)
{
    if (city_id == -1) return NULL;

    struct FamilyTreeNode *node = createTreeNode(city_id);

    int mCity = citiesInfo[city_id].mother_parents_city_id;
    int fCity = citiesInfo[city_id].father_parents_city_id;

    if (mCity != -1)
    {
        routeSearch(*currentCity, mCity, roadEnd);
        *currentCity = mCity;
        node->mother_parents = buildDFS(mCity, roadEnd, currentCity);
    }

    if (fCity != -1)
    {
        routeSearch(*currentCity, fCity, roadEnd);
        *currentCity = fCity;
        node->father_parents = buildDFS(fCity, roadEnd, currentCity);
    }

    return node;
}

// Print tree with "->" arrows — recursive DFS order (used for DFS tree)
void printTreeDFS(struct FamilyTreeNode *node, int depth)
{
    if (node == NULL) return;

    for (int i = 0; i < depth; i++) printf("->");
    printf(" ");

    printf("%s and %s (%s)\n",
           node->motherName, node->fatherName,
           citiesInfo[node->city_id].city_name);

    printTreeDFS(node->mother_parents, depth + 1);
    printTreeDFS(node->father_parents, depth + 1);
}

// Print tree level by level using a queue — used for BFS tree
// Each node is paired with its depth so we know how many "->" to print
void printTreeBFS(struct FamilyTreeNode *root)
{
    if (root == NULL) return;

    // Queue of node pointers
    struct FamilyTreeNode *nodeQueue[NUMBER_CITIES];
    int depthQueue[NUMBER_CITIES]; // parallel array tracking depth of each queued node
    int front = 0, rear = 0;

    nodeQueue[rear]  = root;
    depthQueue[rear] = 0;
    rear++;

    while (front < rear)
    {
        struct FamilyTreeNode *cur = nodeQueue[front];
        int depth                  = depthQueue[front];
        front++;

        for (int i = 0; i < depth; i++) printf("->");
        printf(" ");
        printf("%s and %s (%s)\n",
               cur->motherName, cur->fatherName,
               citiesInfo[cur->city_id].city_name);

        if (cur->mother_parents != NULL)
        {
            nodeQueue[rear]  = cur->mother_parents;
            depthQueue[rear] = depth + 1;
            rear++;
        }
        if (cur->father_parents != NULL)
        {
            nodeQueue[rear]  = cur->father_parents;
            depthQueue[rear] = depth + 1;
            rear++;
        }
    }
}

void deleteAllTree(struct FamilyTreeNode *node)
{
    if (node == NULL) return;
    deleteAllTree(node->mother_parents);
    deleteAllTree(node->father_parents);
    free(node);
}

/*
___________________________________________________________________________________________________________________________________________________
Main
___________________________________________________________________________________________________________________________________________________
*/
int main()
{
    int startCity = 0; // Barcelona

    printf("This is a %s case of the program\n", SIZE_NAME);
    printf("Ancestor's tree:\n\n");

    /* ======================== BFS ======================== */
    struct RoadMap *roadStart = NULL;
    struct RoadMap *roadEnd   = NULL;
    initRoadMap(startCity, &roadStart, &roadEnd);
    int currentCity = startCity;
    deletePartialRoadMapList();
 
    // 1. Build the tree (road map and leg log filled silently)
    struct FamilyTreeNode *bfsRoot = buildBFS(startCity, &roadEnd, &currentCity);

    // 2. Print the tree level by level
    printf("BFS -> Names:\n");
    printTreeBFS(bfsRoot);
    printf("\n");

    // 3. Print each leg (subchain + cost)
    printPartialRoutes(roadEnd);
    printf("\n");

    // 4. Print total road map + final cost
    printRoadMap(roadStart);

    deleteAllTree(bfsRoot);
    deleteAllRoadMap(&roadStart, &roadEnd);

    /* ======================== DFS ======================== */
    printf("\n");
    printf("----------------------------------\n");

    initRoadMap(startCity, &roadStart, &roadEnd);
    currentCity = startCity;
    deletePartialRoadMapList();

    // 1. Build the tree
    struct FamilyTreeNode *dfsRoot = buildDFS(startCity, &roadEnd, &currentCity);

    // 2. Print the tree
    printf("DFS -> Names:\n");
    printTreeDFS(dfsRoot, 0);
    printf("\n");

    // 3. Print each leg
    printPartialRoutes(roadEnd);
    printf("\n");

    // 4. Print total road map + final cost
    printRoadMap(roadStart);

    deleteAllTree(dfsRoot);
    deleteAllRoadMap(&roadStart, &roadEnd);

    return 0;
}
