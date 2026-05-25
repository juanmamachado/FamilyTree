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
    struct RoadMap *newNode = malloc(sizeof(struct RoadMap));
    newNode->city_id = city_id;
    newNode->total_cost = total_cost;
    newNode->next = NULL;
    return newNode;
}

// Baseline setting for the roadmap, executed once
void initRoadMap(int startCity, struct RoadMap** roadStart, struct RoadMap** roadEnd) // Double pointer 
{
    *roadStart = createRoadNode(startCity, 0);  // Directly supposes initial cost is 0
    *roadEnd = *roadStart; // Both start and end point initially to the same node
}

// Road map computation, Bidirected graph
void addToRoadMap(struct RoadMap** roadEnd, int city_id, int total_cost)
{
    struct RoadMap *newRoadNode = createRoadNode(city_id, total_cost);
    (*roadEnd)->next = newRoadNode;  // Attach new node to current last node, we make the next from the node where *roadEnd points to point to newEoadNode
    *roadEnd = newRoadNode;          // Move roadEnd forward to new last node

}

// Printing road map and the total cost
void printRoadMap()
{

}

// Reset roadmap, probably by freeing memory allocation
void deleteAllRoadMap()
{

}

// Searching for the route using a proposed heuristic
int RouteSearch() // parameters: src, dest, roadmap (what are they?) ---- int because returns the TOAL COST
{

}

// Route computation (cost, heuristics)

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

}

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

    struct FamilyTreeNode *treeRoot = NULL; // Tree entry point
    return 0;
}