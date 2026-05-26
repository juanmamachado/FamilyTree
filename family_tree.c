/*
===================================================================================================================================================
 * Universitat Autònoma de Barcelona - UAB
 * 	Bachelor's Degree in Artificial Intelligence
 *     	Fundamentals of Programming II
 *         	Practical Project II - Family Tree
 * 			Team:	
 * 				Name          Surname          NIU
 * 				--------------------------------------
 * 				Javier        Olivé Padrós     1794767
 * 				Juanma        Machado Millan   1797427
 * Universitat Autònoma de Barcelona - UAB
 * 	Bachelor's Degree in Artificial Intelligence
 *     	Fundamentals of Programming II
 *         	Practical Project II - Family Tree
 * 			Team:	
 * 				Name          Surname          NIU
 * 				--------------------------------------
 * 				Javier        Olivé Padrós     1794767
 * 				Juanma        Machado Millan   1797427
===================================================================================================================================================
*/

/*
___________________________________________________________________________________________________________________________________________________
Imports & Utility Functions
___________________________________________________________________________________________________________________________________________________
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Example: gcc family_tree.c -DSMALL -o exe (uppercase is the convention)
// TODO is this the solution the guide suggests? Wouldn't it be better to decide at execution which option to choose?
#ifdef MEDIUM
    #include "medium.h"
#elif defined(LARGE)
    #include "large.h"
#else
    #include "small.h" // Falls back to small.h whether DSMALL was specified or anything else than DMEDIUM and DLARGE
#endif


/*
___________________________________________________________________________________________________________________________________________________
World Journey Graph
___________________________________________________________________________________________________________________________________________________
*/

// Function to gather code and make this recurrent action more understandable inside other functions
struct RoadMap* createRoadNode(int city_id, int total_cost) 
{
    struct RoadMap *newRoadNode = malloc(sizeof(struct RoadMap));
    newRoadNode->city_id = city_id;
    newRoadNode->total_cost = total_cost;
    newRoadNode->next = NULL;
    return newRoadNode;
}

// Baseline setting for the roadmap, executed once
void initRoadMap(int startCity, struct RoadMap **roadStart, struct RoadMap** roadEnd) // Double pointer so we can operate on the pointer itself, not only the value of its address (will only be able to work on a copy of it, not change it on the original otherwise)
{
    *roadStart = createRoadNode(startCity, 0);  // Directly supposes initial cost is 0
    *roadEnd = *roadStart; // Both start and end point initially to the same node
} // TODO do we need to pass the pointers to do end=start?

// Road map computation, Bidirected graph, linked list TODO ?
void addToRoadMap(struct RoadMap **roadEnd, int city_id, int total_cost)
{
    struct RoadMap *newRoadNode = createRoadNode(city_id, total_cost); // Create new road node
    (*roadEnd)->next = newRoadNode;  // Make the current last node (where end now points) point to new
    *roadEnd = newRoadNode; // Make the end pointer point to the new node
    // printRoadMap(struct RoadMap *roadStart) -- Call printRoadMap() each time you update the list in order to check the progress
}

// Printing road map and the total cost
void printRoadMap(struct RoadMap *roadStart)
{
    struct RoadMap* current = roadStart;
    while (current->next != NULL)
    {
        printf("%s-", citiesInfo[current->city_id].city_name);
        current = current->next;
    }
    // Last node printed without dash
    printf("%s ", citiesInfo[current->city_id].city_name);
}

// Reset roadmap, probably by freeing memory allocation
void deleteAllRoadMap(struct RoadMap *roadStart, struct RoadMap *roadEnd)
{
    struct RoadMap *current = *roadStart;
    while (current != NULL)
    {
        roadStart->next = roadStart->next->next;
        free(current);
        current->next = roadStart;
    }
    *roadStart = NULL;
    // roadEnd->next = NULL; TODO dont need because pointes to garbage but will be restted to null at the start of the next new program execution
    // roadStart->next = NULL; TODO unnecessary: will already end up pointg where the last node pointed: NULL
}

// Searching for the route using a proposed heuristic
int routeSearch(int origin_city_id, int target_city_id, struct RoadMap** roadEnd) // parameters: src, dest, roadmap (what are they?) ---- int because returns the TOAL COST
{
    struct RoadMap *partialRoadStart = *roadEnd; // Pointer to the last node, start of the subchain
    int initial_cost = (*roadEnd)->total_cost;
    int final_cost = initial_cost;
    int partial_cost;
    int current_city_id = origin_city_id;
    int visited[NUMBER_CITIES] = {0}; // We create an array of X cities, and initialize to 0 to track when they have been visited to avoid the algorithm to enter loops between 2 cities
    while (current_city_id != target_city_id)
    {
        if (adjacency_matrix[current_city_id][target_city_id] != 0) // First, check if there is a direct connection
        {
            partial_cost = adjacency_matrix[current_city_id][target_city_id];
            current_city_id = target_city_id;
        }
        else // Find connection with the lowest cost through heuristics
        {
            // Route computation (cost, heuristics), take into account visited must be 0
            int current_city_id = ???next create linkedlist? save the start from the new?;
            visited[current_city_id] = 1;
        }
        final_cost = final_cost + partial_cost;
        addToRoadMap(roadEnd, current_city_id, final_cost); 
    }
    printRoadMap(partialRoadStart);
    printf("%d\n", final_cost-initial_cost);
    return final_cost;
}

/*
___________________________________________________________________________________________________________________________________________________
Ancestor's Tree
___________________________________________________________________________________________________________________________________________________
*/
// Family tree computation
// a) BFS tree creation
struct FamilyTreeNode* buildBFS() // Returns root of the tree
{

}
// b) DFS tree creation
struct FamilyTreeNode* buildDFS() // Returns root of the tree
{

}
// c) printing the final ancestors’ tree
void printTree()
{

} // TODO need to track depth in bothe methods

void deleteAllTree()
{

}


/*
___________________________________________________________________________________________________________________________________________________
Main Implementation
___________________________________________________________________________________________________________________________________________________
*/
// Main
int main()
{
    struct RoadMap *roadStart = NULL; // Linked list starting point
    struct RoadMap *roadEnd = NULL; // Linked list ending point
    // First node is a special case, handle it once
    // Total cost is 0 for sure, but the city_id needs to be passed in case it is flexible
    int startCity = 0;
    initRoadMap(startCity, &roadStart, &roadEnd); // This sets the starting point for the head and tail pointers, that will grow progressively by iterating the end pointer through newly created nodes
    // from here, addToRoadMap only needs roadEnd
    // addToRoadMap(&roadEnd, city_id, total_cost);


    // TODO to add need nodes to the travel linked list addToRoadMap, we need to follow the order of DFS or BFS
    // TODO so first a tree will be created
    // TODO when we execute BFS or DFS, each iteration should update the linked list, as it will allow us to know the cities to connect and calculate the shortest cost with RouteSearch

    struct FamilyTreeNode *treeRoot = NULL; // Tree entry point
    return 0;
}
