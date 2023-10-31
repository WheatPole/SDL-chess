#include "Board.h"
#include "EventHandler.h"
//#include "SDL3/SDL.h"
#define WIDTH 800
#define HEIGHT 800

int main()
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    SDL_Event event;

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    std::string initialChessBoard = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    Board chess(0, 0, WIDTH, HEIGHT, 0, initialChessBoard, renderer);;

    while (!quit) {
        SDL_WaitEvent(&event);

        handle(event, chess);

        SDL_UpdateWindowSurface(window);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        chess.drawBoard(renderer, window);
        //SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_Delay(2);
    }

    SDL_Quit();
    //SDL_Window* window = nullptr;
    return 0;
}