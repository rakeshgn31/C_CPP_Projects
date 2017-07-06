/*
 * KimbleGame.h
 *
 *  Created on: Jul 3, 2017
 *      Author: rakeshgn
 */

#ifndef KIMBLEGAME_H_
#define KIMBLEGAME_H_

#include <vector>
#include <map>

using namespace std;

/*
 * Structure that stores the PEG index and the visited position counter
 */
typedef struct _pegVisitInfo {

	int pegIndex;
	int posVisitCtr;
}PEGVisitInfo;

/*
 * Structure that stores the player information, attributes and properties
 */
typedef struct _playerInfo {

   int playerID;				// Unique identifier for each player
   int startIndex;				// Index at which each player's peg starts
   int idxToFinal;				// Index before entering the Finish queue
   vector<int> pegsInHome;	// List of pegs yet in the player's home
   map<int, PEGVisitInfo> pegPositions;	// Map of position and the player's PEG; subset of global game state
   map<int, int> pegsAtFinish; 	// Map of finish position and peg at each position

   _playerInfo();

}PlayerInfo;

typedef struct _PegInfo {

	int playerIndex;
	int pegIndex;
	int positionVisCtr;
} PEGInfo;

/*
 * Class that stores the Game information and the state
 */
class KimbleGame {

    private:
		int numPlayers;
		bool bGameFinishes;

		// Store the player information
		map<int, PlayerInfo> mapPlayersInfo;

		/*
		 * Store the PEGs at different positions on the board
		 * Key to map is the position on the board. PEGInfo structure
		 * holds the player index and his PEG index at the position.
		 */
		map<int, PEGInfo> mapGameState;

		// Players sequence of turns
		vector<int> playerSeq;

    public:
		// Constructor
		KimbleGame(int num_players);

		void addPlayers();
		int rollDice();
		void startGame();
		void playGame(int sequenceIdx);
		void executeIfSix(int sequenceIdx, int value);
		void executeIfNotSix(int sequenceIdx, int value);
		void movePEGOnTrack(PlayerInfo player, int value);

		void printPlayerState(int seqIndex);
		void printGameState();
};

#endif /* KIMBLEGAME_H_ */
