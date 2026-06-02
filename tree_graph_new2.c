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
#include <time.h>

#ifdef MEDIUM
    #include "medium.h"
    #define SIZE_NAME "medium"
#elif defined(LARGE)
    #include "large.h"
    #define SIZE_NAME "large"
#else // Falls into small.h as standard
    #include "small.h"
    #define SIZE_NAME "small"
#endif

#include "astar.h"

/* =================================================================================
   UTILITIES
   ================================================================================= */

int treeNodes_info[NUMBER_CITIES][2]; // 2D array that stores city id and depth info from each node of the tree, stored following the order of addition
int treeNodes_count = 0;

struct RoadMap *partialRoadStarts[NUMBER_CITIES]; // Array of pointers to the starts of each sub roads belonging to the total road
int partialRoadCount = 0;

float computePerformanceTime(clock_t start, clock_t end)
{
    return (float)(end - start) / CLOCKS_PER_SEC;
}


/* =================================================================================
   ROAD MAP
   ================================================================================= */

/* --- Roads --- */

struct RoadMap* createRoadNode(int city_id, int total_cost) // Node elements, basic elements for a road's linked list structure
{
    struct RoadMap *node = malloc(sizeof(struct RoadMap));
    if (node == NULL)
    {
        printf("ERROR: malloc failed in createRoadNode\n");
        exit(1);
    }
    node->city_id = city_id;
    node->total_cost = total_cost;
    node->next = NULL;
    return node;
}

void initRoadMap(int startCity, struct RoadMap **roadStart, struct RoadMap **roadEnd) // Create root node of the linked list, starting accumulative cost from 0
{
    *roadStart = createRoadNode(startCity, 0);
    *roadEnd = *roadStart;
}

void addToRoadMap(struct RoadMap **roadEnd, int city_id, int total_cost) // Create node and append to the last node of the linked list with an updated cost
{
    struct RoadMap *node = createRoadNode(city_id, total_cost);
    (*roadEnd)->next = node;
    *roadEnd = node;
}

void deleteAllRoadMap(struct RoadMap **roadStart, struct RoadMap **roadEnd) // Iterate linked list to free node memory
{
    struct RoadMap *current = *roadStart;
    while (current != NULL)
    {
        struct RoadMap *next = current->next;
        free(current);
        current = next;
    }
    *roadStart = NULL;
    *roadEnd = NULL;
}

void printRoadMap(struct RoadMap *roadStart) // Iterate the nodes of a linked list starting from a pointed node
{
    if (roadStart == NULL) // Avoids crashing from empty structure
    {
        printf("Total Road Map: (empty)\n");
        return;
    }
    struct RoadMap *current = roadStart;
    printf("Total Road Map:\n");
    while (current->next != NULL)
    {
        printf("%s-", citiesInfo[current->city_id].city_name);
        current = current->next;
    }
    printf("%s\n", citiesInfo[current->city_id].city_name);
    printf("Total cost: %d\n", current->total_cost);
}

/* --- Partial Roads --- */

void addPartialRoad(struct RoadMap *partialRoadstart) // Store a node pointer into the array that contains pointers to the starts of all partial roads and increase the counter
{
    partialRoadStarts[partialRoadCount++] = partialRoadstart;
}

void resetPartialRoads() // Return the counter of partial road pointers to the original empty state
{
    partialRoadCount = 0;
}

void printPartialRoads(struct RoadMap *totalEnd) // Iterate pointer array
{
    printf("Partial road map:\n");
    for (int i = 0; i < partialRoadCount; i++)
    {
        struct RoadMap *current = partialRoadStarts[i]; // Indicate iteratively the start of the partial route to focus in
        struct RoadMap *stop = totalEnd; // Set the end of the partial route to be the next node in the pointer array except for the last partial route, when partial end = total end directly
        if (i + 1 < partialRoadCount) stop = partialRoadStarts[i + 1];

        int cost = stop->total_cost - current->total_cost;

        while (current != stop)
        {
            printf("%s-", citiesInfo[current->city_id].city_name);
            current = current->next;
        }
        printf("%s %d\n", citiesInfo[current->city_id].city_name, cost);
    }
}

/* --- Route Search --- */

int routeSearch(int origin, int target, struct RoadMap **roadEnd) // Find the optimal connection between two cities, append to main road, and return its cost
{
    struct RoadMap *partialStart = *roadEnd;
    int initial_cost = (*roadEnd)->total_cost;
    int final_cost = initial_cost;

    if (adjacency_matrix[origin][target] != 0) // Check if there is a direct connection
    {
        final_cost += adjacency_matrix[origin][target];
        addToRoadMap(roadEnd, target, final_cost);
    }
    else // Apply A* heuristic in case of no direct connection 
    {
        struct RoadMap *partialRoadEnd = NULL;
        struct RoadMap *partialRoad = aStarSearch(origin, target, &partialRoadEnd);

        if (partialRoad == NULL)
        {
            printf("WARNING: no path found from %s to %s\n",
                   citiesInfo[origin].city_name, citiesInfo[target].city_name);
            return final_cost;
        }
        // Skip the start node from A* result: it duplicates the already-appended origin
        struct RoadMap *curr = partialRoad->next;
        free(partialRoad);

        while (curr != NULL)
        {
            struct RoadMap *next = curr->next;
            final_cost = initial_cost + curr->total_cost;
            curr->total_cost = final_cost;
            curr->next = NULL;
            (*roadEnd)->next = curr;
            *roadEnd = curr;
            curr = next;
        }
    }

    addPartialRoad(partialStart);
    return final_cost; // TODO The pdf specifies routeSearch to return cost, but we give an alternative pipeline
}

/* =================================================================================
   ANCESTOR TREE
   ================================================================================= */

struct FamilyTreeNode* createTreeNode(int city_id) // Generate a node containing info about a couple from the tree and their city
{
    struct FamilyTreeNode *node = malloc(sizeof(struct FamilyTreeNode));
    if (node == NULL)
    {
        printf("ERROR: malloc failed in createTreeNode\n");
        exit(1);
    }
    node->city_id = city_id;
    strcpy(node->motherName, citiesInfo[city_id].mother_name);
    strcpy(node->fatherName, citiesInfo[city_id].father_name);
    node->mother_parents = NULL;
    node->father_parents = NULL;
    return node;
}

void deleteAllTree(struct FamilyTreeNode *node) // Recursively free nodes of the tree (built-in at the end of each methods' execution)
{
    if (node == NULL) return;
    deleteAllTree(node->mother_parents);
    deleteAllTree(node->father_parents);
    free(node);
}

void buildBFS(int startCity, struct RoadMap **roadEnd, int *currentCity)
{
    treeNodes_count = 0;

    struct FamilyTreeNode *queue[NUMBER_CITIES];
    int depthQueue[NUMBER_CITIES];
    int front = 0, rear = 0;

    struct FamilyTreeNode *root = createTreeNode(startCity);
    treeNodes_info[treeNodes_count][0] = startCity;
    treeNodes_info[treeNodes_count][1] = 0;
    treeNodes_count++;

    queue[rear] = root;
    depthQueue[rear] = 0;
    rear++;

    while (front < rear)
    {
        struct FamilyTreeNode *current = queue[front];
        int depth = depthQueue[front];
        front++;

        int mCity = citiesInfo[current->city_id].mother_parents_city_id;
        int fCity = citiesInfo[current->city_id].father_parents_city_id;

        if (mCity != -1)
        {
            routeSearch(*currentCity, mCity, roadEnd);
            *currentCity = mCity;
            current->mother_parents = createTreeNode(mCity);
            treeNodes_info[treeNodes_count][0] = mCity;
            treeNodes_info[treeNodes_count][1] = depth + 1;
            treeNodes_count++;
            queue[rear] = current->mother_parents;
            depthQueue[rear] = depth + 1;
            rear++;
        }

        if (fCity != -1)
        {
            routeSearch(*currentCity, fCity, roadEnd);
            *currentCity = fCity;
            current->father_parents = createTreeNode(fCity);
            treeNodes_info[treeNodes_count][0] = fCity;
            treeNodes_info[treeNodes_count][1] = depth + 1;
            treeNodes_count++;
            queue[rear] = current->father_parents;
            depthQueue[rear] = depth + 1;
            rear++;
        }
    }

    deleteAllTree(root);
}

void buildDFS(int city_id, int depth, struct RoadMap **roadEnd, int *currentCity)
{
    if (city_id == -1) return;

    if (depth == 0) treeNodes_count = 0;

    treeNodes_info[treeNodes_count][0] = city_id;
    treeNodes_info[treeNodes_count][1] = depth;
    treeNodes_count++;

    int mCity = citiesInfo[city_id].mother_parents_city_id;
    int fCity = citiesInfo[city_id].father_parents_city_id;

    if (mCity != -1)
    {
        routeSearch(*currentCity, mCity, roadEnd);
        *currentCity = mCity;
        buildDFS(mCity, depth + 1, roadEnd, currentCity);
    }

    if (fCity != -1)
    {
        routeSearch(*currentCity, fCity, roadEnd);
        *currentCity = fCity;
        buildDFS(fCity, depth + 1, roadEnd, currentCity);
    }
}

/* --- Tree Print --- */

void printTree() // Iterate treeNodes_info array to directly print in order each visited city data associated with its depth inside the tree
{
    for (int i = 0; i < treeNodes_count; i++)
    {
        int city_id = treeNodes_info[i][0];
        int depth = treeNodes_info[i][1];
        for (int j = 0; j < depth; j++) printf("->"); // Arrows represent the depth level of the node inside the tree
        printf(" %s and %s (%s)\n", citiesInfo[city_id].mother_name, citiesInfo[city_id].father_name, citiesInfo[city_id].city_name);
    }
    printf("\n");
}

/* =================================================================================
   MAIN
   ================================================================================= */

int main(int argc, char *argv[])
{
    clock_t start = clock();

    if (argc != 1)
    {
        printf("ERROR: no arguments expected, got %d\n", argc - 1);
        printf("Usage: %s\n", argv[0]);
        exit(1);
    }

    int startCity = 0; // Standardise the system to start root at element of index 0 from the array

    printf("This is a %s case of the program\n", SIZE_NAME);
    printf("Ancestor's tree:\n\n");
    // Initialise the road sequence
    struct RoadMap *roadStart = NULL;
    struct RoadMap *roadEnd = NULL;

    /* BFS */
    clock_t start_bfs = clock();

    initRoadMap(startCity, &roadStart, &roadEnd);
    int currentCity = startCity;
    buildBFS(startCity, &roadEnd, &currentCity);

    // Print of tree node info in BFS order, and road (segmented and full) + cost info in visiting order
    printf("BFS -> Names:\n");
    printTree();
    printPartialRoads(roadEnd);
    printf("\n");
    printRoadMap(roadStart);

    // Clean road & tree after execution
    resetPartialRoads();
    deleteAllRoadMap(&roadStart, &roadEnd);

    clock_t end_bfs = clock();
    float bfs_time = computePerformanceTime(start_bfs, end_bfs);

    printf("\n----------------------------------\n");

    /* DFS */
    clock_t start_dfs = clock();
    initRoadMap(startCity, &roadStart, &roadEnd);
    currentCity = startCity;

    buildDFS(startCity, 0, &roadEnd, &currentCity);

    // Print of tree node info in DFS order, and road (segmented and full) + cost info in visiting order
    printf("DFS -> Names:\n");
    printTree();
    printPartialRoads(roadEnd);
    printf("\n");
    printRoadMap(roadStart);

    // Clean road & tree after execution
    resetPartialRoads();
    deleteAllRoadMap(&roadStart, &roadEnd);

    clock_t end_dfs = clock();

    float dfs_time = computePerformanceTime(start_dfs, end_dfs);
    float total_time = bfs_time + dfs_time;

    printf(" BFS time: %.5fs\n DFS time: %.5fs\n Total time: %.5fs\n", bfs_time, dfs_time, total_time);
    
    return 0;
}