/*
 * File: Boggle.cpp
 * ----------------
 * Name: Bryan
 * This file is the main starter file for Assignment #4, Boggle.
 */

#include <iostream>
#include "gboggle.h"
#include "grid.h"
#include "gwindow.h"
#include "lexicon.h"
#include "random.h"
#include "simpio.h"
#include "vector.h"
using namespace std;

/* Constants */

const int BOGGLE_WINDOW_WIDTH = 650;
const int BOGGLE_WINDOW_HEIGHT = 350;

const string STANDARD_CUBES[16]  = {
    "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
    "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
    "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
    "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};
 
const string BIG_BOGGLE_CUBES[25]  = {
    "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
    "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
    "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
    "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
    "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};

/* Function prototypes */

Vector<string> ShuffledBoard();
Vector<string> Shuffle(Vector<string> head, Vector<string> str);
Grid<char> GetCube(Vector<string> board);
void HumanPlay(Lexicon &lex, Grid<char> board, Set<string> &list);
void ComputerPlay(Lexicon &lex, Grid<char> board, Set<string> &list);
bool MatchWord(Grid<char> board, string str, int myX, int myY, int lastX, int lastY);
void SearchBoard(Lexicon &lex, Grid<char> board, string result, Set<string> &list, int myX, int myY, int lastX, int lastY);


/* Main program */

int main() {
    GWindow gw(BOGGLE_WINDOW_WIDTH, BOGGLE_WINDOW_HEIGHT);
    initGBoggle(gw);
	Lexicon english("EnglishWords.dat");
	Set<string> wordlist;

    drawBoard(4, 4);
	Vector<string> board = ShuffledBoard();
	Grid<char> cube = GetCube(board);
	// Print the board
	for(int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			labelCube(i, j, cube[i][j]);
	HumanPlay(english, cube, wordlist);
	ComputerPlay(english, cube, wordlist);

    return 0;
}

/* Copy the constant array into a vector and return the shuffled result */
Vector<string> ShuffledBoard() {
	Vector<string> head, result;
	for (int i = 0; i < 16; i++)
		result.add(STANDARD_CUBES[i]);
	return Shuffle(head, result);
}

/* Shuffle a vector recursively */
Vector<string> Shuffle(Vector<string> head, Vector<string> str) {
	if (str.isEmpty())
		return head;

	Vector<string> first, rest;
	int i = randomInteger(0, str.size()-1);
	first = head;
	first.add(str[i]);
	rest = str;
	rest.remove(i);
	return Shuffle(first, rest);
}

/* Pick each side of the cube */
Grid<char> GetCube(Vector<string> board) {
	Grid<char> result (4, 4);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			int k = randomInteger(0, 5);
			result[i][j] = board[i * 4 + j][k];
		}
	}
	return result;
}

/* Human Input */
void HumanPlay(Lexicon &lex, Grid<char> board, Set<string> &list) {
	while (true) {
		string word = getLine();
		word = toUpperCase(word);
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				highlightCube(i, j, 0);

		if (word.length() == 0)
			break;
		else if (word.length() < 4 || !lex.contains(word) || list.contains(word))
			continue;
  		else if (!MatchWord(board, word, -1, -1, -1, -1))
  			continue;
		else {
			recordWordForPlayer(word, HUMAN);
			list.add(word);
		}
	}
}

/* Check if a word can be found in the board */
bool MatchWord(Grid<char> board, string str, int myX, int myY, int lastX, int lastY) {
	if (str == "") {
		highlightCube(myX, myY, 1);
		return true;
	}
	if (myX >= 0) {
		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				if (board.inBounds(myX+i, myY+j) && board[myX+i][myY+j] == str[0] && (myX+i != lastX || myY+j != lastY)) {
					string rest = str.substr(1);
					if (MatchWord(board, rest, myX+i, myY+j, myX, myY)) {
						highlightCube(myX+i, myY+j, 1);
						return true;
					}
				}
			}
		}
	}
	// The first letter of the word
	else {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (board[i][j] == str[0]) {
					string rest = str.substr(1);
					if (MatchWord(board, rest, i, j, -1, -1)) {
						highlightCube(i, j, 1);
						return true;
					}
				}
			}
		}
	}
	return false;
}

/* Computer's turn to find all the words missed by human */
void ComputerPlay(Lexicon &lex, Grid<char> board, Set<string> &list) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			string result = string() + board[i][j];
			SearchBoard(lex, board, result, list, i, j, -1, -1);
		}
	}
}

void SearchBoard(Lexicon &lex, Grid<char> board, string result, Set<string> &list, int myX, int myY, int lastX, int lastY) {
	if (lex.containsPrefix(result)) {
		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				if (board.inBounds(myX+i, myY+j) && (i != 0 || j != 0) && (myX+i != lastX || myY+j != lastY)) {
					string newResult = result + board[myX+i][myY+j];
					SearchBoard(lex, board, newResult, list, myX+i, myY+j, myX, myY);
				}
			}
		}
		if (result.length() >= 4 && !list.contains(result) && lex.contains(result)) {
			recordWordForPlayer(result, COMPUTER);
			list.add(result);
		}
	}
}