#include "EventHandler.h"

bool quit = false;

void handle(SDL_Event& event, Board& chess) {
    switch (event.type)
    {
    case SDL_QUIT:
        quit = true;
        break;
    case SDL_MOUSEBUTTONDOWN:
        int mousex = event.button.x - chess.boardx, mousey = event.button.y - chess.boardy;
        if (mousex > chess.width || mousey > chess.height) {
            break;
        }
        int cellj = mousex / (chess.width / 8);
        if (cellj >= 8) cellj = 7;
        int celli = mousey / (chess.height / 8);
        if (celli >= 8) celli = 7;
        if (chess.selectedi == -1) {
            if (chess.board[celli][cellj] != ' ') {
                //select a new piece
                chess.selectedi = celli;
                chess.selectedj = cellj;
            }
        }
        else {
            if (!chess.isEnemyOrNone(chess.board[chess.selectedi][chess.selectedj], chess.board[celli][cellj])) {
                //select a new piece
                chess.selectedi = celli;
                chess.selectedj = cellj;
            }
            else {
                //place
                bool t = chess.putPiece(celli, cellj);
                //std::cout << t << " " << chess.isEnemy(chess.board[chess.selectedi][chess.selectedj], chess.board[celli][cellj]) << " " << int(chess.board[chess.selectedi][chess.selectedj]) << " " << int(chess.board[celli][cellj]) << std::endl;
                if (t) {
                    //end of board
                    char pawn = (chess.turn == 'p') ? 'p' : 'P';
                    char queen = (chess.turn == 'b') ? 'q' : 'Q';
                    std::cout << pawn << " " << queen << " " << celli << " " << cellj << " " << chess.board[celli][cellj] << std::endl;
                    if ((celli == 0 || celli == 7) && chess.board[celli][cellj] == pawn) {
                        chess.board[celli][cellj] = queen;
                    }
                    if (chess.turn == 'w') chess.turn = 'b';
                    else if (chess.turn == 'b') chess.turn = 'w';
                }
                else if (chess.isEnemy(chess.board[chess.selectedi][chess.selectedj], chess.board[celli][cellj])) {
                    chess.selectedi = celli;
                    chess.selectedj = cellj;
                }
            }
        }
        break;
    }
}