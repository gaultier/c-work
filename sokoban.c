#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

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
    SDL_Surface* img = IMG_Load(
        "/Users/pgaultier/Downloads/sprites_mario_sokoban/mario_haut.gif");

    if (!img) {
        printf("Failed load img: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_BlitSurface(img, NULL, screen_surface, NULL);

    SDL_UpdateWindowSurface(window);
    SDL_Delay(5000);

    SDL_FreeSurface(img);
    SDL_DestroyWindow(window);
    SDL_Quit();
    IMG_Quit();
}
