//
//  smm_object.h
//  SMMarble object
//
//  Created by Juyeop Kim on 2023/11/05.
//

#ifndef smm_object_h
#define smm_object_h

/* node type :
    lecture,
    restaurant,
    laboratory,
    home,
    experiment,
    foodChance,
    festival
*/
#define SMMNODE_TYPE_LECTURE        0
#define SMMNODE_TYPE_RESTAURANT     1
#define SMMNODE_TYPE_LABORATORY     2
#define SMMNODE_TYPE_HOME           3
#define SMMNODE_TYPE_GOTOLAB        4
#define SMMNODE_TYPE_FOODCHANCE     5
#define SMMNODE_TYPE_FESTIVAL       6

/* grade :
    A+,
    A0,
    A-,
    B+,
    B0,
    B-,
    C+,
    C0,
    C-
*/
#define SMMGRADE_AP     0
#define SMMGRADE_A0     1
#define SMMGRADE_AM     2
#define SMMGRADE_BP     3
#define SMMGRADE_B0     4
#define SMMGRADE_BM     5
#define SMMGRADE_CP     6
#define SMMGRADE_C0     7
#define SMMGRADE_CM     8
#define SMMOBJ_TYPE_BOARD   0
#define SMMOBJ_TYPE_GRADE   1


//object generation
void* smmObj_genObject(char* name, int objType, int type, int credit, int energy, int grade);
void smmObj_genFood(char* name, int energy);
void smmObj_genFestival(char* msg);

//member retrieving
char* smmObj_getObjectName(void* obj);
int smmObj_getObjectType(void* obj);
int smmObj_getObjectCredit(void* obj);
int smmObj_getObjectEnergy(void* obj);
int smmObj_getObjectGrade(void* obj);
char* smmObj_getFoodName(int nr);
int smmObj_getFoodEnergy(int nr);
char* smmObj_getFestivalMsg(int nr);

//element to string
char* smmObj_getTypeName(int type);
char* smmObj_getGradeName(int grade);


#endif /* smm_object_h */