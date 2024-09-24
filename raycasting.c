#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAP_WIDTH 8
#define MAP_HEIGHT 8
#define TILE_SIZE 64

int map[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 1},
    {1, 0, 1, 0, 1, 0, 0, 1},
    {1, 0, 1, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 1, 1, 1, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1}
};

int loadMap(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening map file.\n");
        return 1;
    }

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            fscanf(file, "%1d", &map[y][x]);
        }
        fscanf(file, "\n");
    }

    fclose(file);
    return 0;
}

int isWall(float x, float y) {
    int mapX = (int)(x / TILE_SIZE);
    int mapY = (int)(y / TILE_SIZE);
    return map[mapY][mapX] == 1;
}

void handleInput(SDL_Event* event, float* angle, float* posX, float* posY) {
    const float rotationSpeed = 0.05f;
    const float moveSpeed = 0.1f;
    float newX, newY;

    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_LEFT:
                *angle -= rotationSpeed;
                break;
            case SDLK_RIGHT:
                *angle += rotationSpeed;
                break;
            case SDLK_UP:
                newX = *posX + moveSpeed * cos(*angle);
                newY = *posY + moveSpeed * sin(*angle);
                if (!isWall(newX, newY)) {
                    *posX = newX;
                    *posY = newY;
                }
                break;
            case SDLK_DOWN:
                newX = *posX - moveSpeed * cos(*angle);
                newY = *posY - moveSpeed * sin(*angle);
                if (!isWall(newX, newY)) {
                    *posX = newX;
                    *posY = newY;
                }
                break;
        }
    }
}

void drawWall(SDL_Renderer* renderer, int x, int height, SDL_Color color) {
    SDL_Rect wall = {x, (SCREEN_HEIGHT - height) / 2, 1, height};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &wall);
}

void render(SDL_Renderer* renderer) {
    SDL_Color northSouthColor = {255, 0, 0, 255}; // Red
    SDL_Color eastWestColor = {0, 0, 255, 255}; // Blue
    
    int i;
    for (i = 0; i < SCREEN_WIDTH; i++) {
        int wallHeight = 100; // Placeholder value
        SDL_Color color = (i % 2 == 0) ? northSouthColor : eastWestColor;
        drawWall(renderer, i, wallHeight, color);
    }
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;
    int quit = 0;
    float cameraAngle = 0.0f;
    float posX = SCREEN_WIDTH / 2;
    float posY = SCREEN_HEIGHT / 2;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Raycasting Maze", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
            handleInput(&event, &cameraAngle, &posX, &posY);
        }

        // Render based on cameraAngle and posX, posY
        render(renderer);

        SDL_Delay(16); // Cap frame rate
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

