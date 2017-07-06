/*
 * KimbleGame.cpp
 *
 *  Created on: Jul 3, 2017
 *      Author: rakeshgn
 */

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "KimbleGame.h"

using namespace std;


/*
 * Structure constructor
 */
PlayerInfo::_playerInfo() {

	// Initially, all the four pegs are at HOME
	int i = 1;
	while(i < 5) {

		pegsInHome.push_back(i);
		i++;
	}
}

/*
 * Constructor of the class
 */
KimbleGame::KimbleGame(int num_players) {

	numPlayers = num_players;
	srand(time(0)); 	// Set seed to current time
	bGameFinishes = false;
}

/*
 * This function adds new players
 */
void KimbleGame::addPlayers() {

	for(int index = 1; index <= numPlayers; index++) {

		PlayerInfo playerInfo;
		playerInfo.playerID = index;
		playerInfo.startIndex = ( (index - 1) * 7 + 1 );
		if(index == 1)
			playerInfo.idxToFinal = 28;
		else
			playerInfo.idxToFinal = (playerInfo.startIndex - 1);

		// Insert the player object into the map
		mapPlayersInfo.insert( pair<int, PlayerInfo>(index, playerInfo) );
		cout << "Player " << index << " - Start position: " << playerInfo.startIndex
				<< ", Position to enter Finish: " << playerInfo.idxToFinal << endl;
	}
}

/*
 * This function generates a random number between 1 & 6
 */
int KimbleGame::rollDice() {

	return (1 + (rand() % 6));
}

/*
 * This function identifies the player to start the game
 * and others follow in a circle starting from that number
 *   E.g.: If player 2 starts, others continue in order 3, 4 and 1.
 */
void KimbleGame::startGame() {

	cout << "Rolling dice to start the game" << endl;
	int startPlayerIdx = 0, temp = 0;
	for(int p = 1; p <= numPlayers; p++) {

		int value = rollDice();
		if(value > temp) {

			temp = value;
			startPlayerIdx = p;
		}
		cout << "Player " << p << ", Die-value " << value << endl;
	}
	cout << "Player " << startPlayerIdx << " starts the game."
						<< "Players turns sequence: P" << startPlayerIdx;
	playerSeq.push_back(startPlayerIdx);

	for(int p = 1; p < numPlayers; p++) {

		startPlayerIdx++;
		temp = (startPlayerIdx % numPlayers);
		if(temp == 0) {

			playerSeq.push_back(numPlayers);
			cout << "  P" << numPlayers;
		} else {

			playerSeq.push_back(temp);
			cout << "  P" << temp;
		}
	}
	cout << endl;

	/*
	 * Start playing the game
	 */
	int ctr = 0;
	while(true) {
		ctr++;
		cout << "\n===================== TURN " << ctr << " STARTS ===========================" << endl;
		for(int index = 0; index < numPlayers; index++) { // Turn for every player
			if(!bGameFinishes)
				playGame(index);
			else
				break;
		}
		// printGameState();
		if(bGameFinishes)
			break;
	}
}

void KimbleGame::playGame(int sequenceIdx) {

	int value = rollDice();
	if(value == 6) {

		executeIfSix(sequenceIdx, value);
		printPlayerState(sequenceIdx);
		if(mapPlayersInfo[playerSeq.at(sequenceIdx)].pegsInHome.size() == 0 &&
				mapPlayersInfo[playerSeq.at(sequenceIdx)].pegsAtFinish.size() == 4) {

			cout << endl << "HURRAYYY! PLAYER " <<
				mapPlayersInfo[playerSeq.at(sequenceIdx)].playerID << " WINS THE GAME" << endl;
			bGameFinishes = true;
			return;
		}
		playGame(sequenceIdx); // Second chance for rolling dice
	} else {

		executeIfNotSix(sequenceIdx, value);
		printPlayerState(sequenceIdx);
		if(mapPlayersInfo[playerSeq.at(sequenceIdx)].pegsInHome.size() == 0 &&
				mapPlayersInfo[playerSeq.at(sequenceIdx)].pegsAtFinish.size() == 4) {

			cout << endl << "HURRAYYY! PLAYER " <<
				mapPlayersInfo[playerSeq.at(sequenceIdx)].playerID << " WINS THE GAME" << endl;
			bGameFinishes = true;
			return;
		}
	}
}

void KimbleGame::executeIfSix(int seqIndex, int value) {

	PlayerInfo player = mapPlayersInfo[playerSeq.at(seqIndex)];
	cout << "Player " << player.playerID << " rolled " << value << ". ";

	if( player.pegsInHome.size() == 4 ||
						(player.pegPositions.size() == 0 && player.pegsInHome.size() > 0) ) {

		/* if no PEGs are currently on track */
		PEGInfo peg;
		peg.playerIndex = player.playerID;
		peg.pegIndex = player.pegsInHome.back();
		peg.positionVisCtr = 1; // First 6 brings the PEG into the first position on track
		player.pegsInHome.pop_back();
		PEGVisitInfo pegVisit;
		pegVisit.pegIndex = peg.pegIndex;
		pegVisit.posVisitCtr = peg.positionVisCtr;
		player.pegPositions.insert(pair<int, PEGVisitInfo>(player.startIndex, pegVisit)); // Player local copy
		mapPlayersInfo[player.playerID] = player; // Replace modified player object

		// Insert into the global game info map
		if(mapGameState.find(player.startIndex) == mapGameState.end()) {

			mapGameState.insert(pair<int, PEGInfo>(player.startIndex, peg));
			cout << "Action: Moved peg " << peg.pegIndex << ". HOME --> START";
		} else {

			// Opponent PEG sits in the start index, send it back to his/her HOME
			// and remove corresponding PEG position in his/her local copy
			PEGInfo oppPEG = mapGameState[player.startIndex];
			mapPlayersInfo[oppPEG.playerIndex].pegsInHome.push_back(oppPEG.pegIndex);
			mapPlayersInfo[oppPEG.playerIndex].pegPositions.erase(
				mapPlayersInfo[oppPEG.playerIndex].pegPositions.find(player.startIndex));
			mapGameState[player.startIndex] = peg;
			cout << "Action: Replaced opponent peg at position " << player.startIndex;
		}
	} else {

		/* Player's PEGS are on track, he/she wishes to introduce new PEG if there is a
		 * possibility to send Opponents PEG to his/her home
		 */
		if(player.pegsInHome.size() > 0 &&
			mapGameState.find(player.startIndex) != mapGameState.end() &&
			mapGameState[player.startIndex].playerIndex != player.playerID ) {

			// Send opponent PEG back HOME & remove corresponding PEG position in local copy
			PEGInfo oppPEG = mapGameState[player.startIndex];
			mapPlayersInfo[oppPEG.playerIndex].pegsInHome.push_back(oppPEG.pegIndex);
			mapPlayersInfo[oppPEG.playerIndex].pegPositions.erase(
				mapPlayersInfo[oppPEG.playerIndex].pegPositions.find(player.startIndex));

			PEGInfo peg;
			peg.playerIndex = player.playerID;
			peg.pegIndex = player.pegsInHome.back();
			peg.positionVisCtr = 1;
			player.pegsInHome.pop_back();
			PEGVisitInfo pegVisit;
			pegVisit.pegIndex = peg.pegIndex;
			pegVisit.posVisitCtr = peg.positionVisCtr;
			player.pegPositions.insert(pair<int, PEGVisitInfo>(player.startIndex, pegVisit));
			mapGameState[player.startIndex] = peg;
			mapPlayersInfo[player.playerID] = player; // Replace modified player object

			cout << "Action: Replaced opponent peg at position " << player.startIndex;
		} else if(player.pegPositions.size() > 0) {

			movePEGOnTrack(player, value);
		}
	} // End of else

	cout << endl;
}

void KimbleGame::executeIfNotSix(int seqIndex, int value) {

	/* If value rolled is not 6 */
	PlayerInfo player = mapPlayersInfo[playerSeq.at(seqIndex)];
	cout << "Player " << player.playerID << " rolled " << value << ". ";
	if(player.pegPositions.size() > 0) {

		movePEGOnTrack(player, value);
	}

	cout << endl;
}

void KimbleGame::movePEGOnTrack(PlayerInfo player, int value) {

	// Finds possible PEG to move
	int pegCurrPos = 0, newVisCtr = 0, newPosition = 0;
	for(map<int, PEGVisitInfo>::iterator iter = player.pegPositions.begin();
									iter != player.pegPositions.end(); iter++) {

		newVisCtr = (iter->second.posVisitCtr + value);
		if(newVisCtr > 28) {

			newPosition = newVisCtr - 28;
		} else {

			newPosition = (iter->first + value) % 28;
			if(newPosition == 0) { newPosition = 28; }
		}
		if(newVisCtr <= 32) {	// 28 positions on track + 4 finish positions
			if(newVisCtr > 28 && newPosition <= 4) {

				// Check that there is no PEG at this finish position.
				if( player.pegsAtFinish.find(newPosition) == player.pegsAtFinish.end() )
					pegCurrPos = iter->first;
			} else {
				if( mapGameState.find(newPosition) == mapGameState.end())
					pegCurrPos = iter->first;
				else {
					if(mapGameState[newPosition].playerIndex != player.playerID )
						pegCurrPos = iter->first;
				}
			}

			// Got one possible move, so exiting
			if(pegCurrPos != 0)
				break;
		}
	}

	if(pegCurrPos != 0) {
		if(newVisCtr > 28 && newPosition <= 4) {

			// Insert in the finish line
			player.pegsAtFinish.insert(pair<int, int>(newPosition, mapGameState[pegCurrPos].pegIndex));
			cout << "Action: Moved peg " << mapGameState[pegCurrPos].pegIndex << " to finish";
			mapGameState.erase(mapGameState.find(pegCurrPos));
			player.pegPositions.erase(player.pegPositions.find(pegCurrPos));
		} else {

			PEGInfo peg = mapGameState[pegCurrPos];
			peg.positionVisCtr += value;
			PEGVisitInfo pegVisit;
			pegVisit.pegIndex = peg.pegIndex;
			pegVisit.posVisitCtr = peg.positionVisCtr;
			mapGameState.erase(mapGameState.find(pegCurrPos));
			player.pegPositions.erase(player.pegPositions.find(pegCurrPos));

			if( mapGameState.find(newPosition) == mapGameState.end()) {

				mapGameState.insert( pair<int, PEGInfo>(newPosition, peg));
				cout << "Action: Moved peg from " << pegCurrPos << " --> " << newPosition;
			} else {

				// Opponent PEG sits in the newPos, send it back to his/her HOME
				// and remove corresponding PEG position in his/her local copy
				PEGInfo oppPEG = mapGameState[newPosition];
				mapPlayersInfo[oppPEG.playerIndex].pegsInHome.push_back(oppPEG.pegIndex);
				mapPlayersInfo[oppPEG.playerIndex].pegPositions.erase(
							mapPlayersInfo[oppPEG.playerIndex].pegPositions.find(newPosition));

				mapGameState[newPosition] = peg;
				cout << "Action: Replaced opponent peg at position " << newPosition;
			}

			player.pegPositions.insert(pair<int, PEGVisitInfo>(newPosition, pegVisit));
		}
		mapPlayersInfo[player.playerID] = player;
	}
}

void KimbleGame::printPlayerState(int seqIndex) {

	PlayerInfo player = mapPlayersInfo[playerSeq.at(seqIndex)];

	cout << "---------------- PLAYER " << player.playerID << " STATE ------------------" << endl;

	cout << "PEGS at HOME: {";
	for(vector<int>::iterator it = player.pegsInHome.begin(); it != player.pegsInHome.end(); it++) {
		cout << " " << *it;
	}
	cout << " }" << endl;

	cout << "PEGS on TRACK (position,PEGIndex,PosVisitCtr): { ";
	for(map<int, PEGVisitInfo>::iterator it = player.pegPositions.begin(); it != player.pegPositions.end(); it++) {
		cout << "(" << it->first << "," << it->second.pegIndex << "," << it->second.posVisitCtr << ") ";
	}
	cout << " }" << endl;

	cout << "PEGS at Finish (position,PEGIndex): {";
	for(map<int, int>::iterator it = player.pegsAtFinish.begin(); it != player.pegsAtFinish.end(); it++) {
		cout << "(" << it->first << "," << it->second << ") ";
	}
	cout << " }" << endl << endl;
}

/*
void KimbleGame::printGameState() {

	cout << "********************** GAME STATE ***********************" << endl;
	cout << "POSITION  PLAYER  PEG VisitCnt" << endl;
	for(map<int, PEGInfo>::iterator it = mapGameState.begin(); it != mapGameState.end(); it++) {

		cout << "   " << it->first << "   " << it->second.playerIndex
				<< "   " << it->second.pegIndex << "   " << it->second.positionVisCtr << endl;
	}
	cout << endl;
} */
