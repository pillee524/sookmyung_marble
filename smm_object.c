//
//  smm_node.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>

#define MAX_NODETYPE    7
#define MAX_GRADE       9
#define MAX_NODE        100
static char smmNodeName[MAX_NODETYPE][MAX_CHARNAME] = {
    "lecture",
    "restaurant",
    "laboratory",
    "home",
    "gotoLab",
    "foodChance",
    "festival"
};
static char smmGradeName[MAX_GRADE][MAX_CHARNAME] = {
    "A+", "A0", "A-", "B+", "B0", "B-", "C+", "C0", "C-"
};

typedef struct smmObject {
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
} smmObject_t;

static smmObject_t smm_node[MAX_NODE];
static int smmObj_noNode = 0;

static char smmObj_foodName[MAX_NODE][MAX_CHARNAME];
static int smmObj_foodEnergy[MAX_NODE];
static int smmObj_noFood = 0;

static char smmObj_festMsg[MAX_NODE][MAX_CHARNAME];
static int smmObj_noFest = 0;

//object generation
void smmObj_genNode(char* name, int type, int credit, int energy)
{
    strcpy(smm_node[smmObj_noNode].name, name);
    smm_node[smmObj_noNode].type = type;
    smm_node[smmObj_noNode].credit = credit;
    smm_node[smmObj_noNode].energy = energy;
    smmObj_noNode++;
}

void smmObj_genFood(char* name, int energy)
{
    strcpy(smmObj_foodName[smmObj_noFood], name);
    smmObj_foodEnergy[smmObj_noFood] = energy;
    smmObj_noFood++;
}

void smmObj_genFestival(char* msg)
{
    strcpy(smmObj_festMsg[smmObj_noFest], msg);
    smmObj_noFest++;
}

//member retrieving
char* smmObj_getNodeName(int node_nr)  { return smm_node[node_nr].name; }
int smmObj_getNodeType(int node_nr)    { return smm_node[node_nr].type; }
int smmObj_getNodeCredit(int node_nr)  { return smm_node[node_nr].credit; }
int smmObj_getNodeEnergy(int node_nr)  { return smm_node[node_nr].energy; }
char* smmObj_getFoodName(int nr)       { return smmObj_foodName[nr]; }
int smmObj_getFoodEnergy(int nr)       { return smmObj_foodEnergy[nr]; }
char* smmObj_getFestivalMsg(int nr)    { return smmObj_festMsg[nr]; }

//element to string
char* smmObj_getTypeName(int type)
{
    return smmNodeName[type];
}

char* smmObj_getGradeName(int grade)
{
    return smmGradeName[grade];
}

