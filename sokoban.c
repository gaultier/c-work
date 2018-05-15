#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>

typedef enum { DIR_UP, DIR_RIGHT, DIR_DOWN, DIR_LEFT } Direction;
typedef enum { NONE, WALL, CRATE, CRATE_OK, OBJECTIVE, MARIO } Entity;

void swap(Entity* a, Entity* b);
uint8_t get_next_cell_i(Direction dir, uint8_t cell);
bool is_out(uint8_t cell);
void go(Direction dir, uint8_t* cell, Entity map[144]);

void swap(Entity* a, Entity* b) {
    Entity tmp = *a;
    *a = *b;
    *b = tmp;
}

bool is_out(uint8_t cell) {
    return cell < 0 || ((1 + cell) % 12 == 0) || cell >= 12 * 12 ||
           cell % 12 == 0;
}

uint8_t get_next_cell_i(Direction dir, uint8_t cell) {
    switch (dir) {
        case DIR_UP:
            return cell - 12;
        case DIR_RIGHT:
            return cell + 1;
        case DIR_DOWN:
            return cell + 12;
        case DIR_LEFT:
            return cell - 1;
    }
}

void go(Direction dir, uint8_t* mario_cell, Entity map[144]) {
    uint8_t next_cell_i = get_next_cell_i(dir, *mario_cell);
    // MB
    if (is_out(next_cell_i)) return;

    Entity* next_cell = &map[next_cell_i];
    // MW
    if (*next_cell == WALL) return;
    // MN, MO
    if (*next_cell == NONE || *next_cell == OBJECTIVE) {
        *mario_cell = next_cell_i;
        return;
    }

    // MC*, MCo* at this point

    uint8_t next_next_cell_i = get_next_cell_i(dir, next_cell_i);

    // MCB, MCoB
    if (is_out(next_next_cell_i)) return;

    Entity* next_next_cell = &map[next_next_cell_i];

    // MCW, MCC, MCC0, MCoW, MCoC, MCoCo
    if (*next_next_cell == WALL || *next_next_cell == CRATE ||
        *next_next_cell == CRATE_OK)
        return;

    // MCN, MCoN
    if (*next_next_cell == NONE) {
        swap(next_cell, next_next_cell);
        *mario_cell = next_cell_i;
        return;
    }

    // MCO, MCoO
    if (*next_next_cell == OBJECTIVE) {
        *next_next_cell = CRATE_OK;
        *next_cell = *next_cell == CRATE_OK ? OBJECTIVE : NONE;
        *mario_cell = next_cell_i;
        return;
    }
}

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
            map[i] = NONE;
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
                case SDLK_UP:
                    current = mario[DIR_UP];
                    go(DIR_UP, &mario_cell, map);
                    break;
                case SDLK_RIGHT:
                    current = mario[DIR_RIGHT];
                    go(DIR_RIGHT, &mario_cell, map);
                    break;

                case SDLK_DOWN:
                    current = mario[DIR_DOWN];
                    go(DIR_DOWN, &mario_cell, map);
                    break;

                case SDLK_LEFT:
                    current = mario[DIR_LEFT];
                    go(DIR_LEFT, &mario_cell, map);
                    break;
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
