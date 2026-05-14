/*
Degree: Artificial Intelligence
Subject: Fundamentals of Programming 2
Practical project: 2

large.h
Header of adjacency matrix - data structures required by the project 2.
This is the large case, however it's an empty case

We've already declared, but not initialized, both data structures:
- adjacency matrix 
- cities info
*/

#define NUMBER_CITIES 100
#define MAX_NAME 10
#define MAX_CITY_NAME 20

int adjacency_matrix[NUMBER_CITIES][NUMBER_CITIES] = 
{
    
};

struct CivilRegistry
{
    int city_id;
    char city_name[MAX_CITY_NAME];
    char mother_name[MAX_NAME];
    char father_name[MAX_NAME];
    int mother_parents_city_id;
    int father_parents_city_id;
};

struct CivilRegistry citiesInfo[NUMBER_CITIES] =
{

};

struct FamilyTreeNode
{
    char motherName[MAX_NAME];
    char fatherName[MAX_NAME];
    int city_id;
    struct FamilyTreeNode * mother_parents;
    struct FamilyTreeNode * father_parents;
};

struct RoadMap
{
    int city_id;
    int total_cost;
    struct RoadMap * next;
};
