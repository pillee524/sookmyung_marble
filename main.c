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

static player_t cur_player[MAX_PLAYER];


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




int rolldie(int player)
{
    char c;
    printf(" Press any key to roll a die (press g to see grade): ");
    c = getchar();
    fflush(stdin);
    
#if 0
    if (c == 'g')
        printGrades(player);
#endif
    
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
        fflush(stdin);

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
               smmObj_getNodeName(cur_player[i].position));
    }
    printf("=====================================================\n");
}

//move player
void goForward(int player, int step)
{
    int i;
    for (i = 0; i < step; i++)
    {
        cur_player[player].position = (cur_player[player].position + 1) % board_nr;

        if (smmObj_getNodeType(cur_player[player].position) == SMMNODE_TYPE_HOME)
        {
            cur_player[player].energy += smmObj_getNodeEnergy(cur_player[player].position);
            printf("  -> %s passed home! energy +%i\n", cur_player[player].name,
                   smmObj_getNodeEnergy(cur_player[player].position));
        }
    }
    printf("%s go to node %i (%s)\n",
           cur_player[player].name,
           cur_player[player].position,
           smmObj_getNodeName(cur_player[player].position));
}

//action code when a player stays at a node
void actionNode(int player)
{
    int pos = cur_player[player].position;
    int type = smmObj_getNodeType(pos);

    switch(type)
    {
        case SMMNODE_TYPE_LECTURE:
            if (cur_player[player].energy >= smmObj_getNodeEnergy(pos))
            {
                cur_player[player].accumCredit += smmObj_getNodeCredit(pos);
                cur_player[player].energy -= smmObj_getNodeEnergy(pos);
                printf("  -> take lecture %s : credit +%i, energy -%i\n",
                       smmObj_getNodeName(pos), smmObj_getNodeCredit(pos), smmObj_getNodeEnergy(pos));
            }
            else
            {
                printf("  -> not enough energy for %s\n", smmObj_getNodeName(pos));
            }
            break;

        case SMMNODE_TYPE_RESTAURANT:
            cur_player[player].energy += smmObj_getNodeEnergy(pos);
            printf("  -> eat at %s : energy +%i\n", smmObj_getNodeName(pos), smmObj_getNodeEnergy(pos));
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
        smmObj_genNode(name, type, credit, energy);
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
        fflush(stdin);
    }
    while (player_nr < 1 || player_nr > MAX_PLAYER);   // 1~10이 아니면 다시 입력
    generatePlayers(player_nr, smmObj_getNodeEnergy(0));   // 초기 에너지 = 우리집 에너지(18)
    
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
        
        //4-5. next turn
        turn = (turn + 1) % player_nr;   // 다음 사람, 마지막 사람 다음엔 0번
    }
    return 0;
}
