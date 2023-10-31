#include "Board.h"

std::vector<std::string> split(std::string& a, char c) {
	std::vector<std::string> res;
	std::string curr = "";
	for (int i = 0; i < a.length(); i++) {
		if (a[i] == c) {
			res.push_back(curr);
			curr = "";
		}
		else {
			curr += a[i];
		}
	}
	res.push_back(curr);
	return res;
}

Board::Board(int boardx, int boardy, int width, int height, bool user, std::string& fen, SDL_Renderer* r) {
	this->boardx = boardx;
	this->boardy = boardy;
	this->width = width;
	this->height = height;
	this->user = user;

	image = IMG_Load("Resources/chess.png");
	sheet = SDL_CreateTextureFromSurface(r, image);
	

	/*enum class Piece {
	NONE = 0, BPAWN = 1, BROOK = 2, BDONKEY = 3, BBISHOP = 4, BQUEEN = 5, BKING = 6,
	WPAWN = 7, WROOK = 8, WDONKEY = 9, WBISHOP = 10, WQUEEN = 12, WKING = 11
	};*/

	/*unsigned int hboard[][8] = {
		{ 2, 3, 4, 5, 6, 4, 3, 2 },
		{ 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 7, 7, 7, 7, 7, 7, 7, 7 },
		{ 8, 9, 10, 12, 11, 10, 9, 8 }
	};*/

	/*unsigned int hboard[][8] = {
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 12, 8, 10, 8, 12, 0, 0 },
		{ 0, 0, 10, 6, 0, 0, 0, 0 },
		{ 0, 0, 8, 10, 8, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 11 }
	};*/

	/*int distance = user * 6; //WPAWN-BPAWN
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (hboard[i][j] == 0) board[i][j] = Piece::NONE;
			else board[i][j] = static_cast<Piece>((hboard[i][j] >= 7) ? hboard[i][j] - distance : hboard[i][j] + distance);
		}
	}*/

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			board[i][j] = ' ';
		}
	}

	//TODO : add swap sides

	std::vector<std::string> data = split(fen, ' ');
	std::vector<std::string> boarddata = split(data[0], '/');
	int distance = user * 6; //WPAWN-BPAWN
	for (int i = 0; i < boarddata.size(); i++) {
		int iter = 0;
		for (int j = 0; j < boarddata[i].length(); j++) {
			if (boarddata[i][j] >= '0' && boarddata[i][j] <= '9') {
				iter += boarddata[i][j] - '0';
			}
			else {
				if (boarddata[i][j] < 'a' && boarddata[i][j] != ' ') {
					whitePiecesAlive.push_back({ i, j });
				}
				else if (boarddata[i][j] != ' ') {
					blackPiecesAlive.push_back({ i, j });
				}
				board[i][iter] = boarddata[i][j];
				iter++;
			}
		}
	}
	for (int i = 0; i < data.size(); i++) {
		std::cout << "|" << data[i] << "|" << std::endl;
	}
	turn = data[1][0];

	castle1 = data[2];
	castle[0] = castle1[0] == 'K';
	castle[1] = castle1[1] == 'Q'; 
	castle[2] = castle1[2] == 'k';
	castle[3] = castle1[3] == 'q';
	
	if (data[3][0] != '-') {
		//enpassant, decrypt the thing
	}

	drawcnt = std::stoi(data[4]);
	counter = std::stoi(data[5]);

	win = Winner::NONE;
}

SDL_Rect Board::getPieceTexture(char piece) {
	int w = image->w / 6;
	int h = image->h / 2;
	switch (piece) {
	case 'K':
		return { 0, 0, w, h };
		break;
	case 'Q':
		return { w, 0, w, h };
		break;
	case 'B':
		return { w*2, 0, w, h };
		break;
	case 'N':
		return { w*3, 0, w, h };
		break;
	case 'R':
		return { w*4, 0, w, h };
		break;
	case 'P':
		return { w*5, 0, w, h };
		break;
	case 'k':
		return { 0, h, w, h };
		break;
	case 'q':
		return { w, h, w, h };
		break;
	case 'b':
		return { w * 2, h, w, h };
		break;
	case 'n':
		return { w * 3, h, w, h };
		break;
	case 'r':
		return { w * 4, h, w, h };
		break;
	case 'p':
		return { w * 5, h, w, h };
		break;
	default:
		return { 0, 0, 1, 1 };
	}
}

bool Board::checkIfChess() { 
	//keep king coordinates saved ?
	std::vector<std::pair<int, int>> loop = (turn == 'w') ? whitePiecesAlive : blackPiecesAlive;
	char king = (turn == 'w') ? 'K' : 'k';
	for (int i = 0; i < loop.size(); i++) {
		if (board[loop[i].first][loop[i].second] == king) {
			//check for donkey
			{
				int di[] = { -2, -2, -1, -1, 1, 1, 2, 2 };
				int dj[] = { -1, 1, -2, 2, -2, 2, -1, 1 };

				for (int k = 0; k < 8; k++) {
					int i1 = loop[i].first + di[k];
					int j1 = loop[i].second + dj[k];

					if (i1 >= 0 && i1 < 8 && j1 >= 0 && j1 < 8 && 
						tolower(board[i1][j1]) == 'n' && isEnemy(king, board[i1][j1])) {
						return true;
					}
				}
			}
			//check for pawn
			{
				int di[] = { !(user ^ turn) ? -1 : 1, !(user ^ turn) ? -1 : 1 };
				int dj[] = { -1, 1 };

				for (int k = 0; k < 2; k++) {
					int i1 = loop[i].first + di[k];
					int j1 = loop[i].second + dj[k];

					if (i1 >= 0 && i1 < 8 && j1 >= 0 && j1 < 8 && 
						tolower(board[i1][j1]) == 'p' && isEnemy(king, board[i1][j1])) {
						return true;
					}
				}
			}
			//check for king
			{
				int di[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
				int dj[] = { -1, 0, 1, -1, 1, -1, 0, 1 };

				for (int k = 0; k < 8; k++) {
					int i1 = loop[i].first + di[k];
					int j1 = loop[i].second + dj[k];

					if (i1 >= 0 && i1 < 8 && j1 >= 0 && j1 < 8 && 
						tolower(board[i1][j1]) == 'k' && isEnemy(king, board[i1][j1])) {
						return true;
					}
				}
			}
			//check for bishop/queen
			{
				int di[] = { -1, -1, 1, 1 };
				int dj[] = { -1, 1, -1, 1 };

				for (int k = 1; k < 8; k++) {
					for (int t = 0; t < 4; t++) {
						int i1 = loop[i].first + di[k];
						int j1 = loop[i].second + dj[k];
						if (i1 == loop[i].first && j1 == loop[i].second) continue;
						if (i1 >= 0 && i1 < 8 && j1 >= 0 && j1 < 8 && isEnemyOrNone(king, board[i1][j1])) {
							if (isEnemy(king, board[i1][j1])) {
								if (tolower(board[i1][j1]) == 'q' && tolower(board[i1][j1]) == 'b') {
									return true;
								}
								di[t] = 0;
								dj[t] = 0;
							}
						}
						else {
							di[t] = 0;
							dj[t] = 0;
						}
					}
				}
			}
			//check for rook/queen
			{
				int di[] = { -1, 1, 0, 0 };
				int dj[] = { 0, 0, -1, 1 };

				for (int k = 1; k < 8; k++) {
					for (int t = 0; t < 4; t++) {
						int i1 = loop[i].first + di[k];
						int j1 = loop[i].second + dj[k];
						if (i1 == loop[i].first && j1 == loop[i].second) continue;
						if (i1 >= 0 && i1 < 8 && j1 >= 0 && j1 < 8 && isEnemyOrNone(king, board[i1][j1])) {
							if (isEnemy(king, board[i1][j1])) {
								if (tolower(board[i1][j1]) == 'q' && tolower(board[i1][j1]) == 'r') {
									return true;
								}
								di[t] = 0;
								dj[t] = 0;
							}
						}
						else {
							di[t] = 0;
							dj[t] = 0;
						}
					}
				}
			}
			return false;
		}
	}
	return false; //????
}

bool Board::stalemate() {
	bool end = true;
	std::vector<std::pair<int, int>> loop = (turn == 'w') ? whitePiecesAlive : blackPiecesAlive;
	for (int i = 0; i < loop.size(); i++) {
		//opposite turns because it hasn't been updated yet | ????
		//if ((turn && isBlack(board[i][j]) || (!turn && isWhite(board[i][j])))) {
			//std::cout << turn << " " << i << " " << j << " " << getMoves(i, j).size() << std::endl;
		if (getMoves(loop[i].first, loop[i].second).size() != 0) {
			end = false;
			break;
		}
		if (!end) break;
	}
	if (end) {
		return true;
	}
	return false;
}

std::vector<std::pair<int, int>> Board::getMoves(int& i, int& j) {
	if (i == -1 || j == -1) {
		return {};
	}

	std::vector<std::pair<int, int>> res;
	char piece = board[i][j];
	//std::cout << board[i][j] << isBlack(piece) << " " << isWhite(piece) << turn << std::endl;
	if ((isBlack(piece) && turn == 'w') || (isWhite(piece) && turn == 'b')) return {};
	//if (checkIfChess())
	switch (piece) {
	case 'p':
	case 'P': {
		//check up or down (reduced from 2 if statements)
		// -1 down 1 up
		std::cout << user << " " << turn << std::endl;
		int deg = ((!user && turn == 'w') || (user && turn == 'b')) ? -1 : 1;
		std::cout << deg << std::endl;
		if (i + deg < 8 && i + deg >= 0) {
			if (board[i + deg][j] == ' ') {
				res.push_back({ i + deg, j });
				//simple: 1 - down, -1 - up, so it should be i == 1 for up and i == 6 for down
				//1, -1 => -1, 1 => -3, 3 => 1, 6
				if (i == (((-deg) * 3 + 3)/6) * 5 + 1) {
					try {
						if (board[i + 2 * deg][j] == ' ') {
							res.push_back({ i + 2 * deg, j });
						}
					}
					catch (...) {
						//should not happen
						std::cout << "Error with initial pawn positions" << std::endl;
					}
				}
			}

			if (j - 1 >= 0 && isEnemy(board[i][j], board[i + deg][j - 1])) {
				res.push_back({ i + deg, j - 1 });
			}
			if (j + 1 < 8 && isEnemy(board[i][j], board[i + deg][j + 1])) {
				res.push_back({ i + deg, j + 1 });
			}
		}
		//older
			/*if (i - 1 >= 0) {
				if (board[i - 1][j] == Piece::NONE) {
					res.push_back({ i - 1, j });
					if (i == 6) {
						if (board[i - 2][j] == Piece::NONE) {
							res.push_back({ i - 2, j });
						}
					}
				}

				if (j - 1 >= 0 && isEnemy(board[i][j], board[i - 1][j - 1])) {
					res.push_back({ i - 1, j - 1 });
				}
				if (j + 1 < 8 && isEnemy(board[i][j], board[i - 1][j + 1])) {
					res.push_back({ i - 1, j + 1 });
				}
			}*/
		break; }
	case 'N':
	case 'n': {
		int di[] = { -2, -2, -1, -1, 1, 1, 2, 2 };
		int dj[] = { -1, 1, -2, 2, -2, 2, -1, 1 };

		for (int k = 0; k < 8; k++) {
			int i1 = i + di[k];
			int j1 = j + dj[k];
			if (i1 >= 0 && i1 < 8 && j1 >= 0 && j1 < 8 && isEnemyOrNone(board[i][j], board[i1][j1])) {
				res.push_back({ i1, j1 });
			}
		}
		break; }
	case 'b':
	case 'B': {
		int di[] = { -1, -1, 1, 1 };
		int dj[] = { -1, 1, -1, 1 };

		for (int k = 1; k < 8; k++) {
			for (int t = 0; t < 4; t++) {
				int i1 = i + di[t] * k;
				int j1 = j + dj[t] * k;
				if (i1 == i && j1 == j) continue;
				if (i1 >= 0 && i1 < 8 && j1 >= 0 && j1 < 8 && isEnemyOrNone(board[i][j], board[i1][j1])) {
					res.push_back({ i1, j1 });
					if (isEnemy(board[i][j], board[i1][j1])) {
						di[t] = 0;
						dj[t] = 0;
					}
				}
				else {
					di[t] = 0;
					dj[t] = 0;
				}
			}
		}
		break; }
	case 'r':
	case 'R': {
		int di[] = { -1, 1, 0, 0 };
		int dj[] = { 0, 0, -1, 1 };

		for (int k = 1; k < 8; k++) {
			for (int t = 0; t < 4; t++) {
				int i1 = i + di[t] * k;
				int j1 = j + dj[t] * k;
				if (i1 == i && j1 == j) continue;
				if (i1 >= 0 && i1 < 8 && j1 >= 0 && j1 < 8 && isEnemyOrNone(board[i][j], board[i1][j1])) {
					res.push_back({ i1, j1 });
					if (isEnemy(board[i][j], board[i1][j1])) {
						di[t] = 0;
						dj[t] = 0;
					}
				}
				else {
					di[t] = 0;
					dj[t] = 0;
				}
			}
		}
		break; }
	case 'q':
	case 'Q': {
		int di[] = { -1, 1, 0, 0, -1, -1, 1, 1 };
		int dj[] = { 0, 0, -1, 1, -1, 1, -1, 1 };

		for (int k = 1; k < 8; k++) {
			for (int t = 0; t < 8; t++) {
				int i1 = i + di[t] * k;
				int j1 = j + dj[t] * k;
				if (i1 == i && j1 == j) continue;
				if (i1 >= 0 && i1 < 8 && j1 >= 0 && j1 < 8 && isEnemyOrNone(board[i][j], board[i1][j1])) {
					res.push_back({ i1, j1 });
					if (isEnemy(board[i][j], board[i1][j1])) {
						di[t] = 0;
						dj[t] = 0;
					}
				}
				else {
					di[t] = 0;
					dj[t] = 0;
				}
			}
		}
		break; }
	case 'k':
	case 'K': {
		int di[] = { -1, 1, 0, 0, -1, -1, 1, 1 };
		int dj[] = { 0, 0, -1, 1, -1, 1, -1, 1 };

		for (int t = 0; t < 8; t++) {
			int i1 = i + di[t];
			int j1 = j + dj[t];
			if (i1 == i && j1 == j) continue;
			if (i1 >= 0 && i1 < 8 && j1 >= 0 && j1 < 8 && isEnemyOrNone(board[i][j], board[i1][j1])) {
				res.push_back({ i1, j1 });
			}
			else {
				di[t] = 0;
				dj[t] = 0;
			}
		}

		//castle
		if (!checkIfChess()) {

			if (i == 7 && j == 4 - user) {
				if (castle[3]) {
					bool valid = true;
					for (int k = 4 - user + 1; k < 7; k++) {
						if (board[i][k] != ' ') {
							valid = false;
							break;
						}
					}
					if (valid) {
						res.push_back({ i, j + 2 });
					}
				}
				if (castle[2]) {
					bool valid = true;
					for (int k = 4 - user - 1; k > 0; k--) {
						if (board[i][k] != ' ') {
							valid = false;
							break;
						}
					}
					if (valid) {
						res.push_back({ i, j - 2 });
					}
				}
			}

			if (i == 0 && j == 4 - user) {
				if (castle[1]) {
					bool valid = true;
					for (int k = 4 - user + 1; k < 7; k++) {
						if (board[i][k] != ' ') {
							valid = false;
							break;
						}
					}
					if (valid) {
						res.push_back({ i, j + 2 });
					}
				}
				if (castle[0]) {
					bool valid = true;
					for (int k = 4 - user - 1; k > 0; k--) {
						if (board[i][k] != ' ') {
							valid = false;
							break;
						}
					}
					if (valid) {
						res.push_back({ i, j - 2 });
					}
				}
			}
		}
		break; }
	default: {
		std::cout << "Error with move selection" << std::endl; //selected nothing (???)
		break; }
	}

	if (res.size() == 0) {
		// uh oh
	}
	else {
		//check for each
		for (int k = 0; k < res.size(); k++) {
			int i1 = res[k].first, j1 = res[k].second;

			char temp = board[i1][j1];
			board[i1][j1] = board[i][j];
			board[i][j] = ' ';
			
			bool castled = false;
			char rookColour;
			int rookj;

			//literally only checking for castle
			if (tolower(board[i1][j1]) == 'k') {
				if (abs(j1 - j) == 2) {
					if (j1 < 3) {
						board[i1][j1 + 1] = (board[i1][j1] == 'K') ? 'R' : 'r';
						rookj = j1 + 1;
						castled = true;
						rookColour = board[i1][j1 + 1];
					}
					else if (j1 > 4) {
						board[i1][j1 - 1] = (board[i1][j1] == 'K') ? 'R' : 'r';
						rookj = j1 - 1;
						castled = true;
						rookColour = board[i1][j1 - 1];
					}
					else {
						std::cout << "Error" << std::endl;
					}
				}
			}

			if (checkIfChess()) {
				res.erase(res.begin() + k);
				k--;
			}

			//again
			if (castled) {
				board[i1][rookj] = ' ';
				if (rookj == j1 - 1) {
					board[i1][7] = rookColour;
				}
				else if (rookj == j1 + 1) {
					board[i1][0] = rookColour;
				}
				else {
					std::cout << "Error" << std::endl;
				}
			}

			board[i][j] = board[i1][j1];
			board[i1][j1] = temp;
		}
	}

	return res;
}

void Board::drawBoard(SDL_Renderer* r, SDL_Window* w) {
	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);

	SDL_Rect outline;
	outline.x = boardx - 1;
	outline.y = boardy - 1;
	outline.w = width - 2;
	outline.h = height - 2;
	SDL_RenderDrawRect(r, &outline);

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if ((j + i % 2) % 2 == 0) {
				//white
				SDL_SetRenderDrawColor(r, 77, 41, 41, 255);
				//SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
			}
			else {
				//black
				SDL_SetRenderDrawColor(r, 46, 18, 0, 255);
				//SDL_SetRenderDrawColor(r, 120, 255, 120, 255);
			}
			if (win == Winner::NONE) {
				if (i == selectedi && j == selectedj) {
					SDL_SetRenderDrawColor(r, 120, 88, 71, 255);
				}
			}
			else {
				if (win == Winner::WHITE || win == Winner::STALEMATE) {
					if (board[i][j] == 'k') {
						SDL_SetRenderDrawColor(r, 155, 10, 10, 255);
					}
				}
				else if (win == Winner::BLACK || win == Winner::STALEMATE) {
					if (board[i][j] == 'K') {
						SDL_SetRenderDrawColor(r, 155, 10, 10, 255);
					}
				}
			}

			SDL_Rect a;
			a.x = boardx + j * (width / 8);
			a.y = boardy + i * (height / 8);
			a.w = width / 8;
			a.h = height / 8;

			SDL_RenderFillRect(r, &a);

			if (board[i][j] != ' ') {
				SDL_Rect piecePos = getPieceTexture(board[i][j]);
				SDL_RenderCopy(r, sheet, &piecePos, &a);
			}
		}
	}
	if (selectedi != -1) {
		std::vector<std::pair<int, int>> moves = getMoves(selectedi, selectedj);
		SDL_Surface* img = IMG_Load("Resources/circle.png");
		SDL_Texture* circle = SDL_CreateTextureFromSurface(r, img);
		
		SDL_SetRenderTarget(r, circle);
		SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
		SDL_SetTextureBlendMode(circle, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(r, 255, 255, 255, 10);

		for (int i = 0; i < moves.size(); i++) {
			int i1 = moves[i].first;
			int j1 = moves[i].second;
			
			SDL_Rect a;
			a.x = boardx + j1 * (width / 8);
			a.y = boardy + i1 * (height / 8);
			a.w = width / 8;
			a.h = height / 8;

			SDL_Rect t = { 0, 0, 500, 500 };
			SDL_RenderCopy(r, circle, &t, &a);
		}
		SDL_SetRenderTarget(r, nullptr);
		//destroy texture
		SDL_DestroyTexture(circle);
		SDL_FreeSurface(img);
	}
	SDL_RenderPresent(r);
}

bool Board::putPiece(int i, int j) {
	//can be optimised by memo, although not sure if it's worth it
	std::vector<std::pair<int, int>> available = getMoves(selectedi, selectedj);
	
	for (int k = 0; k < available.size(); k++) {
		if (available[k].first == i && available[k].second == j) {
			board[i][j] = board[selectedi][selectedj];
			if (tolower(board[selectedi][selectedj]) == 'r') {
				if ((selectedi == 7 && selectedj == 7) || (i == 7 && j == 7)) {
					castle[3] = 0;
				}
				if ((selectedi == 7 && selectedj == 0) || (i == 7 && j == 0)) {
					castle[2] = 0;
				}
				if ((selectedi == 0 && selectedj == 7) || (i == 0 && j == 7)) {
					castle[1] = 0;
				}
				if ((selectedi == 0 && selectedj == 0) || (i == 0 && j == 0)) {
					castle[0] = 0;
				}
			}

			if (tolower(board[selectedi][selectedj]) == 'k') {
				if (selectedi == 7) {
					castle[2] = 0;
					castle[3] = 0;
				}
				else if (selectedi == 0) {
					castle[0] = 0;
					castle[1] = 0;
				}
				if (abs(selectedj - j) == 2) {
					if (j < 3) {
						board[i][j + 1] = (board[i][j] == 'K') ? 'R' : 'r';;
						board[i][0] = ' ';
						//change to moved
					}
					else if (j > 4) {
						board[i][j - 1] = (board[i][j] == 'K') ? 'R' : 'r';;
						board[i][7] = ' ';
					}
					else {
						std::cout << "Error" << std::endl;
					}
				}
			}

			board[selectedi][selectedj] = ' ';
			selectedi = -1;
			selectedj = -1;

			std::cout << "Stalemate: " << stalemate() << std::endl;
			//turn = !turn;
			/*if (stalemate()) {
				if (checkIfChess()) {
					if (turn) {
						std::cout << "WHITE WINS" << std::endl;
						win = Winner::WHITE;
					}
					else {
						std::cout << "BLACK WINS" << std::endl;
						win = Winner::BLACK;
					}
				}
				else {
					std::cout << "STALEMATE" << std::endl;
					win = Winner::STALEMATE;
				}
			}*/
			return true;
		}
	}
	return false;
}

Board::~Board() {
	SDL_DestroyTexture(sheet);
	SDL_FreeSurface(image);
}