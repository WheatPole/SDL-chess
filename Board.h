#pragma once
#include <vector>
#include <iostream>
#define SDL_MAIN_HANDLED
#include "SDL.h"
#include "SDL_image.h"

#include <algorithm>
#include <string>

/*enum class Piece {
	NONE = 0, BPAWN = 1, BROOK = 2, BDONKEY = 3, BBISHOP = 4, BQUEEN = 5, BKING = 6,
	WPAWN = 7, WROOK = 8, WDONKEY = 9, WBISHOP = 10, WQUEEN = 12, WKING = 11
};*/

enum class Winner {
	NONE = 0, STALEMATE = 1, BLACK = 2, WHITE = 3
};

class Board {
public:
	
	char board[8][8];
	//std::string board;
	int boardx, boardy;
	int width, height;
	int selectedi = -1, selectedj = -1;
	//0 - white
	//1 - black
	char turn;
	bool user;

	std::vector<std::pair<int, int>> whitePiecesAlive;
	std::vector<std::pair<int, int>> blackPiecesAlive;

	int drawcnt = 0;
	int counter = 1;

	//up left, up right, down left, down right
	bool castle[4] = { 1, 1, 1, 1 };
	std::string castle1 = "KQkq";

	Winner win;

	Board(int boardx, int boardy, int width, int height, bool user, std::string& fen, SDL_Renderer* r = nullptr);
	std::vector<std::pair<int, int>> getMoves(int& i, int& j);
	void drawBoard(SDL_Renderer* r, SDL_Window* w);
	
	bool putPiece(int i, int j);
	SDL_Rect getPieceTexture(char piece);

	std::pair<int, int> enpassant = { -1, -1 };

	inline bool isBlack(char& piece) {
		return (piece >= 'a' && piece <= 'z');
	}
	inline bool isWhite(char& piece) {
		return (piece >= 'A' && piece < 'Z');
	}

	inline bool isEnemy(char& attacking, char& target) {
		if (isBlack(attacking)) {
			return (isWhite(target));
		}
		else if (isWhite(attacking)) {
			return (isBlack(target));
		}
		return false;
	}
	inline bool isEnemyOrNone(char& attacking, char& target) {
		if (isBlack(attacking)) {
			return (isWhite(target) || target == ' ');
		}
		else if (isWhite(attacking)) {
			return (isBlack(target) || target == ' ');
		}
		return false;
	}

	bool checkIfChess();
	bool stalemate();
	~Board();
private:
	SDL_Surface* image;
	SDL_Texture* sheet;
};