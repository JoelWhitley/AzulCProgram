#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "Player.h"
#include "Game.h"

void mainMenu();
void newGame();
void printCredits();
void loadGame();
void aiGame();
void printHelp();
std::string userPrompt();

bool running;
std::string player1Name;
std::string player2Name;
int player1Score;
int player2Score;
std::string player1ScoreString = "";
std::string player2ScoreString = "";
std::string nextTurn;
std::string userInput();

int main() {

    running = true;

    while(running){
        mainMenu();
    }
    
    return EXIT_SUCCESS;

}

void mainMenu() {

	std::cout << "*** Welcome to 🅰 🆉 🆄 🅻 ***" << std::endl;
	std::cout << "(1): New Game" << std::endl;
    std::cout << "(2): Load Game" << std::endl;
    std::cout << "(3): AI Game" << std::endl;
    std::cout << "(4): Help" << std::endl;
    std::cout << "(5): Credits" << std::endl;
    std::cout << "(6): Exit" << std::endl;
    std::string input = userInput();
    std::stringstream ss(input);
    int choice = 0;
    ss >> choice;
    	  
	if(choice == 1) {
        newGame();
    }  
    else if(choice == 2) {
		loadGame();
    }
    else if(choice == 3) {
		aiGame();
    }
    else if(choice == 4) {
        printHelp();
    }
    else if(choice == 5) {
        printCredits();
    }
    else if(choice == 6) {
        std::cout << "Goodbye." << std::endl;
        running = false;
    }
	else {
		std::cout << "Invalid choice. Type help for more information" << std::endl;	
	}

    return;

}

void newGame() {

    std::string player1name;
    std::string player2name;
    Game* game;

    std::cout << "Enter a name for Player 1:\n";
    player1name = userInput();
    std::cout << "Enter a name for Player 2:\n";
    player2name = userInput();
    
    Player* player1 = new Player(player1name);
    Player* player2 = new Player(player2name);

    game = new Game(player1, player2, 0,false);
    std::cout << std::endl << player1->getName() << ", " << player2->getName() << ", let's play AZUL!\n\n";
    game->generateTileBag(0);
    game->play();
    
}

void loadGame() {

    Game* game;
    std::string filename;
    std::cout << "Input a filename to load from (or leave blank to cancel):\n";
    filename = userInput();
    if(filename.empty()){
        std::cout << "Load aborted." << std::endl;
    }
    else{
        std::ifstream filein(filename);
        if(filein.fail()){
            std::cout << filename << " does not exist.\n";
        }
        else if(filein.good()) {
            Player* player1 = new Player("");
            Player* player2 = new Player("");
            game = new Game(player1, player2, 0,false); 
            game->loadGame(filein);
            game->play();
        }
    }

}

void aiGame() {
    std::string player1name;
    std::string player2name;
    Game* game;

    std::cout << "Enter a name for Player 1:\n";
    player1name = userInput();
    
    Player* player1 = new Player(player1name);

    //player 2 is a bot here
    Player* player2 = new Player("Bot");

    game = new Game(player1, player2, 0, true);
    std::cout << std::endl << player1->getName() << ", try and beat the bot , let's play AZUL!\n\n";
    game->generateTileBag(1234);
    game->play();
}

void printCredits() {

    std::cout << "\n🅰 🆉 🆄 🅻" << std::endl;
    std::cout << "Created by:" << std::endl
    << "s3687337 Joshua Monaghan-Landy" << std::endl
    << "s3787473 Joel Whitley" << std::endl
    << "s3658845 Wen Koay" << std::endl
    << "s3837218 Dinesh Premanath Amarakone Urulugastenne Mudiyanselage\n" << std::endl;

    return;

}
void printHelp() {
    std::cout << "--Welcome to AZUL, a game of tiles--" << std::endl;
    std::cout << "To win the game, you need to score more points than your opponent" << std::endl;
    std::cout << "Score points by completing rows but be careful, you'll lose points if you overfill a row" << std::endl;
    std::cout << std::endl;
    std::cout << "After leaving this help menu, you'll be presented with some options" << std::endl;
    std::cout << "Input the corresponding number to choose an option" << std::endl;
    std::cout << "Remember to type help during the game for more info on how to make a turn" << std::endl;
    std::cout << std::endl;
    std::cout << "Press enter to continue..." << std::endl;
    std::string input = userInput();
}
std::string userInput(){

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