/*
 * Kimble.cpp
 *
 *  Created on: Jul 3, 2017
 *      Author: rakeshgn
 */

#include <iostream>
#include "KimbleGame.h"

using namespace std;

int main() {

	std::cout << "/===========================================================" << endl;
	std::cout << "			   Lets play Kimble		" << endl;
	std::cout << "/===========================================================" << endl;

	int num_players = 4;
	/*cout << "Enter number of players (2 or 4):" << endl;
	cin >> num_players;*/
	if(num_players != 2 && num_players != 4) {

		cout << "Number of players can be either 2/4. Exiting.";
		return 1;
	}
	// Create an object of the Game class
	KimbleGame game(num_players);
	game.addPlayers();

	cout << "\n----------------------- Game Begins! -----------------------" << endl;
	game.startGame();

	return 0;
}
