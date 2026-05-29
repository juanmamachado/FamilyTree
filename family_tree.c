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
    printf("Total Road Map:\n");
    while (current->next != NULL)
    {
        printf("%s-", citiesInfo[current->city_id].city_name);
        current = current->next;
    }
    // Last node printed without dash
    printf("%s\n\n", citiesInfo[current->city_id].city_name);
    printf("Total cost: %d", current->total_cost);
}

//print partial road map and each route cost
void printPartialRoadMap(struct RoadMap *partialRoadStart, int partialCost)
{
    struct RoadMap *current = partialRoadStart;

    while (current->next != NULL)
    {
        printf("%s-", citiesInfo[current->city_id].city_name);
        current = current->next;
    }

    printf("%s %d\n", citiesInfo[current->city_id].city_name, partialCost);
}

// Reset roadmap, probably by freeing memory allocation
void deleteAllRoadMap(struct RoadMap **roadStart, struct RoadMap **roadEnd)
{
    struct RoadMap *current = *roadStart;
    while (current != NULL)
    {
        struct RoadMap *nextNode = current->next;  // save next BEFORE freeing
        free(current);                             
        current = nextNode;                        
    }
    *roadStart = NULL;
    *roadEnd   = NULL;
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
            final_cost = final_cost + partial_cost;
            addToRoadMap(roadEnd, current_city_id, final_cost);
        }
        else // Find connection with the lowest cost through heuristics
        {
            // Route computation (cost, heuristics), take into account visited must be 0
            // A* route search algorithm finds the complete optimal path
            int leg_cost = 0;
            struct RoadMap *subPath = aStarSearch(current_city_id, target_city_id, &leg_cost);
            if (subPath == NULL) return final_cost;  // no path found

            // subPath starts with current_city_id, which is already
            // in the main road map → skip it and free its node
            struct RoadMap *curr = subPath->next;
            free(subPath);

            // Append the remaining nodes (with globally accumulated cost)
            while (curr != NULL)
            {
                struct RoadMap *nextNode = curr->next;
                final_cost   = initial_cost + curr->total_cost;
                curr->total_cost = final_cost;   // convert leg-relative to global
                curr->next   = NULL;
                (*roadEnd)->next = curr;
                *roadEnd = curr;
                curr = nextNode;
            }
            current_city_id = target_city_id;  // tells while to exit
        }
    }
    printPartialRoadMap(partialRoadStart, final_cost - initial_cost);
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
struct FamilyTreeNode* buildDFS(int city_id, struct RoadMap **roadEnd, int *currentCity) // Returns root of the tree
{
    // Base case: no more ancestors
    if (city_id == -1) return NULL;

    // Create the Node
    struct FamilyTreeNode *newNode = malloc(sizeof(struct FamilyTreeNode));
    newNode->city_id = city_id;
    newNode->motherName = citiesInfo[city_id].mother_name;
    newNode->fatherName = citiesInfo[city_id].father_name;
    mother_parents_cityid = citiesInfo[city_id].mother_parents_city_id;
    father_parents_cityid = citiesInfo[city_id].father_parents_city_id;

    // Mother's side (left part???)
    final cost = routeSearch(int currentCity, int mother_parents_cityid, struct RoadMap **roadEnd); // perque necessitem el final cost aqui?
    newNode->mother_parents = buildDFS(mother_parents_cityid, struct RoadMap **roadEnd, int currentCity); //current city no hauria de ser pointer?

    // Father's side (right part???)
    final cost = routeSearch(int currentCity, int father_parents_cityid, struct RoadMap **roadEnd); // perque necessitem el final cost aqui?
    newNode->father_parents = buildDFS(father_parents_cityid, struct RoadMap **roadEnd, int currentCity); //current city no hauria de ser pointer?

    return Node;
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


    // TODO to add need nodes to the travel linked list addToRoadMap, we need to follow the order of DFS or BFS
    // TODO so first a tree will be created
    // TODO when we execute BFS or DFS, each iteration should update the linked list, as it will allow us to know the cities to connect and calculate the shortest cost with RouteSearch

    struct FamilyTreeNode *treeRoot = NULL; // Tree entry point
    return 0;

}
