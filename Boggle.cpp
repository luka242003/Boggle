/*
 * File: Boggle.cpp
 * ----------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the main starter file for Assignment #4, Boggle.
 * [TODO: extend the documentation]
 */

#include <iostream>
#include "gboggle.h"
#include "grid.h"
#include "gwindow.h"
#include "lexicon.h"
#include "random.h"
#include "simpio.h"
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

void welcome();
void giveInstructions();
void drawCubes(Vector<char>& sides, Vector<Vector<char>>& allSides);
void arrangeCubes(Vector<string>& cubes);
void getSides(Vector<string> cubes, Vector<char>& sides);
void drawAllCubes(Vector<char> sides);
void swapCubes(Vector<string>& cubes, int i, int x);
void player(Vector<Vector<char>>& allSides, Lexicon words);
void sidesIntoMatrix(Vector<char> sides, Vector<Vector<char>>& allSides);
bool dfs(Vector<Vector<char>> allSides, Vector<pair<int,int>>& neighbors, string word, string& cur, int x, int y, Set<pair<int, int>>& visited);
bool inBorder(int x, int y, int row, int col);
bool canBeMade(Vector<Vector<char>> allSides, string word, Vector<pair<int,int>>& neighbors);
void getDirections(Vector<pair<int,int>>& directions);
void toUpper(string& word);

/* Main program */

int main() {
    GWindow gw(BOGGLE_WINDOW_WIDTH, BOGGLE_WINDOW_HEIGHT);
	Vector<char> sides;
	Vector<Vector<char>> allSides;
	Lexicon words("EnglishWords.dat");
    initGBoggle(gw);
    welcome();
    giveInstructions();
	drawBoard(4, 4);
	drawCubes(sides, allSides);
	player(allSides, words);
	system("pause");
    return 0;
}

/*
 * Function: welcome
 * Usage: welcome();
 * -----------------
 * Print out a cheery welcome message.
 */

void welcome() {
    cout << "Welcome!  You're about to play an intense game ";
    cout << "of mind-numbing Boggle.  The good news is that ";
    cout << "you might improve your vocabulary a bit.  The ";
    cout << "bad news is that you're probably going to lose ";
    cout << "miserably to this little dictionary-toting hunk ";
    cout << "of silicon.  If only YOU had a gig of RAM..." << endl << endl;
}

/*
 * Function: giveInstructions
 * Usage: giveInstructions();
 * --------------------------
 * Print out the instructions for the user.
 */

void giveInstructions() {
    cout << endl;
    cout << "The boggle board is a grid onto which I ";
    cout << "I will randomly distribute cubes. These ";
    cout << "6-sided cubes have letters rather than ";
    cout << "numbers on the faces, creating a grid of ";
    cout << "letters on which you try to form words. ";
    cout << "You go first, entering all the words you can ";
    cout << "find that are formed by tracing adjoining ";
    cout << "letters. Two letters adjoin if they are next ";
    cout << "to each other horizontally, vertically, or ";
    cout << "diagonally. A letter can only be used once ";
    cout << "in each word. Words must be at least four ";
    cout << "letters long and can be counted only once. ";
    cout << "You score points based on word length: a ";
    cout << "4-letter word is worth 1 point, 5-letters ";
    cout << "earn 2 points, and so on. After your puny ";
    cout << "brain is exhausted, I, the supercomputer, ";
    cout << "will find all the remaining words and double ";
    cout << "or triple your paltry score." << endl << endl;
    cout << "Hit return when you're ready...";
    getLine();
}

void drawCubes(Vector<char>& sides, Vector<Vector<char>>& allSides) {
	Vector<string> cubes;
	for(int i = 0; i < sizeof(STANDARD_CUBES) / sizeof(STANDARD_CUBES[0]); i++) {
		cubes.add(STANDARD_CUBES[i]);
	}
	arrangeCubes(cubes);
	getSides(cubes, sides);
	drawAllCubes(sides);
	sidesIntoMatrix(sides, allSides);
}

void arrangeCubes(Vector<string>& cubes) {
	for(int i = 0; i < cubes.size(); i++) {
		int x = randomInteger(i, cubes.size() - 1);
		swapCubes(cubes, i, x);
	}
}

void getSides(Vector<string> cubes, Vector<char>& sides) {
	for(int i = 0; i < cubes.size(); i++) {
		string cur = cubes[i];
		int x = randomInteger(0, cur.length() - 1);
		sides.add(cur[x]);
	}
}

void drawAllCubes(Vector<char> sides) {
	int x = -1;
	for(int i = 0; i < sides.size(); i++) {
		int rem = i % 4;
		if(rem == 0) {
			x++;
		}
		labelCube(x, rem, sides[i]);
	}
}

void player(Vector<Vector<char>>& allSides, Lexicon words) {
	//drawPlayerLabel(HUMAN, "Me");
	string word = "";
	Set<string> playerWords;
	int value = 0;
	Vector<pair<int,int>> neighbors;
	cout << "Enter the word: ";
	cin >> word;
	if (!word.size()) {
			cout << endl;
			cout << "Boggle finished! " << endl;
            break;
        }
	toUpper(word);
	if(word.length() < 4) {
		cout << "I'm sorry, but we have our standarts." << endl;
		cout << "That word doesn't meet the minimum word length." << endl;
	}
	else if(!words.contains(word)) {
		cout << "You can't make that word!" << endl;
	}
	else if(words.contains(word)){
			if(canBeMade(allSides, word, neighbors)) {
				cout << "Word Matched" <<endl;
				for(int i = 0; i < neighbors.size(); i++) {
					int x = neighbors[i].first; 
					int y = neighbors[i].second;
					highlightCube(x, y, true);
				}
				pause(1000);
				playerWords.add(word);
				recordWordForPlayer(word, HUMAN);
				for(int i = 0; i < neighbors.size(); i++) {
					int x = neighbors[i].first; 
					int y = neighbors[i].second;
					highlightCube(x, y, false);
				}
				neighbors.clear();
			}
			else {
				cout << "You can't make that word!" << endl;
			}
	}	
	while(true) {
		cout << "Enter the word: " << endl;
		cin >> word;
		if (word.size() == 0) {
			cout << endl;
			cout << "Boggle finished! " << endl;
            break;
        }
		toUpper(word);
		if(word.length() < 4) {
			cout << "I'm sorry, but we have our standarts." << endl;
			cout << "That word doesn't meet the minimum word length." << endl;
		}
		else if(!words.contains(word)) {
			cout << "This word doesn't exist! " << endl;
		}
		else if(playerWords.contains(word)) {
			cout << "You've already guessed that!" << endl;
		}
		else if(words.contains(word)){
			if(canBeMade(allSides, word, neighbors)) {
				cout << "Word Matched" <<endl;
				for(int i = 0; i < neighbors.size(); i++) {
					int x = neighbors[i].first; 
					int y = neighbors[i].second;
					highlightCube(x, y, true);
				}
				pause(1000);
				playerWords.add(word);
				recordWordForPlayer(word, HUMAN);
				for(int i = 0; i < neighbors.size(); i++) {
					int x = neighbors[i].first; 
					int y = neighbors[i].second;
					highlightCube(x, y, false);
				}
				neighbors.clear();
			}
			else {
				cout << "You can't make that word!" << endl;
			}
		}
	}
	
}

void sidesIntoMatrix(Vector<char> sides, Vector<Vector<char>>& allSides) {
	int x = 0;
	for(int i = 0; i < 4; i++) {
		Vector<char> vec;
		allSides.add(vec);
		for(int j = 0; j < 4; j++) {
   			allSides[i].add(sides[x]);
			x++;
		}
	}

}

bool dfs(Vector<Vector<char>> allSides, Vector<pair<int,int>>& neighbors, string word, string& cur, int x, int y, Set<pair<int,int>>& visited) {
	if(cur == word) {
		return true;
	}
	else if(cur.size() > word.size()) {
		//cout << cur << endl;
		return false;
	}
	else {
		cur += allSides[x][y];
		pair<int, int> r;
		r.first = x;
		r.second = y;
		neighbors.add(r);
		visited.add(r);
		Vector<pair<int,int>> directions;
		getDirections(directions);
		for(int i = 0; i < directions.size(); i++) {
			int newX = x + directions[i].first;
			int newY = y + directions[i].second;
			if(inBorder(newX, newY, 4, 4)) {
				pair<int, int> newPos;
				newPos.first = newX;
				newPos.second = newY;
				if(!visited.contains(newPos) && dfs(allSides, neighbors, word, cur, newX, newY, visited)) {
					return true;
				}
			}
		}
		visited.remove(r);
		cur = cur.substr(0, cur.size() - 1);
		neighbors.remove(neighbors.size() - 1);
		return false;
	}
}

bool canBeMade(Vector<Vector<char>> allSides, string word, Vector<pair<int,int>>& neighbors) {
	string cur = "";
	Set<pair<int, int>> visited;
	for(int i = 0; i < allSides.size(); i++) {
		for(int j = 0; j < allSides.size(); j++) {
			if(word[0] == allSides[i][j]) {
				if(dfs(allSides, neighbors, word, cur, i , j, visited)) {
					return true;
				}
			}
		}
	}
	return false;
}

bool inBorder(int x, int y, int row, int col) {
	return min(x, y) >= 0 && x < row && y < col;
}

void swapCubes(Vector<string>& cubes, int i, int x) {
	string cur = cubes[i];
	cubes[i] = cubes[x];
	cubes[x] = cur;
}

void getDirections(Vector<pair<int,int>>& directions) {
	pair<int, int> p1;
	p1.first = -1;
	p1.second = -1;
	directions.add(p1);
	pair<int, int> p2;
	p2.first = -1;
	p2.second = 0;
	directions.add(p2);
	pair<int, int> p3;
	p3.first = -1;
	p3.second = 1;
	directions.add(p3);
	pair<int, int> p4;
	p4.first = 0;
	p4.second = -1;
	directions.add(p4);
	pair<int, int> p5;
	p5.first = 0;
	p5.second = 1;
	directions.add(p5);
	pair<int, int> p6;
	p6.first = 1;
	p6.second = -1;
	directions.add(p6);
	pair<int, int> p7;
	p7.first = 1;
	p7.second = 1;
	directions.add(p7);
	pair<int, int> p8;
	p8.first = 1;
	p8.second = 0;
	directions.add(p8);
}

void toUpper(string& word) {
	for(int i = 0; i < word.length(); i++) {
		if(word[i] >= 'a' && word[i] <= 'z')
			word[i] = (word[i] - 'a') + 'A';
	}
}
// [TODO: Fill in the rest of the code]
