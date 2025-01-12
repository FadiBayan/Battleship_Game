#include "../include/Player.h"
#include "../include/ShipPlacement.h"
#include "../include/Bot.h"
#include "../include/CalcProbs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

char * PlayerColors[playerColorCount] = {RED, GREEN, YELLOW, BLUE, MAGENTA};


Player ** alloc_InitializePlayerArray(int playerCount, Player *** playersArray){
    *playersArray = (Player**)(malloc(sizeof(Player*) * playerCount));

    for (int i = 0; i < playerCount; i++)
    {
        (*playersArray)[i] = NULL;
    }
    
}

/**
 * We pass a pointer to a pointer here not just a pointer because the function would create a copy of the pointer and have the copy point at the allocated
 * memory. Then I would have to set the orignal pointer equal to the one returned here. I decided to pass a pointer to a pointer to not have to do all that,
 * just call the function without needing a left side of an expression.
 */
Player* alloc_InitializePlayer(Player** output, char* playerName, int isBot, BotIQ botIQ){
    
    

    *output = (Player *)(malloc(sizeof(Player)));

    //Initialize Grid and name:

    (*output)->name = (char*)(malloc(sizeof(char) * MAXINPUTLENGTH));

    strcpy((*output)->name, playerName);

    (*output)->grid = (char**)(malloc(sizeof(char*) * GRIDSIZE));
    for (int i = 0; i < GRIDSIZE; i++)
    {
        ((*output)->grid)[i] = (char*)(malloc(sizeof(char) * GRIDSIZE));
    }

    for (int i = 0; i < GRIDSIZE; i++)
    {
        for (int j = 0; j < GRIDSIZE; j++)
        {
            ((*output)->grid)[i][j] = WATER_C;
        }
        
    }
    
    //initializing artilleryused, smokegrid, and torpedoused to false

    (*output)->smokeGrid = (bool**)(malloc(sizeof(bool*) * GRIDSIZE));
    for (int i = 0; i < GRIDSIZE; i++) {
        (*output)->smokeGrid[i] = (bool*)malloc(sizeof(bool) * GRIDSIZE);
        for (int j = 0; j < GRIDSIZE; j++) {
            (*output)->smokeGrid[i][j] = false; // initializing each cell to false
        }
    }
    
    (*output)->usedsmokes = 0;
    // Initialize sweepsLeft to 3
    (*output)->sweepsLeft = 3;
    (*output)->prevSunk = 0;
    (*output)->currSunkShips = 0;



    //initialize player color
    srand(time(0));
    (*output)->UIColor = PlayerColors[rand() % playerColorCount]; //Need a better way so that no two players have same color.
    

    (*output)->isBot = isBot;
    (*output)->botIQ = botIQ;


    //Initialize the probability distribution grid:
    InitializeProbabilities(*output);

    DisplayIntGrid((*output)->probabilityGrid, GRIDSIZE);

    InitializeProbabilityHeaps(*output);
    printf("adsff\n");

    if (isBot == 1){

        //If Bot: Initialize Bot Stack Memory:
        InitializeBotStackMemory(*output);
    }

    (*output)->currTargetCategory = PROB_CATEGORYCOUNT - 1;



    //Initializing Risk Factor:
    (*output)->riskFactor = NORMAL_RISK;

    return *output;
}

#pragma region [PROBABILITY INITIALIZATION]

/**
 * This function passes over the entire grid and adds the number of possible orientations for placing a ship in each square.
 */
int InitializeProbabilities(Player * player){

    player->probabilityGrid = (int**)(malloc(sizeof(int*) * GRIDSIZE));

    for (int i = 0; i < GRIDSIZE; i++)
    {
        player->probabilityGrid[i] = (int*)(malloc(sizeof(int) * GRIDSIZE));
    }

    //I need to calculate the cutoff probability for each square on the grid:
    for (int i = 0; i < GRIDSIZE; i++)
    {
        for (int j = 0; j < GRIDSIZE; j++)
        {
            int ij[2] = {i,j}; 
            InitalizeCutOffProb(player, ij);
        }
        
    }
    
    return 1;

}

#pragma region [Binomial Heap Specific Functions]
int compareProbabilities(void * elem1, void * elem2){

    //The void pointer will be an integer pointer representing an array of 3 integers
    //The integer at index 0 is the key and the indices 1 and 2 represent the coordinates i and j respectively

    return ((int*)elem2)[0] - ((int*)elem1)[0];//returns elem2 - elem1 so that it would act as a maxheap
}

/**
 * Input:
 *      - Prob: the probability at this grid coordinate
 *      - i: the row index
 *      - j: the column index
 */
int BinHeap_Insert_ProbElement(BinomialHeap * binHeap, int Prob, int row, int col){

    int * element = (int*)(malloc(sizeof(int) * 3));
    element[0] = Prob;
    element[1] = row;
    element[2] = col;

    insert(binHeap, (void*)element);

    return 1;
}

/**
 * This function returns an pointer which represents an array of three integers (probability, i, j).
 */
int* BinHeap_FindHighestProbabilityCell(BinomialHeap * binHeap){

    void * topVal = findMin(binHeap);

    return (int*)(topVal);

}

void InOrderTraversalTree_ProbNode(Node * root){
    if (root == NULL) return;

    
    InOrderTraversalTree_INT(root->leftChild);
    printf("%d, ", ((int*)(root->value))[0]);
    
    InOrderTraversalTree_INT(root->rightSibling);
}

void InOrderTraversal_ProbNode_Print(BinomialHeap * heap){

    if (heap == NULL) return;

    Node * curr = heap->head;


    InOrderTraversalTree_INT(curr);
    
    printf("\n");

}

#pragma endregion


/**
 * 
 * This function takes in two integers i and j representing coordinates of a region in the grid.
 * It rounds the two coordinates to the region origin and hashes that origin to an index. The hashing is simple, we just number the regions
 * row by row from left to right, and according to i and j we can calculate that number.
 * 
 */
int HashRegion(int i, int j){

    //Step 1: Get region coordinate
    //The function should first transform i and j into the region corner coordinate which will be then hashed.
    //This way all coordinates within the same region will be hashed to the same index.

    int regionSize[2] = PROB_REGION_SIZE;

    //Let's floor the indices:
    i = (i - i%(regionSize[0] + 1))%regionSize[0];
    j = (j - j%(regionSize[0] + 1))%regionSize[0];


    //We can't just add i and j then hash the sum because (1,0) and (0,1) would then hash to the same index

    //Step 2: Hash the resultant value:

    //This is very simple, we are just numbering the regions on the grid row by row from left to right

    int regsPerRow = GRIDSIZE / regionSize[0];

    int cellNumber = i * regsPerRow + j;

    return cellNumber;
}


/**
 * This function initializes the binomial heaps stored inside the player struct. For every region of the probability graph, the function creates
 * a heap that stores the coordinates of a region and orders them according to probability. The heap is a maximum heap, it prioritizes higher probabilities.
 * 
 * Heaps could fall into one of three categories:
 *      - High probability: Heaps of regions with highest probability >= HIGHPROB_BASE macro are placed here
 *      - Average probability: Heaps of regions with highest probability >= AVGPROB_BASE macro but < HIGHPROB_BASE are placed here
 *      - Low probability: Heaps of regions with highest probability >= LOWPROB_BASE macro but < AVGPROB_BASE are placed here
 *      - Null chance: Heaps of regions with highest probability = 0 are placed here
 * 
 * Maximum memory is allocated for each category (the total number of regions) not to have to realloc every time.
 */
int InitializeProbabilityHeaps(Player * player){

    player->probabilityHeapSet = (BinomialHeap**)(malloc(sizeof(BinomialHeap*) * PROB_REGION_COUNT));

    
    player->probabilityHeapArray = (BinomialHeap***)(malloc(sizeof(BinomialHeap**) * PROB_CATEGORYCOUNT));
    player->highprobArrSize = 0;
    player->avgprobArrSize = 0;
    player->lowprobArrSize = 0;

    //I need to go over each and every region, fill up a heap with all its squares, check the top of the heap and place according to the 
    //value.

    /**
     * Rather than reallocating every time I need to update the probability category arrays, I will allocate the maximum possible memory size
     * for each and keep track of the current size of each. This way it is O(1) to update.
     */

    for (int i = 0; i < PROB_CATEGORYCOUNT; i++)
    {
        player->probabilityHeapArray[i] = (BinomialHeap**)(malloc(sizeof(BinomialHeap*) * PROB_REGION_COUNT));
    }

    for (int i = 0; i < PROB_CATEGORYCOUNT; i++)
    {
        for (int j = 0; j < PROB_REGION_COUNT; j++)
        {
            player->probabilityHeapArray[i][j] = (BinomialHeap*)(malloc(sizeof(BinomialHeap)));
        }
        
    }
    

    int RegionSize[2] = PROB_REGION_SIZE;

    for (int i = 0; i < ceil(GRIDSIZE / RegionSize[0]); i++)
    {

        for (int j = 0; j < ceil(GRIDSIZE / RegionSize[0]); j++)
        {
            

            BinomialHeap binHeap;
            binHeap.head = NULL;
            binHeap.compare = compareProbabilities;

            int k0 = i * RegionSize[0] + 1;
            if (i == 0) k0 = 0;

            int l0 = j * RegionSize[0] + 1;
            if (j == 0) l0 = 0;


            printf("k0 = %d, l0 = %d\n", k0,l0);

            for (int k = k0; k <= MIN(GRIDSIZE - 1, (i+1) * RegionSize[0]); k++)
            {
                for (int l = l0; l <= MIN(GRIDSIZE - 1, (j+1)*RegionSize[0]); l++)
                {
                    BinHeap_Insert_ProbElement(&binHeap, player->probabilityGrid[k][l], k, l);
                    //InOrderTraversal_ProbNode_Print(&binHeap);
                    //printf("prb %d,%d\n", l,j);
                    //printf("prb: %d, coord: %d,%d\n", player->probabilityGrid[k][l], k, l);
                }
            }

            //printf("\n");

            int regRow = (i == 0)?0:(i * RegionSize[0] + 1);
            int regCol = (j == 0)?0:(j * RegionSize[0] + 1);
            //printf("DSF %d,%d\n", regRow, regCol);

            //I must insert the resultant heap into the heap set in the player struct:
            (player->probabilityHeapSet[HashRegion(regRow, regCol)] = (BinomialHeap*)(malloc(sizeof(BinomialHeap))))->head = binHeap.head;
            player->probabilityHeapSet[HashRegion(regRow, regCol)]->compare = binHeap.compare;

            //printf("%d\n", HashRegion(regRow, regCol));

            //Now we check the maximum element of the heap:
            int* highestProbPtr = BinHeap_FindHighestProbabilityCell(&binHeap);
            int highestProb = BinHeap_FindHighestProbabilityCell(&binHeap)[0];

            //InOrderTraversal_ProbNode_Print(&binHeap);
            printf("highestProb: %d\n", highestProb);

            if (highestProb >= HIGHPROB_BASE){

                //Place in array of index 2 for high probabilities:
                player->probabilityHeapArray[2][player->highprobArrSize]->head = binHeap.head;
                player->probabilityHeapArray[2][player->highprobArrSize]->compare = binHeap.compare;
                player->highprobArrSize++;
            }
            else if (highestProb >= AVGPROB_BASE){
                //Place in array of index 1 for avg probabilities:
                player->probabilityHeapArray[1][player->avgprobArrSize]->head = binHeap.head;
                player->probabilityHeapArray[1][player->avgprobArrSize]->compare = binHeap.compare;
                player->avgprobArrSize++;
            }
            else {
                //Place in array of index 0 for low probabilities:
                player->probabilityHeapArray[0][player->lowprobArrSize]->head = binHeap.head;
                player->probabilityHeapArray[0][player->lowprobArrSize]->compare = binHeap.compare;
                player->lowprobArrSize++;
            }
        }
    }

    //exit(0);

    return 1;
}

#pragma endregion



#pragma region [Bot-Specific Initializations]

int InitializeBotStackMemory(Player * bot){

    //To be continued...
    if (bot->isBot == true)
        bot->stackMemory = create_empty_Dlist();
    else bot->stackMemory = NULL;

    return 1;
}

#pragma endregion











void DisplayGrid(char ** grid, int gridSize){

    //I need to calculate proper indentation between grid squares. It depends on the length of the numerals:
    int base = endingCoordinate_1 - startingCoordinate_1 + 1;
    int TopNumLen = 0;
    int k = gridSize;
    do
    {
        TopNumLen++;
        k/=base;
    } while (k > 0);
    

    printf("\n");
    int h = (getConsoleWidth() - (TopNumLen * gridSize) - gridSize);
    Indent(h/2);

    //I need a way to convert integer to numeral according to my numeral system.
    for (int i = 0; i < gridSize; i++)
    {
        char* num = alloc_IntegerToNumeral(i, gridSize, startingCoordinate_1, endingCoordinate_1);
        printf("%s ", num);
        free(num);
    }

    Println("");

    for (int i = 1; i <= gridSize; i++)
    {
        char* num = alloc_IntegerToNumeral(i, gridSize, startingCoordinate_2, endingCoordinate_2);
        
        Indent(h/2 - 7 - strlen(num) - 1);
        printf("%s%*s", num, 8, "");
        free(num);

        for (int j = 0; j < gridSize; j++)
        {
            char * color = WHITE;

            if (grid[i-1][j] == HIT) color = RED;
            printf("%s", color);
            printf("%c", grid[i-1][j]);
            printf(RESET);
            for (int l = 0; l < (TopNumLen); l++)
            {
                printf(" ");
            }
            
        }
        Println("");
    }
    Println("");

}

void DisplayIntGrid(int ** grid, int gridSize){

    //I need to calculate proper indentation between grid squares. It depends on the length of the numerals:
    int base = endingCoordinate_1 - startingCoordinate_1 + 1;
    int TopNumLen = 0;
    int k = gridSize;
    do
    {
        TopNumLen++;
        k/=base;
    } while (k > 0);
    

    printf("\n");
    int h = (getConsoleWidth() - (TopNumLen * gridSize) - gridSize);
    Indent(h/2);

    //I need a way to convert integer to numeral according to my numeral system.
    for (int i = 0; i < gridSize; i++)
    {
        char* num = alloc_IntegerToNumeral(i, gridSize, startingCoordinate_1, endingCoordinate_1);
        printf("%s ", num);
        free(num);
    }

    Println("");

    for (int i = 1; i <= gridSize; i++)
    {
        char* num = alloc_IntegerToNumeral(i, gridSize, startingCoordinate_2, endingCoordinate_2);
        
        Indent(h/2 - 7 - strlen(num) - 1);
        printf("%s%*s", num, 8, "");
        free(num);

        for (int j = 0; j < gridSize; j++)
        {
            char * color = WHITE;

            if (grid[i-1][j] == HIT) color = RED;
            printf("%s", color);
            printf("%d", grid[i-1][j]);
            printf(RESET);
            for (int l = 0; l < (TopNumLen); l++)
            {
                printf(" ");
            }
            
        }
        Println("");
    }
    Println("");

}

int IsShipChar(char c){

    return (c == BATTLESHIP_C || c == SUBMARINE_C || c == DESTROYER_C || c == CARRIER_C);

}

/**
 * Does the same job as DisplayGrid() function but hides ships on the gird and shows misses according to an integer passed.
 */
void DisplayOpponentGrid(char ** grid, int gridSize, int showMiss){

    //I need to calculate proper indentation between grid squares. It depends on the length of the numerals:
    int base = endingCoordinate_1 - startingCoordinate_1 + 1;
    int TopNumLen = 0;
    int k = gridSize;
    do
    {
        TopNumLen++;
        k/=base;
    } while (k > 0);

    //I need a way to convert integer to numeral according to my numeral system.
    printf("\n");
    int h = (getConsoleWidth() - (TopNumLen * gridSize) - gridSize);
    Indent(h/2);
    for (int i = 0; i < gridSize; i++)
    {
        char* num = alloc_IntegerToNumeral(i, gridSize, startingCoordinate_1, endingCoordinate_1);
        printf("%s ", num);
        free(num);
    }

    Println("");

    for (int i = 1; i <= gridSize; i++)
    {
        char* num = alloc_IntegerToNumeral(i, gridSize, startingCoordinate_2, endingCoordinate_2);
        Indent(h/2 - 7 - strlen(num) - 1);
        printf("%s%*s", num, 8, "");
        free(num);

        for (int j = 0; j < gridSize; j++)
        {

            char * color = WHITE;

            if (grid[i-1][j] == HIT) color = RED;

            printf("%s", color);

            char c = grid[i-1][j];

            if (!IsShipChar(c)){
                if (c == MISS){
                    if (showMiss == 0){
                        printf("%c", WATER_C);
                        continue;
                    } 
                    else {
                        printf("%c", c);
                    }
                }
                else{
                    printf("%c", c);
                }
            }
            else {
                printf("%c", WATER_C);
            }

            //Indent the correct amount of times:
            for (int l = 0; l < (TopNumLen); l++)
            {
                printf(" ");
            }
            
        }
        Println("");
    }
    Println("");

}



//I hate this function, for some reason I debugged it for 2 hrs
int countSunkShips(Player* player){

    int res = MAX(0, checkIfSunk(player, &(player->battleshipBounds)));
    res += MAX(0, checkIfSunk(player, &(player->carrierBounds)));
    res += MAX(0, checkIfSunk(player, &(player->destroyerBounds)));
    res += MAX(0, checkIfSunk(player, &(player->submarineBounds)));

    return MAX(0,res);

}

int checkIfSunk(Player *player, ShipBounds *shipBounds) {

    for (int i = shipBounds->startRow; i <= shipBounds->endRow; i++) {
        for (int j = shipBounds->startCol; j <= shipBounds->endCol; j++) {
            if (player->grid[i][j] != HIT) {
                shipBounds->IsSunk=0;
                return -1;
            }
        }
    }
    shipBounds->IsSunk=1;
    return 1;
}


void ShowPlayerStats(Player * player){
    Println("");
    char * txt = CreateString_alloc(2, player->name, "'s STATS:");
    Println_Centered(txt, strlen(txt), player->UIColor);

    free(txt);

    int shipsLeft = 4 - player->currSunkShips;

    Print_Centered("Ships left: ", strlen("Ships left: 1"), player->UIColor);
    
    SetColor(player->UIColor);
    printf("%d", shipsLeft);
    ResetFormat();
    Println("");

    Print_Centered("Artillery: ", strlen("Artillery: 1"), player->UIColor);
    SetColor(player->UIColor);
    printf("%d", player->prevSunk);
    ResetFormat();
    Println("");

}

void Show_2_PlayerStats(Player * player, Player * opponent){
    ShowPlayerStats(player);
    Print_Centered("Torpedo: ", strlen("Torpedo: 1"), player->UIColor);
    SetColor(player->UIColor);
    printf("%d", (player->prevSunk && opponent->currSunkShips >= 3)?(1):0);
    ResetFormat();
    Println("");

    Println("");
    ShowPlayerStats(opponent);
}