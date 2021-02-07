#include "AI.h"



AI::AI(Player* opponent, Player* controlled, Game* game) 
    : opponent(opponent), controlled(controlled),game(game) {
}

AI::~AI() {
    delete this;
}

int AI::makeTurn() {
    int storageCount = 5;
    // move tuple: [0] = row it's stored in, [1] = tile, [2] = number of tiles pulled when this move is used
    std::vector<std::tuple<int,Tile,int> > moves = game->availableTiles(controlled);
    bool foundValidLocation = false;
    int storageRow = 1; 

    //the tuple currently having it's data checked to see if there's a valid move
    std::tuple<int,Tile,int> currentTuple = moves[0];

    //tracks the index of the current tuple
    unsigned int tupleCounter = 0;

    /*make an intelligent move
     * 1: Prevent human player from completing a row
     * 2: Try and complete a bots own row
     */


    

    Player* players[] = {opponent, controlled};
    bool foundValidRow = true;
    for(Player* p: players) {
        for(int i = 0;i<storageCount;++i) {
            //if the storage row is not empty
            if(p->getStorageCell(i,0) != NO_TILE && !foundValidLocation) {
                Tile possibleTile = p->getStorageCell(i,0);
                //initalise the number of tiles needed at the number already in the row (+1 as it takes in the displayed number, not the indexed number)
                int numberOfTilesNeeded = p->countStorage(i + 1,possibleTile);
                //the actual number needed is the row num (+1 as this is indexed from 0) - number already in the row
                numberOfTilesNeeded = i + 1 - numberOfTilesNeeded;

                //if the row is not already full
                if(numberOfTilesNeeded > 0) {
                    
                    for(std::tuple<int,Tile,int> move:moves) {
                        //if a move is found
                        if(std::get<1>(move) == possibleTile && std::get<2>(move) == numberOfTilesNeeded) {
                            //set the current move as the move to make later in the method
                            currentTuple = move; 

                            //if denying the opponent, a valid storage still needs to be found 
                            if(p == opponent) {
                                foundValidRow = false;
                            }
                            //otherwise the bot is completing one of it's own rows, so it can just set the current storage as the one to insert to
                            else {
                                storageRow = i + 1; 
                            }

                            //prevent further changes to the move
                            foundValidLocation = true;
                        }                       
                    }
                }
            }
        }
    }

    if(!foundValidRow) {
        for(int i = 0;i<storageCount;++i) {
            Tile t = controlled->getStorageCell(i,0);
            if(t == std::get<1>(currentTuple) && controlled->countStorage(i + 1,t) < i + 1) {
                storageRow = i + 1;
                foundValidRow = true;
            }
        }
    }
    if(!foundValidRow) {
        for(int i = 0;i<storageCount;++i) {
            Tile t = controlled->getStorageCell(i,0);
            if(t == NO_TILE && controlled->mosaicRowHasTile(storageRow,std::get<1>(currentTuple)) == false && !foundValidRow) {
                storageRow = i + 1;
                foundValidRow = true;
            }
        }
    }
    
    //if no intelligent move can be made, make a random move
    while(!foundValidLocation) {
        Tile toCheck = std::get<1>(currentTuple);

        /*check the two conditions to make the turn valid, being that the storage doesn't already have a different tile in it
        * and that the corresponding mosaic row doesn't already have the tile filed in. 
        * Checking getStorageCell at storageRow-1,0 is the only check needed as if this spot doesn't contain a tile then the rest
        * of it won't.
        */
        if(controlled->mosaicRowHasTile(storageRow,toCheck) == false && controlled->getStorageCell(storageRow - 1,0) == NO_TILE) {
            foundValidLocation = true;
        }
        else {
            storageRow++;
        }

        // if all storage rows have been scanned and nothing has been found, move to the next tuple
        if(storageRow > FACTORIES) {
            ++tupleCounter;
            // if there are still more possible moves to check
            if(tupleCounter < moves.size()) {
                currentTuple = moves[tupleCounter];
                storageRow = 1;
            }
            //if no random move can be made move to broken
            else {
                foundValidLocation = true;
                storageRow = 6;
            }
        }
    }
    std::cout << "The bot made this move: turn " << std::get<0>(currentTuple) << " " << std::get<1>(currentTuple) << " " << storageRow << std::endl;
    this->game->turn(controlled,std::get<0>(currentTuple),std::get<1>(currentTuple),storageRow);
    return 0;
}