#include "Game.h"
class Game;
class SaveAndLoad {
    public:
        SaveAndLoad(Game* game,Player* p1, Player* p2, LinkedList* pile,
                LinkedList* tileBag, LinkedList* boxLid, Player* currentPlayer);
        ~SaveAndLoad();
        void loadGame(std::istream& inputStream);
        void saveGame();
    private:
        Game* game;
        Player* p1;    
        Player* p2;
        LinkedList* pile;
        LinkedList* tileBag;
        LinkedList* boxLid;
        Player* currentPlayer;
        
};