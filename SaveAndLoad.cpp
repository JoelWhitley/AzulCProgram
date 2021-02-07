#include "SaveAndLoad.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>


SaveAndLoad::SaveAndLoad(Game* game, Player* p1, Player* p2, LinkedList* pile,
                LinkedList* tileBag, LinkedList* boxLid, Player* currentPlayer)
                : game(game),p1(p1),p2(p2),pile(pile),tileBag(tileBag),boxLid(boxLid)
                    ,currentPlayer(currentPlayer) {
                }
SaveAndLoad::~SaveAndLoad() {
    delete this;
}

void SaveAndLoad::loadGame(std::istream& inputStream) {      

    bool loadComplete = false;

    Player* players[] = {p1,p2};

    while(inputStream.good() && !loadComplete){

        std::string aiState = "false";
        getline(inputStream,aiState); 
        game->setAI(aiState);
        
        std::string name; 
        for(Player* player:players) {                       //SET PLAYER NAMES   
            getline(inputStream, name);                                               
            player->setName(name);
        }

        int points; 
        for(Player* player:players) {                       //SET PLAYER POINTS                  
            inputStream >> points;                                
            player->setPoints(points);
        }

        std::string currentPlayerName;
        getline(inputStream, currentPlayerName);
        getline(inputStream, currentPlayerName);            //NEXT TURN
        if(currentPlayerName == name) {
            game->setCurrentPlayer(p2);
        }
        else {
            game->setCurrentPlayer(p1);
        }
        
        std::string pileLine;                               //FACTORY 0 (PILE)
        getline(inputStream, pileLine);
        Tile pileTile;
        std::stringstream ss(pileLine);
        while(ss >> pileTile){
            pile->addBack(pileTile);
        } 

        Tile factoryTile;                                   //FACTORIES
        for(int i=0; i<FACTORIES; ++i) {
            for(int j=0; j<FACTORY_SIZE; ++j) {
                inputStream >> factoryTile;
                game->setFactory(i,j,factoryTile);
            }
        }

        Tile mosaicTile;                                    
        for(Player* player:players) {                       //SET MOSAIC ROWS
            for(int i=1; i<SIZE+1; ++i) {                     
                for(int j=0; j<SIZE; ++j) {
                    inputStream >> mosaicTile;
                    if(isupper(mosaicTile)) {
                        player->moveToMosaic(i,mosaicTile);
                    }
                }
            }
        }
        
        LinkedList* toInsert = new LinkedList;              //SET STORAGE ROWS
        char storageTile;
        for(Player* player:players) {
            for(int i=1; i<SIZE+1; ++i) {
                for(int j=0; j<i; ++j) {
                    inputStream >> storageTile;
                    if(storageTile != NO_TILE){
                        toInsert->addFront(storageTile);
                    }                
                }
                if(toInsert->getSize()>0){
                    player->addToStorage(i,toInsert,boxLid);
                    toInsert->clear(); 
                }
            }
        }
    
        Tile brokenTile;
        for(Player* player:players) {                       //SET BROKEN TILES
            for(int i=0; i<FLOOR_SIZE; i++) {                   
                inputStream >> brokenTile;
                if(brokenTile != NO_TILE){
                    if(brokenTile == FIRST_PLAYER){
                        game->setPlayerWithFPTile(player);  //PLAYER WITH FP TILE
                    }
                    player->getBroken()->addBack(brokenTile);
                }            
            }
        }

        inputStream.ignore();                               //BOX LID TILES
        std::string boxLidLine;
        getline(inputStream, boxLidLine);
        getline(inputStream, boxLidLine);
        Tile boxLidTile;
        std::stringstream boxLidStream(boxLidLine);
        while(boxLidStream >> boxLidTile){
            boxLid->addBack(boxLidTile);
        } 
                             
        std::string tileBagLine;                            //BAG TILES
        getline(inputStream, tileBagLine);
        Tile tileBagTile;
        std::stringstream tileBagStream(tileBagLine);
        while(tileBagStream >> tileBagTile){
            tileBag->addBack(tileBagTile);
        } 

        int randomSeed;                                     //RANDOM SEED
        inputStream >> randomSeed;

        loadComplete = true;

    }
    std::cout << "Game successfully loaded.\n\n";
    
}

void SaveAndLoad::saveGame(){
 
    std::string fileName;
    std::cout << "\nEnter a name for the save file (or leave blank to cancel):\n";
    getline(std::cin, fileName);
    if(fileName.empty()){
        std::cout << "Save aborted." << std::endl;
    }
    else {
 
        Player* players[] = {p1,p2};
 
        std::ofstream file;
        file.open(fileName);
        file << game->getAi() << std::endl;
        file << p1->getName()<< std::endl;                  //PLAYER 1 NAME
        file << p2->getName()<< std::endl;                  //PLAYER 2 NAME
        file << p1->getPoints() << std::endl;               //PLAYER 1 POINTS
        file << p2->getPoints() << std::endl;               //PLAYER 2 POINTS
        file << currentPlayer->getName() << std::endl;      //NEXT TURN
 
        for(int j=0; j<this->pile->getSize(); ++j){         //FACTORY 0 (PILE)
            file << pile->get(j) << " ";
        }        
        file << std::endl;
       
        for(int i=0; i<FACTORIES; ++i){                     //FACTORIES
            for(int j=0; j < FACTORY_SIZE; ++j){
                file << game->getTileWithinFactory(i,j) << " ";
            }
            file << std::endl;
        }
        for(Player* p : players){                           //PLAYER MOSAIC ROWS
            for(int i=0; i<SIZE; ++i) {         
                for(int j=0; j<SIZE; ++j) {
                    if(p->getMosaicCell(i,j) == NO_TILE){
                        file << (char)tolower(topRowOrder[(((j-i) % SIZE) + SIZE) % SIZE]) << " ";
                    }
                    else {
                        file << p->getMosaicCell(i,j) << " ";
                    }                    
                }
                file << std::endl;
            }            
        }
        for(Player* p : players){
            for(int j=0; j<SIZE; ++j) {                     //PLAYER STORAGE ROWS
                for(int i=j; i>=0; --i) {
                    file << p->getStorageCell(j, j-i) << " ";
                }
                file << std::endl;
            }
        }
        for(Player* p : players){
            for(int i=0; i < FLOOR_SIZE; ++i) {             //PLAYER BROKEN TILES
                if(i<p->getBroken()->getSize()){
                    file << p->getBroken()->get(i) << " ";
                }
                else {
                    file << NO_TILE << " ";
                }            
            }    
            file << std::endl;  
        }
 
        for(int i=0; i<boxLid->getSize(); i++){             //BOX LID TILES
            file << boxLid->get(i) << " ";
        }                                        
        file << std::endl;         
        for(int i=0; i<tileBag->getSize(); i++){            //BAG TILES
            file << tileBag->get(i) << " ";
        }                                                   //BAG TILES
        file << std::endl;
        file << "0";                                        //RANDOM SEED              
                                                                                     
        std::cout << "\nGame successfully saved.\n\n";
        file.close();
    }
 
}