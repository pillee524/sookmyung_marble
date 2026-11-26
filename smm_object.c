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
    int objType;    // 보드칸인지 성적인지 (SMMOBJ_TYPE_BOARD / SMMOBJ_TYPE_GRADE)
    int type;
    int credit;
    int energy;
    int grade;      // 성적 (SMMGRADE_AP ~ SMMGRADE_CM)
} smmObject_t;

static char smmObj_foodName[MAX_NODE][MAX_CHARNAME];
static int smmObj_foodEnergy[MAX_NODE];
static int smmObj_noFood = 0;

static char smmObj_festMsg[MAX_NODE][MAX_CHARNAME];
static int smmObj_noFest = 0;

//object generation
void* smmObj_genObject(char* name, int objType, int type, int credit, int energy, int grade)
{
    smmObject_t* ptr;

    ptr = (smmObject_t*)malloc(sizeof(smmObject_t));
    if (ptr == NULL)
    {
        printf("[ERROR] smmObj_genObject() : malloc failed!\n");
        return NULL;
    }

    strcpy(ptr->name, name);
    ptr->objType = objType;
    ptr->type = type;
    ptr->credit = credit;
    ptr->energy = energy;
    ptr->grade = grade;

    return (void*)ptr;
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
char* smmObj_getObjectName(void* obj)
{
    smmObject_t* ptr = (smmObject_t*)obj;
    return ptr->name;
}

int smmObj_getObjectType(void* obj)
{
    smmObject_t* ptr = (smmObject_t*)obj;
    return ptr->type;
}

int smmObj_getObjectCredit(void* obj)
{
    smmObject_t* ptr = (smmObject_t*)obj;
    return ptr->credit;
}

int smmObj_getObjectEnergy(void* obj)
{
    smmObject_t* ptr = (smmObject_t*)obj;
    return ptr->energy;
}

int smmObj_getObjectGrade(void* obj)
{
    smmObject_t* ptr = (smmObject_t*)obj;
    return ptr->grade;
}

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