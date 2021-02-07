#ifndef AZUL_GAME
#define AZUL_GAME

#include "Player.h"
#include "Rules.h"
#include "LinkedList.h"
#include "TileColour.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <iomanip>

class Game {
    
    public:
    
        Game(Player* p1, Player* p2, int seed, bool isAI);
        ~Game();
        void setupGame();
        void setupRound();
        void generateFactories();
        void generateTileBag(int seed);
        Tile getTileFromBag();
        void emptyLidIntoBag();
        Tile randomTile();

        void play();
        int turnPrompt(Player* p);
        std::string userInput();
        void round();
        void endRound();
        void endGame();
        bool turn(Player* p, int factory, Tile tile, int row);

        void printBoard(Player* current,Player* opponent);
        void printFactories();
        void switchPlayer();
        bool checkRoundEnd();
        bool checkGameEnd(Player* p);
        void storageToMosaic();
        void moveTiles(Player* p);
        int getMosaicColumnByTile(int row, Tile tile);
        int matchingTilesInFactory(int factory, Tile tile);
        void printHelp();
        void saveGame();
        void loadGame(std::istream& filename);
        void setFactory(int row, int column, Tile insert);
        void setCurrentPlayer(Player* p);
        void setPlayerWithFPTile(Player* p);
        Tile getTileWithinFactory(int row, int column);
        std::vector<std::tuple<int,Tile,int> > availableTiles(Player* p);
        void setAI(std::string aiState);
        std::string getAi();
        std::tuple<int,Tile,int> getValidMove();
        
    private:

        Player* p1;
        Player* p2;
        bool isAI;
        Player* currentPlayer;
        Player* winner;
        Tile factories[FACTORIES][FACTORY_SIZE];
        LinkedList* pile;
        Player* playerWithFPTile;
        bool saved;
        bool gameEnd;
        bool isTie;
        LinkedList* boxLid;
        LinkedList* tileBag;
        bool resumed;

        // User input outcomes
        enum outcome { 
        OUTCOME_TURNSUCCESS,
        OUTCOME_TURNFAIL,
        OUTCOME_SAVE,
        OUTCOME_EXIT,
        OUTCOME_INVALID,
        };

};

#endif
