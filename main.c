//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include <time.h>
#include <string.h>
#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"


//board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;

//player
static int player_nr;

typedef struct {
    char name[MAX_CHARNAME];   // 이름
    int energy;                // 에너지
    int position;              // 현재 칸 번호
    int accumCredit;           // 모은 학점
    int flag_graduate;         // 졸업했으면 1
} player_t;

static player_t* cur_player;


//function prototypes
#if 0
int isGraduated(void); //check if any player is graduated
void generatePlayers(int n, int initEnergy); //generate a new player
void printGrades(int player); //print grade history of the player
void goForward(int player, int step); //make player go "step" steps on the board (check if player is graduated)
void printPlayerStatus(void); //print all player status at the beginning of each turn
float calcAverageGrade(int player); //calculate average grade of the player
smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history
void printGrades(int player); //print all the grade history of the player
#endif



//print all grade history of the player
void printGrades(int player)
{
    int i;
    void* gradePtr;
    printf("\n----- %s's grades -----\n", cur_player[player].name);
    for (i = 0; i < smmdb_len(LISTNO_OFFSET_GRADE + player); i++)
    {
        gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
        printf("%s (credit %i) : %s\n",
               smmObj_getObjectName(gradePtr),
               smmObj_getObjectCredit(gradePtr),
               smmObj_getGradeName(smmObj_getObjectGrade(gradePtr)));
    }
    printf("------------------------\n");
}

//find the grade from the player's grade history (없으면 NULL)
void* findGrade(int player, char* lectureName)
{
    int i;
    void* gradePtr;
    for (i = 0; i < smmdb_len(LISTNO_OFFSET_GRADE + player); i++)
    {
        gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
        if (strcmp(smmObj_getObjectName(gradePtr), lectureName) == 0)
            return gradePtr;
    }
    return NULL;
}

int rolldie(int player)
{
    char c;
    printf(" Press any key to roll a die (press g to see grade): ");
    c = getchar();
    if (c != '\n')
        while (getchar() != '\n');
    
    if (c == 'g')
        printGrades(player);
    
    return (rand()%MAX_DIE + 1);
}
//make players
void generatePlayers(int n, int initEnergy)
{
    int i;
    for (i = 0; i < n; i++)
    {
        printf("Input player %i's name: ", i);
        scanf("%s", cur_player[i].name);
        while (getchar() != '\n');

        cur_player[i].energy = initEnergy;
        cur_player[i].position = 0;
        cur_player[i].accumCredit = 0;
        cur_player[i].flag_graduate = 0;
    }
}

//print player info
void printPlayerStatus(void)
{
    int i;
    printf("\n=================== PLAYER STATUS ===================\n");
    for (i = 0; i < player_nr; i++)
    {
        printf("%s : credit %i, energy %i, at %i(%s)\n",
               cur_player[i].name,
               cur_player[i].accumCredit,
               cur_player[i].energy,
               cur_player[i].position,
               smmObj_getObjectName(smmdb_getData(LISTNO_NODE, cur_player[i].position)));
    }
    printf("=====================================================\n");
}

//move player
void goForward(int player, int step)
{
    int i;
    void* boardPtr;
    for (i = 0; i < step; i++)
    {
        cur_player[player].position = (cur_player[player].position + 1) % board_nr;
        boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);

        if (smmObj_getObjectType(boardPtr) == SMMNODE_TYPE_HOME)
        {
            cur_player[player].energy += smmObj_getObjectEnergy(boardPtr);
            printf("  -> %s passed home! energy +%i\n", cur_player[player].name,
                   smmObj_getObjectEnergy(boardPtr));
        }
    }
    boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
    printf("%s go to node %i (%s)\n",
           cur_player[player].name,
           cur_player[player].position,
           smmObj_getObjectName(boardPtr));
}

//action code when a player stays at a node
void actionNode(int player)
{
    void* boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
    int type = smmObj_getObjectType(boardPtr);
    char* nodeName = smmObj_getObjectName(boardPtr);
    int credit = smmObj_getObjectCredit(boardPtr);
    int energy = smmObj_getObjectEnergy(boardPtr);
    void* gradePtr;

    switch(type)
    {
        case SMMNODE_TYPE_LECTURE:
            if (findGrade(player, nodeName) != NULL)   // 이미 들은 강의
            {
                printf("  -> %s already taken\n", nodeName);
            }
            else if (cur_player[player].energy >= energy)
            {
                cur_player[player].accumCredit += credit;
                cur_player[player].energy -= energy;

                // 성적 생성해서 그 플레이어의 성적 리스트에 저장
                gradePtr = smmObj_genObject(nodeName, SMMOBJ_TYPE_GRADE, 0, credit, 0, rand() % 9);
                smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);

                printf("  -> take lecture %s : credit +%i, energy -%i, grade %s\n",
                       nodeName, credit, energy,
                       smmObj_getGradeName(smmObj_getObjectGrade(gradePtr)));
            }
            else
            {
                printf("  -> not enough energy for %s\n", nodeName);
            }
            break;

        case SMMNODE_TYPE_RESTAURANT:
            cur_player[player].energy += energy;
            printf("  -> eat at %s : energy +%i\n", nodeName, energy);
            break;

        default:
            break;
    }
}



int main(int argc, const char * argv[]) {
    
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    
    board_nr = 0;
    food_nr = 0;
    festival_nr = 0;
    
    srand(time(NULL));
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    while (fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4) //read a node parameter set
    {
        //store the parameter set
        void* boardObj = smmObj_genObject(name, SMMOBJ_TYPE_BOARD, type, credit, energy, 0);
        smmdb_addTail(LISTNO_NODE, boardObj);
        board_nr++;
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);
    
    
    
    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");
    while (fscanf(fp, "%s %i", name, &energy) == 2) //read a food parameter set
    {
        //store the parameter set
        smmObj_genFood(name, energy);
        food_nr++;
    }
    fclose(fp);
    printf("Total number of food cards : %i\n", food_nr);
    
    
    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while (fscanf(fp, "%s", name) == 1) //read a festival card string
    {
        //store the parameter set
        smmObj_genFestival(name);
        festival_nr++;
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n", festival_nr);
    
    
    //2. Player configuration ---------------------------------------------------------------------------------
    do
    {
        //input player number to player_nr
        printf("input player no. (1~%i): ", MAX_PLAYER);
        scanf("%d", &player_nr);
        while (getchar() != '\n');
    }
    while (player_nr < 1 || player_nr > MAX_PLAYER);   // 1~10이 아니면 다시 입력
    cur_player = (player_t*)malloc(player_nr * sizeof(player_t));   // 플레이어 수만큼 메모리 할당
    generatePlayers(player_nr, smmObj_getObjectEnergy(smmdb_getData(LISTNO_NODE, 0)));   // 초기 에너지 = 우리집 에너지(18)
    
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    int turn = 0;   // 지금 차례인 플레이어 번호
    while (1) //is anybody graduated?
    {
        int die_result;
        
        //4-1. initial printing
        printPlayerStatus();
        
        //4-2. die rolling (if not in experiment)
        die_result = rolldie(turn);
        
        //4-3. go forward
        goForward(turn, die_result);

        //4-4. take action at the destination node of the board
        actionNode(turn);

        // 졸업 체크 : 학점이 GRADUATE_CREDIT 이상이면 게임 종료
        if (cur_player[turn].accumCredit >= GRADUATE_CREDIT)
        {
            printf("\n*** %s graduated! GAME OVER ***\n", cur_player[turn].name);
            printGrades(turn);
            break;
        }
        
        //4-5. next turn
        turn = (turn + 1) % player_nr;   // 다음 사람, 마지막 사람 다음엔 0번
    }
    free(cur_player);
    return 0;
}