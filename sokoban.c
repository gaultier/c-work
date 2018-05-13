#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

enum Direction { DIR_UP, DIR_RIGHT, DIR_DOWN, DIR_LEFT };

int main() {
    IMG_Init(IMG_INIT_JPG);
    if (SDL_Init(SDL_INIT_VIDEO) < 0) exit(1);

    const uint16_t SCREEN_WIDTH = 500;
    const uint16_t SCREEN_HEIGHT = 500;

    SDL_Window* window =
        SDL_CreateWindow("Sokoban", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
                         SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_GRABBED |
                             SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window) exit(1);

    SDL_Surface* screen_surface = SDL_GetWindowSurface(window);
    SDL_Surface* mario[4];
    mario[DIR_UP] = IMG_Load(
        "/Users/pgaultier/Downloads/sprites_mario_sokoban/mario_up.gif");
    mario[DIR_RIGHT] = IMG_Load(
        "/Users/pgaultier/Downloads/sprites_mario_sokoban/mario_right.gif");
    mario[DIR_DOWN] = IMG_Load(
        "/Users/pgaultier/Downloads/sprites_mario_sokoban/mario_down.gif");
    mario[DIR_LEFT] = IMG_Load(
        "/Users/pgaultier/Downloads/sprites_mario_sokoban/mario_left.gif");
    SDL_Surface* current = mario[DIR_UP];

    // if (!mario_up) {
    //    printf("Failed load mario_up: %s\n", SDL_GetError());
    //    exit(1);
    //}

    bool running = true;
    while (running) {
        SDL_Event e;
        SDL_PollEvent(&e);
        if (e.type == SDL_QUIT)
            running = false;
        else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_UP:
                    current = mario[DIR_UP];
                    break;
                case SDLK_RIGHT:
                    current = mario[DIR_RIGHT];
                    break;
                case SDLK_DOWN:
                    current = mario[DIR_DOWN];
                    break;
                case SDLK_LEFT:
                    current = mario[DIR_LEFT];
                    break;
            }
        }
        SDL_BlitSurface(current, NULL, screen_surface, NULL);
        SDL_UpdateWindowSurface(window);
    }

    SDL_FreeSurface(mario[0]);
    SDL_FreeSurface(mario[1]);
    SDL_FreeSurface(mario[2]);
    SDL_FreeSurface(mario[3]);
    SDL_DestroyWindow(window);
    SDL_Quit();
    IMG_Quit();
}
