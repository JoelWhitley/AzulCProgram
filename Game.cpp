#include "Game.h"
#include "SaveAndLoad.h"
#include "AI.h"


Game::Game(Player* p1, Player* p2, int seed, bool isAI) : p1(p1), p2(p2) {
    pile = new LinkedList();
    tileBag = new LinkedList();
    boxLid = new LinkedList();
    playerWithFPTile = p1;
    currentPlayer = playerWithFPTile;
    this->isAI = isAI;
}
Game::~Game() {
    delete this;
}

void Game::setupRound() {    
    pile->addFront(FIRST_PLAYER);   
    this->currentPlayer = playerWithFPTile;    
    generateFactories();
}

void Game::generateTileBag(int seed){

    if(seed==0){
        //TODO: randomly distributed
        //currently sequential
        for(int tile=0; tile<GAME_TILES; tile++){
            tileBag->addBack(tileSelection[tile%SIZE]);
        }
    }
    else {
        //specific seed generation
        for(int tile=0; tile<GAME_TILES; tile++){
            int newSeed = seed*tile;
            tileBag->addBack(tileSelection[newSeed%SIZE]);
        }
    }

}

void Game::generateFactories() {
    for(int i=0; i<FACTORIES; ++i) {
        for(int j=0; j<FACTORY_SIZE; ++j) {
            this->factories[i][j] = getTileFromBag();
        }
    }
}

Tile Game::getTileFromBag(){    
    if(tileBag->getSize()==0){
        emptyLidIntoBag();
    }
    if(tileBag->getSize()>0){
        Tile first = tileBag->get(0);
        tileBag->removeFront();
        return first;
    }
    else {
        return NO_TILE;
    }
}

void Game::emptyLidIntoBag(){
    Tile currTile;
    for(int i=0; i<boxLid->getSize(); i++){
        currTile = boxLid->get(0);
        tileBag->addBack(currTile);
        boxLid->removeFront();
    }
}

Tile Game::randomTile() {    
    int ran = rand() % SIZE;
    return tileSelection[ran];
}

//-------------------GAMEPLAY LOOP LOGIC-----------------

void Game::play() {
    saved=true;
    gameEnd = false;
    
    
    while(!gameEnd){
        round();
    }
    std::cout << "Game over." << std::endl;    
}

void Game::round() {
    AI* aiController = nullptr;
    if(this->isAI) {
        aiController = new AI(p1,p2,this);
    }
    bool exit = false;
    bool roundEnd = false;
    if(!resumed){
        setupRound();
    }
    resumed = false;

    std::cout << "\n---FACTORY OFFER PHASE---\n" << std::endl;
    
    while(!roundEnd) {         
        printFactories();
        std::cout << std::endl;
        
        if(this->currentPlayer == p1) {
            printBoard(this->currentPlayer,p2);
        }
        else {
            printBoard(this->currentPlayer,p1);
        }

        int outcome = OUTCOME_INVALID;
        if(!this->isAI) {
            outcome = turnPrompt(this->currentPlayer);
        }
        else {
            if(currentPlayer == p1) {
                outcome = turnPrompt(this->currentPlayer);
            }

            // put ai implementation here
            else {
                outcome = aiController->makeTurn();
            }
        }       
        if(outcome == OUTCOME_TURNSUCCESS) {
            switchPlayer();
            std::cout << "\nSuccess! It's " << this->currentPlayer->getName() << "'s turn now!" << std::endl;
            std::cout << std::endl;
            
        }
        else if(outcome == OUTCOME_TURNFAIL){ 
            std::cout << "Turn failed, please try something else.\n" << std::endl;
        }  
        else if(outcome == OUTCOME_EXIT){
            exit = true;
            roundEnd = true;
        }
        if(!roundEnd){
            roundEnd = checkRoundEnd();
        }          
    }

    if(exit){
        gameEnd = true;
    }
    else {
        std::cout << "---FACTORIES DEPLETED!---" << std::endl;
        std::cout << "---WALL TILING PHASE---" << std::endl;
        std::cout << "SCORES FOR " << p1->getName() << ":" << std::endl;
        moveTiles(p1); 
        std::cout << "SCORES FOR " << p2->getName() << ":" << std::endl; 
        moveTiles(p2);
        printBoard(p1,p2);
        
        endRound(); 
    }

}

void Game::endRound(){

    //empty broken tiles into boxLid, make note of who has the FP tile for next round
    //while there, check for endgame condition in each players' mosaic
    Player* players[] = {p1, p2};
    for(Player* player : players){
        for(int i=0; i<player->getBroken()->getSize(); i++){
            if(player->getBroken()->get(i)==FIRST_PLAYER){
                playerWithFPTile = player;
                player->getBroken()->removeNodeAtIndex(i);
            }
            else {
                boxLid->addBack(player->getBroken()->get(i));
            }
        }
        player->getBroken()->clear();
        if(checkGameEnd(player)){
            gameEnd = true;
        }
    }

    std::cout << "---END OF ROUND---" << std::endl;

    if(gameEnd){
        endGame();     
    }  

}

void Game::endGame(){

    std::cout << "!!! ROW COMPLETED !!!\n";
    if(!isTie){
        winner = p1->getPoints()>p2->getPoints()?p1:p2;
        std::cout << "!!! " << winner->getName() << " wins with " << winner->getPoints() << " points !!!" << std::endl;
    }    
    else{
        std::cout << "!!! GAME TIED !!!";
    }

}

bool Game::turn(Player* p, int factory, Tile tile, int row) {

    saved = false;
    //TODO: prevent adding tiles to storage when that type of tile already exists in that row of the mosaic

    std::vector<Tile> found;
    bool isValid = false;
    bool compatibleTileRow = false;
    int roomInRow = 0;
    bool validRow = true;

    //amount of specified tile in specified factory
    int matchingTiles = matchingTilesInFactory(factory, tile);
    
    if(row != FLOOR_ROW){
        compatibleTileRow = p->countStorage(row,tile) >= 0;
        roomInRow = row-p->countStorage(row,tile);  
        validRow = compatibleTileRow && roomInRow>0 && !p->mosaicRowHasTile(row, tile);
    }    

    if((matchingTiles>0) && (tile!=FIRST_PLAYER) && validRow){ 
        //if specified factory is orbiting/standard (not middle pile)
        if((factory>0) && (factory<=FACTORIES)){ 
            for(int i=0; i<FACTORY_SIZE; ++i){
                //if current tile is the player's chosen one
                if(this->factories[factory-1][i] == tile){
                    if(row==FLOOR_ROW){ 
                        found.push_back(factories[factory-1][i]);
                    }
                    //prepare specified tiles to be sent to storage (if there's room)
                    else if(roomInRow > 0) {
                        found.push_back(factories[factory-1][i]);
                        roomInRow--;
                    }
                    //add excess to broken tiles
                    else { 
                        p->getBroken()->addBack(factories[factory-1][i]);
                    }
                }                
                //add other tiletypes to pile         
                else {  
                    this->pile->addBack(this->factories[factory-1][i]);
                }
                this->factories[factory-1][i] = NO_TILE;
            }
            isValid = true;
        }
        //if specified factory is middle pile
        else if(factory == 0){
            int counter = 0;
            //move first_player tile to player's floor (if it's there)
            if(this->pile->get(0) == FIRST_PLAYER){
                p->addToBroken(FIRST_PLAYER);
                this->pile->removeFront();
                counter++;
            }
            //add specified tiles to "found"
            for(int i=counter; i-counter < this->pile->getSize(); ++i){
                int adjustedCount = i-counter;
                if(this->pile->get(adjustedCount) == tile){
                    if(row==FLOOR_ROW){                    
                        found.push_back(this->pile->get(adjustedCount));
                        this->pile->removeNodeAtIndex(adjustedCount);
                        counter++;
                    }
                    //if there's still room in specified row, send it there
                    else if(roomInRow > 0) {
                        found.push_back(this->pile->get(adjustedCount));
                        this->pile->removeNodeAtIndex(adjustedCount);
                        roomInRow--;
                        counter++;
                    }
                    //if not, send to floor
                    else {
                        p->getBroken()->addBack(this->pile->get(adjustedCount));
                        this->pile->removeNodeAtIndex(adjustedCount);
                        counter++;
                    }
                }
                
                
            }
            isValid = true;
        }
        if(isValid){
            p->addToStorage(row,found,boxLid);
        }    
    }
    return isValid;

}

//--------------END OF GAMEPLAY LOOP LOGIC-----------------

int Game::turnPrompt(Player* p){

    int outcome = -1;
    std::string input;
    std::string command;
    int factory;
    Tile tile;
    int row;
    
    std::cout << "it is " << p->getName() << "'s turn: \n";

    //take input string, split into args
    input = userInput();
    std::stringstream(input) >> command >> factory >> tile >> row;
    std::stringstream(input).clear();
    std::stringstream(input).ignore();
       
    if(command=="exit" || command=="EXIT"){
        outcome = OUTCOME_EXIT;
        if(!saved){
            std::cout << "Would you like to save? (y/n):\n";
            std::string decision = userInput();
            while(decision != "Y" && decision != "y" && decision != "N" && decision != "n"){
                std::cout << "Please try again.\n";
                decision = userInput();
            }
            if(decision=="Y" || decision=="y"){
                saveGame();
            }
        }
    }
    else if(command=="save" || command=="SAVE"){
        saveGame();
        outcome = OUTCOME_SAVE;        
    }
    else if(command=="turn" || command=="TURN"){
        //for case insensitivity during later comparison
        tile = toupper(tile);
        outcome = turn(p, factory, tile, row) ? OUTCOME_TURNSUCCESS : OUTCOME_TURNFAIL;
    }
    else if(command=="help" || command=="HELP"){
        printHelp();
        outcome = OUTCOME_INVALID;
    }
    else{
        outcome = OUTCOME_INVALID;
        std::cout << "Invalid input. Type \"help\" to see list of commands.\n";
    }

    return outcome;

}

std::string Game::userInput(){

    std::string input;
    std::cout << "> "; 
    getline(std::cin, input);
    if(std::cin.eof()){
        std::cout << "Goodbye." << std::endl;
        exit(EXIT_SUCCESS);
    }  
    std::cin.clear();

    return input;

}

int Game::matchingTilesInFactory(int factory, Tile tile){

    int count = 0;
    if(factory==0){
        for(int i=0; i < this->pile->getSize(); ++i) {
            if(this->pile->get(i) == tile){
                count++;
            }
        }
    }
    else if((factory>0) && (factory<=FACTORIES)){
        for(int i=0; i<FACTORY_SIZE; i++) {
            if(factories[factory-1][i] == tile){
                count++;
            }
        }
    }
    return count;

}

void Game::printFactories() {
    std::string colour = REMOVECOLOURS;
    std::cout << "Factories: " << std::endl;
    std::cout << "0: ";
    for(int j=0; j<this->pile->getSize(); ++j) {
        colour = currentPlayer->getColour(this->pile->get(j)); 
        std::cout << colour << pile->get(j) << " ";  
    }
    std::cout << REMOVECOLOURS;
    std::cout << std::endl;
    for(int i=0; i<FACTORIES; ++i) {
        std::cout << REMOVECOLOURS << i+1 << ": ";
        for(int j=0; j < FACTORY_SIZE; ++j) { 
            colour = currentPlayer->getColour(this->factories[i][j]);
            std::cout << colour << this->factories[i][j];
            std::cout << " ";
        }
        std::cout << REMOVECOLOURS << std::endl;
    }
}


void Game::printBoard(Player* current,Player* opponent) {

    int NAMEOFFSET = 30;
    int STORAGEOFFSET = 20;
    int MAXBROKENOFFSET = 27;
    
    std::string prompt = current->getName() + "'s board:";
    std::cout << std::left << std::setw(NAMEOFFSET) << prompt << "---  " << opponent->getName() + "'s board:" << std::endl;
    for(int i=0; i<SIZE; ++i) {
        //print the current players board
        std::cout << i+1 << ": ";
        for(int j=0; FACTORY_SIZE > i + j; ++j) {
            std::cout << " ";
        }
        current->printStorageLine(i);
        std::cout << "||";
        current->printMosaicLine(i);

        // print opponents board to the right
        std::cout << std::right << std::setw(STORAGEOFFSET) << "---  ";
        std::cout << i+1 << ": ";
        for(int j=0; FACTORY_SIZE > i + j; ++j) {
            std::cout << " ";
        }
        opponent->printStorageLine(i);
        std::cout << "||";
        opponent->printMosaicLine(i);
        std::cout << std::endl;
    }

    //Broken Printing
    std::string tileColour = REMOVECOLOURS;
    std::cout << "Broken: ";
    //track additions to adjust where the opponents broken tiles are printed to keep their location static
    int countSpaces = 0;
    for(int i=0; i < current->getBroken()->getSize(); ++i) {
        tileColour = currentPlayer->getColour(current->getBroken()->get(i));
        std::cout << tileColour << current->getBroken()->get(i);
        std::cout << REMOVECOLOURS << " ";
        countSpaces = countSpaces + 2;
    }
    std::cout << REMOVECOLOURS;

    std::cout << std::right << std::setw(MAXBROKENOFFSET - countSpaces) << "---  ";
    std::cout << "Broken: ";
    for(int i=0; i < opponent->getBroken()->getSize(); ++i) {
        tileColour = currentPlayer->getColour(opponent->getBroken()->get(i));
        std::cout << tileColour << opponent->getBroken()->get(i);
        std::cout << REMOVECOLOURS << " ";
    }
    std::cout << REMOVECOLOURS << std::endl;

}

void Game::switchPlayer() {
    if(currentPlayer == p1) {
        this->currentPlayer = p2;
    }
    else {
        this->currentPlayer = p1;
    }
}

bool Game::checkRoundEnd() {
    //initialised to true, as it's faster to prove that there -are- tiles left, than it is to prove there aren't
    bool noTilesLeft = true;
    if(this->pile->getSize() > 0) {
        noTilesLeft = false;
    } 
    else {
        for(int i=0; noTilesLeft && i<FACTORIES; ++i) {
            if(!(this->factories[i][0] == NO_TILE)) {
                noTilesLeft = false;
            }
        }
    } 
    return noTilesLeft;
}

bool Game::checkGameEnd(Player* p) {
    bool gameComplete = false;
    for(int i=0; !gameComplete && i<SIZE; ++i) {
        if(p->checkComplete(i) == true) {
            gameComplete = true;
        }
    }
    return gameComplete;
}

//Sends tiles from player's FULL storage rows to their corresponding cells of the mosaic.
void Game::moveTiles(Player* p) {
    for(int row=0; row<SIZE; ++row) {
        Tile tile = p->getTile(row);
        if(p->countStorage(row+1, tile) == row+1 && tile != NO_TILE) {
            p->moveToMosaic(row+1, tile);
            std::cout << "Row " << row+1 << ":" << std::endl;
            p->addPoints(p->calcScore(row, getMosaicColumnByTile(row, tile)));
            p->clearStorageRow(row);
        }
    }
    p->addPoints(demerits[p->getBroken()->getSize()]);
}

//The standard Azul board staggers the tile order of each row by 1. Returns the column matching the input row and tile, based on the global "firstRowOrder".
int Game::getMosaicColumnByTile(int row, Tile tile){    
    int lineIndex = 0;
    for(int i=0; i<SIZE; ++i) {
        if(tile == topRowOrder[i]) {
            lineIndex = i;
        }
    }
    int output = (lineIndex+row) % SIZE;
    return output;
}

void Game::printHelp(){
    std::cout << "In order to make a turn you must use the following format:" << std::endl;
    std::cout << "turn <factory> <tile> <row> " << std::endl;
    std::cout << "where: <factory> is replaced with the factory number you wish to pull from" << std::endl;
    std::cout << "<tile> is the tile you want from that factory" << std::endl;
    std::cout << "<row> one of your storage rows that you want to insert into (choose 6 to place straight into your broken tiles)" << std::endl;
    std::tuple<int,Tile,int> move = getValidMove();

    std::cout << "For example, a valid turn for you would be: turn ";
    std::cout << std::get<0>(move) << " " << std::get<1>(move) << " " << std::get<2>(move) << " " << std::endl;
    std::cout << std::endl;
    std::string waitForInput = "";
    std::cout << "Press enter to continue..." << std::endl;
    waitForInput = userInput();
}

std::tuple<int,Tile,int> Game::getValidMove() {
    Tile possibleTile = NO_TILE;
    Tile toInsert = NO_TILE;
    std::tuple<int,Tile,int> move;
    int factory = 0;
    int row = -1;
    bool foundMove = false;
    for(int i = 0;i<FACTORIES;++i) {
        possibleTile = currentPlayer->getStorageCell(i,0);
        if(possibleTile == NO_TILE) {
            row = i + 1;
        }
    }
    if(row == -1) {
        row = 6;
    }

    for(int i = 0;i<FACTORIES;++i) {
        possibleTile = factories[i][0];
        if(possibleTile != NO_TILE && !foundMove) {
            factory = i + 1;
            toInsert = possibleTile;
            foundMove = true;
        }
    }
    if(!foundMove) {
        for(int i = 0;i<pile->getSize();++i) {
            if(!foundMove && pile->get(i) != FIRST_PLAYER) {
                toInsert = pile->get(i);
                foundMove = true;
            }
        }
    }
    if(!foundMove) {
        row = 6;
    }
    return std::make_tuple(factory,toInsert,row);
}
void Game::saveGame() {            
        SaveAndLoad* save = new SaveAndLoad(this,this->p1, this->p2, this->pile,
                this->tileBag, this->boxLid, this->currentPlayer);
        save->saveGame();                                                                             
        saved = true;

}

void Game::loadGame(std::istream& inputStream) {      
    
    SaveAndLoad* load = new SaveAndLoad(this,this->p1, this->p2, this->pile,
                this->tileBag, this->boxLid, this->currentPlayer);

    load->loadGame(inputStream);

    this->saved = true;
    this->resumed = true;

}

void Game::setCurrentPlayer(Player* p){
    currentPlayer = p;
}

void Game::setPlayerWithFPTile(Player* p){
    playerWithFPTile = p;
}

void Game::setFactory(int row, int column, Tile insert) {
    this->factories[row][column] = insert;
}

Tile Game::getTileWithinFactory(int row, int column) {
    return this->factories[row][column];
}

std::vector<std::tuple<int,Tile,int> > Game::availableTiles(Player* p) {
    std::vector<std::tuple<int,Tile,int> > moves = std::vector<std::tuple<int,Tile,int>>();

    //scan the factories for avaiable tiles and their respective factory numbers
    for(int i=0;i<SIZE;++i) {
        for(Tile t:tileSelection) {
            int count = 0;
            for(int j = 0;j<FACTORY_SIZE;++j) {
                if(factories[i][j] == t) {
                    ++count;
                }
            }
            if(count > 0){
                std::tuple<int,Tile,int> availableTile = std::make_tuple(i+1,t,count);
                moves.push_back(availableTile);
            }
        }
    }

    //scan the pile for available tiles      
    for(Tile t:tileSelection) {
        int count = 0;
        for(int i = 0;i<pile->getSize();++i) {
            if(pile->get(i) == t) {
                ++count;
            }
        }
        if(count > 0) {
            std::tuple<int,Tile,int> availableTile = std::make_tuple(0,t,count);
            moves.push_back(availableTile);
        }
    }
    
    
    return moves;
}
void Game::setAI(std::string aiState) {
    if(aiState == "true") {
        this->isAI = true;
    }
    else {
        this->isAI = false;
    }
}
std::string Game::getAi() {
    std::string stateAsString = "false";
    if(this->isAI) {
        stateAsString = "true";
    }
    return stateAsString;
}