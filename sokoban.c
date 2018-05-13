#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>

typedef enum { DIR_UP, DIR_RIGHT, DIR_DOWN, DIR_LEFT } Direction;
typedef enum { NONE, WALL, CRATE, CRATE_OK, OBJECTIVE, MARIO } Entity;

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
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
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

    SDL_Texture* textures[6] = {NULL};

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
    if (!map_file) exit(1);

    unsigned char map_str[12 * 13] = {0};
    Entity map[12 * 12] = {NONE};
    uint64_t read_res = fread(map_str, 1, 12 * 13, map_file);
    if (read_res == 0) exit(1);
    fclose(map_file);

    uint8_t mario_cell = 0;
    for (uint8_t i = 0, j = 0; j < 12 * 13; j++) {
        if (map_str[j] == '\n') {
            continue;
        }

        map[i] = (Entity)(map_str[j] - '0');
        if (map[i] == MARIO) {
            mario_cell = i;
        }
        i++;
    };

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
                case SDLK_UP: {
                    current = mario[DIR_UP];

                    if (mario_cell < 12) break;  // Border
                    Entity* next_cell = &map[mario_cell - 12];
                    if (*next_cell == WALL || *next_cell == CRATE_OK) break;
                    if (*next_cell == NONE || *next_cell == OBJECTIVE) {
                        mario_cell -= 12;
                        break;
                    }

                    // Next cell is a crate at this point
                    bool has_next_next_cell = mario_cell >= 12 * 2;
                    if (!has_next_next_cell) break;  // Crate against border

                    Entity* next_next_cell = &map[mario_cell - 12 * 2];
                    if (*next_next_cell == WALL || *next_next_cell == CRATE ||
                        *next_next_cell == CRATE_OK)
                        break;

                    Entity* current_cell = &map[mario_cell];
                    if (*next_next_cell == NONE) {
                        *next_next_cell = CRATE;
                        *next_cell = MARIO;
                        *current_cell = NONE;
                        mario_cell -= 12;
                        break;
                    }

                    if (*next_next_cell == OBJECTIVE) {
                        *next_next_cell = CRATE_OK;
                        *next_cell = MARIO;
                        *current_cell = NONE;
                        mario_cell -= 12;
                        break;
                    }

                    break;
                }
                case SDLK_RIGHT: {
                    current = mario[DIR_RIGHT];
                    // WIP
                    if ((1 + mario_cell) % 12 == 0) break;  // Border
                    Entity* next_cell = &map[mario_cell - 12];
                    if (*next_cell == WALL || *next_cell == CRATE_OK) break;
                    if (*next_cell == NONE || *next_cell == OBJECTIVE) {
                        mario_cell -= 12;
                        break;
                    }

                    // Next cell is a crate at this point
                    bool has_next_next_cell = mario_cell >= 12 * 2;
                    if (!has_next_next_cell) break;  // Crate against border

                    Entity* next_next_cell = &map[mario_cell - 12 * 2];
                    if (*next_next_cell == WALL || *next_next_cell == CRATE ||
                        *next_next_cell == CRATE_OK)
                        break;

                    Entity* current_cell = &map[mario_cell];
                    if (*next_next_cell == NONE) {
                        *next_next_cell = CRATE;
                        *next_cell = MARIO;
                        *current_cell = NONE;
                        mario_cell -= 12;
                        break;
                    }

                    if (*next_next_cell == OBJECTIVE) {
                        *next_next_cell = CRATE_OK;
                        *next_cell = MARIO;
                        *current_cell = NONE;
                        mario_cell -= 12;
                        break;
                    }

                    break;
                }
                case SDLK_DOWN: {
                    current = mario[DIR_DOWN];
                    bool is_out = mario_cell > 11 * 12 - 1;
                    bool is_next_cell_wall = map[mario_cell + 12] == WALL;
                    if (!is_out && !is_next_cell_wall) mario_cell += 12;
                    break;
                }
                case SDLK_LEFT: {
                    current = mario[DIR_LEFT];
                    bool is_out = mario_cell % 12 == 0;
                    bool is_next_cell_wall = map[mario_cell - 1] == WALL;
                    if (!is_out && !is_next_cell_wall) mario_cell -= 1;
                    break;
                }
            }
        }
        SDL_RenderClear(renderer);
        for (uint8_t i = 0; i < 12 * 12; i++) {
            if (map[i] != NONE && map[i] != MARIO) {
                SDL_Rect rect = {.w = CELL_SIZE,
                                 .h = CELL_SIZE,
                                 .x = CELL_SIZE * (i % 12),
                                 .y = CELL_SIZE * (i / 12)};
                SDL_RenderCopy(renderer, textures[map[i]], NULL, &rect);
            }
        }
        SDL_Rect mario_rect = {.w = CELL_SIZE,
                               .h = CELL_SIZE,
                               .x = CELL_SIZE * (mario_cell % 12),
                               .y = CELL_SIZE * (mario_cell / 12)};
        SDL_RenderCopy(renderer, current, NULL, &mario_rect);

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
