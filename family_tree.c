/*
===================================================================================================================================================
Fundamentals of Programming II: Trees and Graphs
Team: Juanma Machado, Javier Olivé
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

// Example: gcc family_tree.c -Dsmall -o exe
// TODO is this the solution the guide suggests? Wouldn't it be better to decide at execution which option to choose?
#ifdef small
    #include "small.h"
#elif defined(medium)
    #include "medium.h"
#elif defined(large)
    #include "large.h"
#else
    #include "small.h"
#endif


// Define as a command argument if we use large, medium or small, setting small as standard. Set root?

/*
___________________________________________________________________________________________________________________________________________________
World Journey Graph
___________________________________________________________________________________________________________________________________________________
*/
// Road map computation, Bidirected graph
// a) searching for the route using a proposed heuristic
struct RoadMap* addToRoadMap()
{

}
// b) printing road map and the total cost
struct RoadMap* printRoadMap()
{

}


// Route computation (cost, heuristics)

/*
___________________________________________________________________________________________________________________________________________________
Ancestor's Tree
___________________________________________________________________________________________________________________________________________________
*/
// Family tree computation
// a) DFS tree creation
// b) BFS tree creation
// c) printing the final ancestors’ tree

/*
___________________________________________________________________________________________________________________________________________________
Main Implementation
___________________________________________________________________________________________________________________________________________________
*/
// Main
int main()
{
    return 0;
}
