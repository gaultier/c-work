#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>

enum Direction { DIR_UP, DIR_RIGHT, DIR_DOWN, DIR_LEFT };
enum Entity { NONE, WALL, CRATE, CRATE_OK, OBJECTIVE, MARIO };

int main() {
    IMG_Init(IMG_INIT_JPG);
    if (SDL_Init(SDL_INIT_VIDEO) < 0) exit(1);

    const uint32_t CELL_SIZE = 34;
    const uint16_t SCREEN_WIDTH = 12 * CELL_SIZE;
    const uint16_t SCREEN_HEIGHT = 12 * CELL_SIZE;

    SDL_Window* window = SDL_CreateWindow("Sokoban", SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                          SCREEN_HEIGHT, 0);
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

    SDL_Texture* textures[5] = {NULL};

    SDL_Surface* crate_surface =
        IMG_Load("/Users/pgaultier/Downloads/sprites_mario_sokoban/crate.jpg");
    textures[CRATE] = SDL_CreateTextureFromSurface(renderer, crate_surface);
    SDL_FreeSurface(crate_surface);

    SDL_Surface* crate_ok_surface = IMG_Load(
        "/Users/pgaultier/Downloads/sprites_mario_sokoban/crate_ok.jpg");
    textures[CRATE_OK] =
        SDL_CreateTextureFromSurface(renderer, crate_ok_surface);
    SDL_FreeSurface(crate_ok_surface);

    SDL_Surface* wall_surface =
        IMG_Load("/Users/pgaultier/Downloads/sprites_mario_sokoban/wall.jpg");
    textures[WALL] = SDL_CreateTextureFromSurface(renderer, wall_surface);
    SDL_FreeSurface(wall_surface);

    SDL_Surface* objective_surface = IMG_Load(
        "/Users/pgaultier/Downloads/sprites_mario_sokoban/objective.png");
    textures[OBJECTIVE] =
        SDL_CreateTextureFromSurface(renderer, objective_surface);
    SDL_FreeSurface(objective_surface);

    FILE* map_file = fopen("map.txt", "r");
    unsigned char map[12 * 12] = {0};
    fread(map, 1, 12 * 12, map_file);
    for (uint8_t i = 0; i < 12 * 12; i++) map[i] -= '0';

    SDL_Rect mario_rect = {.w = CELL_SIZE, .h = CELL_SIZE, .x = 0, .y = 0};
    const uint16_t velocity = CELL_SIZE;
    bool running = true;
    while (running) {
        SDL_Event e;
        SDL_WaitEvent(&e);
        if (e.type == SDL_QUIT)
            running = false;
        else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_UP:
                    current = mario[DIR_UP];
                    if (mario_rect.y - velocity >= 0) mario_rect.y -= velocity;
                    break;
                case SDLK_RIGHT:
                    current = mario[DIR_RIGHT];
                    if (mario_rect.x + velocity + mario_rect.w <= SCREEN_WIDTH)
                        mario_rect.x += velocity;
                    break;
                case SDLK_DOWN:
                    current = mario[DIR_DOWN];
                    if (mario_rect.y + velocity + mario_rect.h <= SCREEN_HEIGHT)
                        mario_rect.y += velocity;
                    break;
                case SDLK_LEFT:
                    current = mario[DIR_LEFT];
                    if (mario_rect.x - velocity >= 0) mario_rect.x -= velocity;
                    break;
            }
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, current, NULL, &mario_rect);
        for (uint8_t i = 0; i < 12 * 12; i++) {
            if (map[i]) {
                SDL_Rect rect = {.w = CELL_SIZE,
                                 .h = CELL_SIZE,
                                 .x = CELL_SIZE * (i % 12),
                                 .y = CELL_SIZE * (i / 12)};
                printf("type=%u x=%d y=%d\n", map[i], rect.x, rect.y);
                SDL_RenderCopy(renderer, textures[map[i]], NULL, &rect);
            }
        }
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(mario[0]);
    SDL_DestroyTexture(mario[1]);
    SDL_DestroyTexture(mario[2]);
    SDL_DestroyTexture(mario[3]);
    SDL_DestroyTexture(textures[WALL]);
    SDL_DestroyTexture(textures[CRATE]);
    SDL_DestroyTexture(textures[CRATE_OK]);
    SDL_DestroyTexture(textures[OBJECTIVE]);
    SDL_DestroyWindow(window);
    SDL_Quit();
    IMG_Quit();
}
