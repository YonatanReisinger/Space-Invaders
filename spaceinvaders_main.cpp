#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr int PLAYER_WIDTH = 60;
constexpr int PLAYER_HEIGHT = 20;
constexpr int PLAYER_SPEED = 6;
constexpr int INVADER_WIDTH = 40;
constexpr int INVADER_HEIGHT = 30;
constexpr int INVADER_ROWS = 3;
constexpr int INVADER_COLS = 5;
constexpr int INVADER_X_GAP = 30;
constexpr int INVADER_Y_GAP = 20;
constexpr int BULLET_WIDTH = 6;
constexpr int BULLET_HEIGHT = 16;
constexpr int BULLET_SPEED = 8;
constexpr int INVADER_BULLET_SPEED = 4;
constexpr int INVADER_MOVE_INTERVAL = 30; // frames
const int INVADER_PIXELS = 8;
const int NUM_INVADER_PATTERNS = 5;

struct Entity {
    int x, y, w, h;
    bool alive = true;
    int patternIndex = 0;
};

struct Bullet {
    int x, y, w, h, vy;
    bool alive = true;
};

SDL_FRect invaderSpriteRects[5] = {
    {74, 13, 42, 40},
    {75, 62, 42, 40},
    {124, 63, 42, 40},
    {219, 110, 42, 40},
    {264, 162, 42, 40}
};
SDL_FRect playerSpriteRect = {136, 13, 55, 58};

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Space Invaders POC", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Surface* invaderSurf = IMG_Load("res/invaders.png");
    if (!invaderSurf) { std::cerr << SDL_GetError() << std::endl; return 1; }
    SDL_Texture* invaderTexture = SDL_CreateTextureFromSurface(renderer, invaderSurf);
    SDL_DestroySurface(invaderSurf);

    SDL_Surface* playerSurf = IMG_Load("res/player.png");
    if (!playerSurf) { std::cerr << SDL_GetError() << std::endl; return 1; }
    SDL_Texture* playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurf);
    SDL_DestroySurface(playerSurf);

    // Player setup
    Entity player{WINDOW_WIDTH/2 - PLAYER_WIDTH/2, WINDOW_HEIGHT - 60, PLAYER_WIDTH, PLAYER_HEIGHT, true};
    int playerHealth = 3; // Player health
    std::vector<Bullet> playerBullets;
    bool leftPressed = false, rightPressed = false, firePressed = false, canFire = true;

    // Invaders setup
    std::vector<Entity> invaders;
    int invaderStartX = 100;
    int invaderStartY = 60;
    for (int row = 0; row < INVADER_ROWS; ++row) {
        for (int col = 0; col < INVADER_COLS; ++col) {
            int x = invaderStartX + col * (INVADER_WIDTH + INVADER_X_GAP);
            int y = invaderStartY + row * (INVADER_HEIGHT + INVADER_Y_GAP);
            int pattern = (row + col) % NUM_INVADER_PATTERNS;
            invaders.push_back({x, y, INVADER_WIDTH, INVADER_HEIGHT, true, pattern});
        }
    }
    int invaderDir = 1; // 1=right, -1=left
    int invaderMoveCounter = 0;

    // Invader bullets
    std::vector<Bullet> invaderBullets;

    bool quit = false;
    SDL_Event e;
    bool gameOver = false;

    while (!quit) {
        // --- Input ---
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) quit = true;
            if (e.type == SDL_EVENT_KEY_DOWN) {
                if (e.key.key == SDLK_LEFT) leftPressed = true;
                if (e.key.key == SDLK_RIGHT) rightPressed = true;
                if (e.key.key == SDLK_SPACE) firePressed = true;
            }
            if (e.type == SDL_EVENT_KEY_UP) {
                if (e.key.key == SDLK_LEFT) leftPressed = false;
                if (e.key.key == SDLK_RIGHT) rightPressed = false;
                if (e.key.key == SDLK_SPACE) firePressed = false;
            }
        }

        // --- Player movement ---
        if (leftPressed) player.x -= PLAYER_SPEED;
        if (rightPressed) player.x += PLAYER_SPEED;
        if (player.x < 0) player.x = 0;
        if (player.x + player.w > WINDOW_WIDTH) player.x = WINDOW_WIDTH - player.w;

        // --- Player shooting ---
        if (firePressed && canFire) {
            playerBullets.push_back({player.x + player.w/2 - BULLET_WIDTH/2, player.y - BULLET_HEIGHT, BULLET_WIDTH, BULLET_HEIGHT, -BULLET_SPEED, true});
            canFire = false;
        }
        if (!firePressed) canFire = true;

        // --- Move player bullets ---
        for (Bullet& b : playerBullets) {
            if (b.alive) b.y += b.vy;
            if (b.y + b.h < 0) b.alive = false;
        }

        // --- Invader movement ---
        invaderMoveCounter++;
        if (invaderMoveCounter >= INVADER_MOVE_INTERVAL) {
            int minX = WINDOW_WIDTH, maxX = 0;
            for (Entity& inv : invaders) {
                if (!inv.alive) continue;
                inv.x += invaderDir * 20;
                if (inv.x < minX) minX = inv.x;
                if (inv.x + inv.w > maxX) maxX = inv.x + inv.w;
            }
            if (minX < 10 || maxX > WINDOW_WIDTH - 10) invaderDir *= -1;
            invaderMoveCounter = 0;
        }

        // --- Invader shooting (random) ---
        if (rand() % 60 == 0) { // random chance
            std::vector<Entity*> shooters;
            for (Entity& inv : invaders) if (inv.alive) shooters.push_back(&inv);
            if (!shooters.empty()) {
                Entity* shooter = shooters[rand() % shooters.size()];
                invaderBullets.push_back({shooter->x + shooter->w/2 - BULLET_WIDTH/2, shooter->y + shooter->h, BULLET_WIDTH, BULLET_HEIGHT, INVADER_BULLET_SPEED, true});
            }
        }

        // --- Move invader bullets ---
        for (Bullet& b : invaderBullets) {
            if (b.alive) b.y += b.vy;
            if (b.y > WINDOW_HEIGHT) b.alive = false;
        }

        // --- Collision: player bullets vs invaders ---
        for (Bullet& b : playerBullets) {
            if (!b.alive) continue;
            for (Entity& inv : invaders) {
                if (!inv.alive) continue;
                SDL_Rect br{b.x, b.y, b.w, b.h};
                SDL_Rect ir{inv.x, inv.y, inv.w, inv.h};
                if (SDL_HasRectIntersection(&br, &ir)) {
                    b.alive = false;
                    inv.alive = false;
                }
            }
        }

        // --- Collision: invader bullets vs player ---
        for (Bullet& b : invaderBullets) {
            if (!b.alive) continue;
            SDL_Rect br{b.x, b.y, b.w, b.h};
            SDL_Rect pr{player.x, player.y, player.w, player.h};
            if (SDL_HasRectIntersection(&br, &pr)) {
                b.alive = false;
                if (playerHealth > 0) {
                    playerHealth--;
                }
                if (playerHealth <= 0) {
                    player.alive = false;
                    gameOver = true;
                }
            }
        }

        // --- Check for win: all invaders dead ---
        bool allInvadersDead = true;
        for (const Entity& inv : invaders) {
            if (inv.alive) {
                allInvadersDead = false;
                break;
            }
        }
        if (allInvadersDead) {
            gameOver = true;
            player.alive = false;
        }

        // --- Remove dead bullets ---
        playerBullets.erase(std::remove_if(playerBullets.begin(), playerBullets.end(), [](const Bullet& b){return !b.alive;}), playerBullets.end());
        invaderBullets.erase(std::remove_if(invaderBullets.begin(), invaderBullets.end(), [](const Bullet& b){return !b.alive;}), invaderBullets.end());

        // --- Rendering ---
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw player
        if (player.alive) {
            SDL_FRect dest = {(float)player.x, (float)player.y, (float)player.w, (float)player.h};
            SDL_RenderTexture(renderer, playerTexture, &playerSpriteRect, &dest);
        }

        // Draw invaders
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (const Entity& inv : invaders) {
            if (!inv.alive) continue;
            SDL_FRect dest = {(float)inv.x, (float)inv.y, (float)inv.w, (float)inv.h};
            SDL_RenderTexture(renderer, invaderTexture, &invaderSpriteRects[inv.patternIndex], &dest);
        }

        // Draw player bullets
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        for (const Bullet& b : playerBullets) {
            if (!b.alive) continue;
            SDL_FRect rect{(float)b.x, (float)b.y, (float)b.w, (float)b.h};
            SDL_RenderFillRect(renderer, &rect);
        }

        // Draw invader bullets
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (const Bullet& b : invaderBullets) {
            if (!b.alive) continue;
            SDL_FRect rect{(float)b.x, (float)b.y, (float)b.w, (float)b.h};
            SDL_RenderFillRect(renderer, &rect);
        }

        // Draw player health as red rectangles at the top left
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (int i = 0; i < playerHealth; ++i) {
            SDL_FRect healthRect{10.0f + i * 25.0f, 10.0f, 20.0f, 20.0f};
            SDL_RenderFillRect(renderer, &healthRect);
        }

        // Draw Game Over message if needed
        if (gameOver) {
            // Draw a big red rectangle in the center as a placeholder for 'Game Over'
            SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
            SDL_FRect goRect{WINDOW_WIDTH/2.0f - 120.0f, WINDOW_HEIGHT/2.0f - 40.0f, 240.0f, 80.0f};
            SDL_RenderFillRect(renderer, &goRect);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS

        // Stop updating the game after game over
        if (gameOver) {
            while (true) {
                while (SDL_PollEvent(&e)) {
                    if (e.type == SDL_EVENT_QUIT) {
                        quit = true;
                        break;
                    }
                }
                SDL_Delay(16);
                if (quit) break;
            }
            break;
        }
    }

    SDL_DestroyTexture(invaderTexture);
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}