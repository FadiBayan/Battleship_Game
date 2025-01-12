#include "../include/Bot.h"
#include "../include/Attacks.h"
#include "../include/Player.h"
#include "../include/ShipPlacement.h"
#include "../include/CalcProbs.h"

#include <time.h>


void BotSmartAttack(Player * bot, Player * opponent){

    start:

    //First we must check if the stack is empty:
    if (is_empty(bot->stackMemory)){

        printf("stack empty, continuing pattern: \n");

        //Normal Randomized Probabilistic Pattern:

        //First: Get the next target
        
        int row = 0;
        int col = 0;


        if (bot->riskFactor < HIGH_RISK){
            printf("bot.lowProbCatgSize = %d\n", bot->lowprobArrSize);
            printf("bot.avgProbCatgSize = %d\n", bot->avgprobArrSize);
            printf("bot.highProbCatgSize = %d\n", bot->highprobArrSize);
            int categorySize = (bot->currTargetCategory == 0)?bot->lowprobArrSize:(bot->currTargetCategory == 1)?bot->avgprobArrSize:bot->highprobArrSize;

            while (categorySize == 0)
            {
                bot->currTargetCategory += 1;
                bot->currTargetCategory %= PROB_CATEGORYCOUNT;
                categorySize = (bot->currTargetCategory == 0)?bot->lowprobArrSize:(bot->currTargetCategory == 1)?bot->avgprobArrSize:bot->highprobArrSize;
            }

            //printf("currTargetCatg: %d\n", bot->currTargetCategory);

            for (int i = 0; i < categorySize; i++)
            {
                InOrderTraversal_ProbNode_Print(opponent->probabilityHeapArray[bot->currTargetCategory][0]);

            }
            
            
                        

            int getCoord = GetRandomProbCoordinateFromCategory(&row, &col, bot->probabilityHeapArray[bot->currTargetCategory], categorySize);
            printf("got random coord %d,%d\n", row, col);
            printf("getCoord: %d\n", getCoord);
            bot->currTargetCategory += 1;
            bot->currTargetCategory %= PROB_CATEGORYCOUNT; //Keeping track of the next category to target.

        }
        else {
            if (bot->riskFactor == EXTREMELYHIGH_RISK){
                GetHighestProbability(&row, &col, opponent);
            }
            else {
                GetRandomHighestProbCell(&row, &col, opponent);
            }
        }


        //Now I must attack the target:
        #pragma region [Firing]
        //For now, we'll stick to the Fire() function:

        //Transform the indexes to coordinates: because attack methods take in user coords

        char * error = NULL;

        int fire = BotFireHelper(row, col, bot, opponent);


        if (error != NULL) free(error);


        #pragma endregion

    }
    else {

        //In this case the stack is non empty, so we must pop out from it and perform the required task.


        //When it's a success (a HIT), then we must add new stuff onto the stack. Whether the stuff added are at the top or at the bottom depends on
        //their importance.

        //There's a costy path of operations when sinking a ship, we'll have to check the nodes surrounding the ship

        doTask:

        if (is_empty(bot->stackMemory)){
            goto start;
        }

        BotTask * topTask = (BotTask*)removeFirst(bot->stackMemory);
        int res = PerformTask(topTask);

        //if res = 0 then the task was either invalid or 

        if (res <= 0){
            freeTask(topTask);
            goto doTask;
        }

    }
}


#pragma region [Accessing Probability Cells]
/**
 * Input:
 *      - heapCategory: The binomial heap array that the function will randomly choose from.
 *      - categorySize
 * 
 * Output:
 *      - row: the address where the function will store the row index
 *      - col: the address where the function will store the column index
 */
int GetRandomProbCoordinateFromCategory(int * row, int * col, BinomialHeap ** heapCategory, int categorySize){

    if (categorySize <= 0){
        Println_Centered("Invalid array size for heapCategory!", strlen("Invalid array size for heapCategory!"), RED);
        return -1;
    }

    if (row == NULL || col == NULL){
        Println_Centered("Cannot return coordinate from category when row or column addresses are NULL.", strlen("Cannot return coordinate from category when row or column addresses are NULL."), RED);
        return -1;
    }
    if (heapCategory == NULL){
        Println_Centered("Cannot pick probability from binomial heap. Heap is NULL.", strlen("Cannot pick probability from binomial heap. Heap is NULL."), RED);
        return -1;
    }

    srand(time(0));
    int randIndex = rand() % categorySize;
    void * elem = findMin(heapCategory[randIndex]);

    printf("max prob in heap: %d\n", ((int*)elem)[0]);
    printf("hmm: %d\n", *(int*)(heapCategory[randIndex]->head->rightSibling->value));

    *row = ((int*)elem)[1];
    *col = ((int*)elem)[2];

    return 1;

}

/**
 * This function will return a random grid cell coordinate from the highest probability category that is nonempty.
 * 
 * Output: 
 *      - integer array of i and j coordinates
 */
int GetRandomHighestProbCell(int * row, int* col, Player * player){

    //First we check if the high-probability category contains anything:
    if (player->highprobArrSize > 0){
        GetRandomProbCoordinateFromCategory(row, col, player->probabilityHeapArray[3], player->highprobArrSize);
    }
    else if (player->avgprobArrSize > 0){
        GetRandomProbCoordinateFromCategory(row, col, player->probabilityHeapArray[2], player->avgprobArrSize);
    }
    else if (player->lowprobArrSize > 0){
        GetRandomProbCoordinateFromCategory(row, col, player->probabilityHeapArray[1], player->lowprobArrSize);
    }
    else {
        printf("Something must have gone wrong. All grid cells have 0 probability.\n");
        return -1;
    }

    return 1;
}


/**
 * This function searches in the specified probability category array and stores the row and column coordinates of the highest probability cell
 * in row and col respectively.
 */
int GetHighestProbCoordinateFromCategory(int * row, int * col, BinomialHeap ** heapCategory, int categorySize){

    if (categorySize <= 0){
        Println_Centered("Invalid array size for heapCategory!", strlen("Invalid array size for heapCategory!"), RED);
        return -1;
    }

    if (row == NULL || col == NULL){
        Println_Centered("Cannot return coordinate from category when row or column addresses are NULL.", strlen("Cannot return coordinate from category when row or column addresses are NULL."), RED);
        return -1;
    }
    if (heapCategory == NULL){
        Println_Centered("Cannot pick probability from binomial heap. Heap is NULL.", strlen("Cannot pick probability from binomial heap. Heap is NULL."), RED);
        return -1;
    }

    int highestIndex = 0;

    for (int i = 0; i < categorySize; i++)
    {
        if (heapCategory[i]->compare(findMin(heapCategory[i]), findMin(heapCategory[highestIndex])) < 0){//Searching for the highest probability
            highestIndex = 0;
        }
    }

    void * res = findMin(heapCategory[highestIndex]);

    *row = ((int*)res)[1];
    *col = ((int*)res)[2];

    return 1;
    

}

/**
 * This function searches across all probability categories for the highest probability cell and saves its coordinate values in row and col.
 */
int GetHighestProbability(int * row, int* col, Player * player){

    //First we check if the high-probability category contains anything:
    if (player->highprobArrSize > 0){
        GetHighestProbCoordinateFromCategory(row, col, player->probabilityHeapArray[3], player->highprobArrSize);
    }
    else if (player->avgprobArrSize > 0){
        GetHighestProbCoordinateFromCategory(row, col, player->probabilityHeapArray[2], player->avgprobArrSize);
    }
    else if (player->lowprobArrSize > 0){
        GetHighestProbCoordinateFromCategory(row, col, player->probabilityHeapArray[1], player->lowprobArrSize);
    }
    else {
        printf("Something must have gone wrong. All grid cells have 0 probability.\n");
        return -1;
    }

    return 1;
}

#pragma endregion

/**
 * This function updates a specified binomial heap by updating each element's probability and re-inserting it into the heap.
 * 
 * Input:
 *      - player: the player of which a heap will be updated
 *      - heapIndex: the index of the heap that will be updated. This index should be a valid index within the size of the heap hashmap.
 */
int UpdateHeap(Player * player, int heapIndex){

    if (heapIndex >= PROB_REGION_COUNT || heapIndex < 0) return -1;

    //First I need to intialize a temporary binomial heap:
    BinomialHeap temp;
    temp.head = NULL;
    temp.compare = player->probabilityHeapSet[heapIndex]->compare;

    BinomialHeap * targetHeap = player->probabilityHeapSet[heapIndex];

    while (targetHeap->head != NULL)
    {
        int * element = (int*)deleteMin(targetHeap);

        int row = element[1];
        int col = element[2];

        //Updating the probability to the new one after probGrid got updated:
        element[0] = player->probabilityGrid[row][col];

        //Now I need to make sure that the probability is not 0 and the coordinate is neither a hit nor a miss:
        if (element[0] > 0 && player->grid[row][col] != MISS && player->grid[row][col] != HIT){
            insert(&temp, element);
        }
    }
    
    //Finally, connect the head of the temporary heap to the initial, now empty, heap:
    targetHeap->head = temp.head;

    return 1;
}


/**
 * This function will scan an area on the graph and update, once only, the binomial heap tied to each probability region.
 */
int UpdateHeapsWithinBounds(Player * player, int row0, int row1, int col0, int col1){

    //I could keep track of every region I've already updated so that I don't update it again.
    //But I don't see no easy way of doing that without using a data structure
    //Or, better way: I could save the hash indices of the regions updated and check if the array contains the one I'm trying to update

    //Even better way: I could set up a coordinate point that jumps between regions. I can access the region size from the PROB_REGION_SIZE macro.
    //First I initialize the coordinate point to be the top right of the target area. Then I add to the index (region width + 1) 
    //in the horizontal direction and update until it goes out of bounds of the target area.
    //I do the same vertically.

    int curr[2] = {row0, col0};

    int regionSize[2] = PROB_REGION_SIZE;

    
    while (IndexWithinRange(curr[0])){

        while (IndexWithinRange(curr[0]) && IndexWithinRange(curr[1])){
            UpdateHeap(player, HashRegion(curr[0], curr[1]));
            curr[1] += regionSize[1] + 1;
        }
        curr[1] = col0;
        curr[0] += regionSize[0] + 1;

    }
        

}






/**
 * This function assigns a new task. It creates a new task structure, fills it with a pointer to the function it must perform and with the arguments
 * necessary. It then pushes the new task onto the bot's stack memory.
 */
BotTask * CreateTask(int (*function)(void**), void** arguments, int argumentCount, void** flags, int flagCount){

    BotTask * task = (BotTask*)(malloc(sizeof(BotTask)));

    task->arguments = arguments;
    task->argumentCount = argumentCount;
    task->flags = flags;
    task->flagCount = flagCount;


}

int freeTask(BotTask * task){

    if (task->arguments != NULL){
        for (int i = 0; i < task->argumentCount; i++)
        {
            free(task->arguments[i]);
        }
    }

    if (task->flags != NULL){
        for (int i = 0; i < task->flagCount; i++)
        {
            free(task->flags[i]);
        }
        
    }

    free(task);    

}

int AssignNewTask(int priorityFlag,  Player * bot, int (*function)(void**), void** arguments, int argumentCount, void** flags, int flagCount){

    BotTask * task = CreateTask(function, arguments, argumentCount, flags, flagCount);

    switch (priorityFlag)
    {
    case TASKFLAG_HIGHPRIORITY:
        //We add the task at the top of the stack for high importance
        addFirst(bot->stackMemory, task);
        break;

    case TASKFLAG_LOWPRIORITY:
        //We add the task at the bottom of the stack for low importance
        addLast(bot->stackMemory, task);
        break;
    
    default:
        return 0;
    }


    return 1;


}


BotTask * GetNextTask(Player * bot){

    if (is_empty(bot->stackMemory)) return NULL;

    return (BotTask*)removeFirst(bot->stackMemory);

}

/**
 * This function takes in a BotTask, performs it and returns an integer showing whether the task was successfully performed or not.
 * 
 * Output:
 *      - The function returns 1 if task was successful
 *      - It returns 0 if task was not successful
 * 
 */
int PerformTask(BotTask * task){

    /**
     * We must call the function specified in the task and pass it the arguments 
     */

    if (task->function == NULL) return 0;
    
    return task->function(task->arguments);

}

#pragma region [Bot Firing Systems]
/**
 * Input:
 *      - int row: the row number of the targeted cell
 *      - int col: the column number of the targeted cell
 *      - Player * bot
 *      - Player * opponent
 */
int BotFire(void** args){

    if (args == NULL) return 0;

    int row = *(int*)args[0];
    int col = *(int*)args[1];

    Player * bot = (Player*)args[2];

    Player * opponent = (Player*)args[3];

    BotFireHelper(row, col, bot, opponent);

}

int BotFireHelper(int row, int col, Player * bot, Player * opponent){

    char * coords = alloc_GetCoordsFromIndices(row, col, GRIDSIZE, startingCoordinate_1, startingCoordinate_2,
     endingCoordinate_1, endingCoordinate_2, coord_1_shift, coord_2_shift);

    startfire:

    char * error = NULL;
    int fireRes = Fire(coords, opponent->grid, DifficultyValue, &error);


    if (fireRes < 0){
        if (error != NULL) free(error);

        goto startfire;
    }


    int target[2] = {row,col};


    //Updating the probability distribution:
    UpdateSurroundingProbabilities(opponent, target);

    
    //Update the probability heaps for all the probability regions that surround the target.
    UpdateHeapsWithinBounds(opponent, MAX(0, row - LONGEST_SHIPLENGTH), MIN(GRIDSIZE, row + LONGEST_SHIPLENGTH),
     MAX(0, col - LONGEST_SHIPLENGTH), MIN(GRIDSIZE, col + LONGEST_SHIPLENGTH));

    //Need to check if the target was a HIT or a MISS. If it's a HIT then we assign 4 new tasks to target the surrounding cells:
    if(opponent->grid[row][col] == HIT){
        if (IndexWithinRange(row + 1)){
            if (opponent->grid[row+1][col] != HIT && opponent->grid[row+1][col] != MISS){

                int argCount = 4;
                void ** args = (void**)(malloc(sizeof(void*) * argCount));

                for (int i = 0; i < argCount; i++)
                {
                    args[i] = (void*)(malloc(sizeof(void*)));
                }
                
                int* rowPtr = (int*)(malloc(sizeof(int)));
                *rowPtr = row + 1;
                int* colPtr = (int*)(malloc(sizeof(int)));
                *colPtr = col;

                //Setting the arguments:
                args[0] = rowPtr;
                args[1] = colPtr;
                args[2] = bot;
                args[3] = opponent;
                

                AssignNewTask(TASKFLAG_HIGHPRIORITY, bot, BotFire, args, argCount, NULL, 0);
            }
        }

        if (IndexWithinRange(row - 1)){
            if (opponent->grid[row-1][col] != HIT && opponent->grid[row-1][col] != MISS){
                int argCount = 4;
                void ** args = (void**)(malloc(sizeof(void*) * argCount));

                for (int i = 0; i < argCount; i++)
                {
                    args[i] = (void*)(malloc(sizeof(void*)));
                }
                
                int* rowPtr = (int*)(malloc(sizeof(int)));
                *rowPtr = row - 1;
                int* colPtr = (int*)(malloc(sizeof(int)));
                *colPtr = col;

                //Setting the arguments:
                args[0] = rowPtr;
                args[1] = colPtr;
                args[2] = bot;
                args[3] = opponent;
                

                AssignNewTask(TASKFLAG_HIGHPRIORITY, bot, BotFire, args, argCount, NULL, 0);
            }
        }

        if (IndexWithinRange(col + 1)){
            if (opponent->grid[row][col+1] != HIT && opponent->grid[row][col+1] != MISS){
                int argCount = 4;
                void ** args = (void**)(malloc(sizeof(void*) * argCount));

                for (int i = 0; i < argCount; i++)
                {
                    args[i] = (void*)(malloc(sizeof(void*)));
                }
                
                int* rowPtr = (int*)(malloc(sizeof(int)));
                *rowPtr = row;
                int* colPtr = (int*)(malloc(sizeof(int)));
                *colPtr = col + 1;

                //Setting the arguments:
                args[0] = rowPtr;
                args[1] = colPtr;
                args[2] = bot;
                args[3] = opponent;
                

                AssignNewTask(TASKFLAG_HIGHPRIORITY, bot, BotFire, args, argCount, NULL, 0);
            }
        }

        if (IndexWithinRange(col - 1)){
            if (opponent->grid[row][col-1] != HIT && opponent->grid[row][col-1] != MISS){
                int argCount = 4;
                void ** args = (void**)(malloc(sizeof(void*) * argCount));

                for (int i = 0; i < argCount; i++)
                {
                    args[i] = (void*)(malloc(sizeof(void*)));
                }
                
                int* rowPtr = (int*)(malloc(sizeof(int)));
                *rowPtr = row;
                int* colPtr = (int*)(malloc(sizeof(int)));
                *colPtr = col - 1;

                //Setting the arguments:
                args[0] = rowPtr;
                args[1] = colPtr;
                args[2] = bot;
                args[3] = opponent;
                

                AssignNewTask(TASKFLAG_HIGHPRIORITY, bot, BotFire, args, argCount, NULL, 0);
            }
        }
    }

    return fireRes;

}

#pragma endregion



//Later on, ship placement will depend on recorded data on previous players.
//The bot will write to a folder records for each player under the player's unique name.
//Every new game, the bot accesses the player's record and analyzes the player's tactics from previous games, his attacking patterns
//and tries to place ships in accordingly.
#pragma region [BOT PLACEMENT]

int BotPickRandomIndicesWithinBounds(int * outRow, int * outCol, char** grid, int shipSize[2], char* orientation, int startRow, int endRow, int startCol, int endCol){
    

    start:
    int rowMax = endRow, colMax = endCol;

    

    if (strcmpi(orientation, "h") == 0){
        colMax -= shipSize[0];
    }
    else {
        rowMax -= shipSize[0];
    }

    printf("startRow = %d, rowMax = %d\n", startRow, rowMax);

    int row = startRow + (rand() % rowMax);
    int col = startCol + (rand() % colMax);

    printf("%d,%d\n", row, col);

    char * coords = alloc_GetCoordsFromIndices(row, col, GRIDSIZE, startingCoordinate_1, startingCoordinate_2, endingCoordinate_1, endingCoordinate_2, coord_1_shift, coord_2_shift);

    printf("%s\n",coords);
    printf("%s\n",orientation);
    
    char * error = NULL;

    int * shipBounds = alloc_GridAreaFromInput(coords, orientation, shipSize[0] - 1, shipSize[1], &error);

    if (error != NULL) printf("%s\n", error);
    
    printf("huhh\n");
    printf("%d, %d,%d,%d\n", shipBounds[0], shipBounds[1], shipBounds[2], shipBounds[3]);

    if (CheckForOverlap(grid, shipBounds)){
        printf("there was an overlap\n");
        free(coords);
        free(shipBounds);
        if (error != NULL) free(error);
        goto start;
    }

    *outRow = row;
    *outCol = col;

    free(coords);
    free(shipBounds);
    if (error != NULL) free(error);

    return 1;
}



void PlaceBotShips(Player *bot) {

    srand(time(0));

    char shipTypes[] = {SUBMARINE_C, DESTROYER_C, BATTLESHIP_C, CARRIER_C};
    int ShipSizes[SHIPCOUNT][2] = {{2,0}, {3,0}, {4,0}, {5,0}};

    for (int i = 0; i < SHIPCOUNT; i++) {
        char orientation[2];
        int horizontal = rand() % 2;

        // Set orientation to 'H' for horizontal or 'V' for vertical
        orientation[0] = horizontal ? 'h' : 'v';
        orientation[1] = '\0';

        int row;
        int col;

        BotPickRandomIndicesWithinBounds(&row, &col, bot->grid, ShipSizes[i], orientation, 0, GRIDSIZE - 1, 0, GRIDSIZE - 1);

printf("%d,%d\n", row, col);
        // Convert to user coordinates
        char * coords = alloc_GetCoordsFromIndices(row, col, GRIDSIZE, startingCoordinate_1, startingCoordinate_2, endingCoordinate_1, endingCoordinate_2, coord_1_shift, coord_2_shift);
        
printf("DSA_2\n");
        printf("%d, %d\n", ShipSizes[i][0], ShipSizes[i][1]);
        printf("%s\n", coords);
        // Try to place the ship
        char * error = NULL;
        PlaceShipOnGridHelper(bot, shipTypes[i], bot->grid, coords, orientation, ShipSizes[i], &error);

        printf("%s\n", error);

        printf("%c\n", coords[0]);
        
        free (coords);
        if(error != NULL) free(error);
        
        DisplayGrid(bot->grid, GRIDSIZE);
        
        char c;

        scanf("%c", &c);
    }
}

#pragma endregion