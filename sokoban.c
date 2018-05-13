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

    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Error creating renderer\n");
        exit(1);
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_Texture* mario[4];
    SDL_Surface* surface = NULL;
    surface = IMG_Load(
        "/Users/pgaultier/Downloads/sprites_mario_sokoban/mario_up.gif");
    mario[DIR_UP] = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load(
        "/Users/pgaultier/Downloads/sprites_mario_sokoban/mario_right.gif");
    mario[DIR_RIGHT] = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load(
        "/Users/pgaultier/Downloads/sprites_mario_sokoban/mario_down.gif");
    mario[DIR_DOWN] = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load(
        "/Users/pgaultier/Downloads/sprites_mario_sokoban/mario_left.gif");
    mario[DIR_LEFT] = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    SDL_Texture* current = mario[DIR_UP];

    const float time_frame = 1000 / 60;
    bool running = true;
    uint64_t frame_count = 0;

    SDL_Rect mario_rect = {.w = 100, .h = 100, .x = 100, .y = 100};
    while (running) {
        time_t time_start = time(NULL);
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
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, current, NULL, &mario_rect);
        SDL_RenderPresent(renderer);
        frame_count++;
        time_t time_end = time(NULL);
        time_t time_diff = time_end - time_start;
        if (time_diff < time_frame)
            SDL_Delay((uint32_t)(time_frame - time_diff));
    }

    SDL_DestroyTexture(mario[0]);
    SDL_DestroyTexture(mario[1]);
    SDL_DestroyTexture(mario[2]);
    SDL_DestroyTexture(mario[3]);
    SDL_DestroyWindow(window);
    SDL_Quit();
    IMG_Quit();
}
