#include "Game.h"
#include "Rules.h"

class AI {
    public:
        AI(Player* opponent, Player* controlled, Game* game);
        ~AI();
        int makeTurn();
    private:
        Player* opponent; 
        Player* controlled; 
        Game* game;
};