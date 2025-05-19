#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>

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

struct Entity {
    int x, y, w, h;
    bool alive = true;
};

struct Bullet {
    int x, y, w, h, vy;
    bool alive = true;
};

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
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr, 0);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Player setup
    Entity player{WINDOW_WIDTH/2 - PLAYER_WIDTH/2, WINDOW_HEIGHT - 60, PLAYER_WIDTH, PLAYER_HEIGHT, true};
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
            invaders.push_back({x, y, INVADER_WIDTH, INVADER_HEIGHT, true});
        }
    }
    int invaderDir = 1; // 1=right, -1=left
    int invaderMoveCounter = 0;

    // Invader bullets
    std::vector<Bullet> invaderBullets;

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        // --- Input ---
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) quit = true;
            if (e.type == SDL_EVENT_KEY_DOWN) {
                if (e.key.keysym.sym == SDLK_LEFT) leftPressed = true;
                if (e.key.keysym.sym == SDLK_RIGHT) rightPressed = true;
                if (e.key.keysym.sym == SDLK_SPACE) firePressed = true;
            }
            if (e.type == SDL_EVENT_KEY_UP) {
                if (e.key.keysym.sym == SDLK_LEFT) leftPressed = false;
                if (e.key.keysym.sym == SDLK_RIGHT) rightPressed = false;
                if (e.key.keysym.sym == SDLK_SPACE) firePressed = false;
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
        for (auto& b : playerBullets) {
            if (b.alive) b.y += b.vy;
            if (b.y + b.h < 0) b.alive = false;
        }

        // --- Invader movement ---
        invaderMoveCounter++;
        if (invaderMoveCounter >= INVADER_MOVE_INTERVAL) {
            int minX = WINDOW_WIDTH, maxX = 0;
            for (auto& inv : invaders) {
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
            for (auto& inv : invaders) if (inv.alive) shooters.push_back(&inv);
            if (!shooters.empty()) {
                Entity* shooter = shooters[rand() % shooters.size()];
                invaderBullets.push_back({shooter->x + shooter->w/2 - BULLET_WIDTH/2, shooter->y + shooter->h, BULLET_WIDTH, BULLET_HEIGHT, INVADER_BULLET_SPEED, true});
            }
        }

        // --- Move invader bullets ---
        for (auto& b : invaderBullets) {
            if (b.alive) b.y += b.vy;
            if (b.y > WINDOW_HEIGHT) b.alive = false;
        }

        // --- Collision: player bullets vs invaders ---
        for (auto& b : playerBullets) {
            if (!b.alive) continue;
            for (auto& inv : invaders) {
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
        for (auto& b : invaderBullets) {
            if (!b.alive) continue;
            SDL_Rect br{b.x, b.y, b.w, b.h};
            SDL_Rect pr{player.x, player.y, player.w, player.h};
            if (SDL_HasRectIntersection(&br, &pr)) {
                b.alive = false;
                player.alive = false;
            }
        }

        // --- Remove dead bullets ---
        playerBullets.erase(std::remove_if(playerBullets.begin(), playerBullets.end(), [](const Bullet& b){return !b.alive;}), playerBullets.end());
        invaderBullets.erase(std::remove_if(invaderBullets.begin(), invaderBullets.end(), [](const Bullet& b){return !b.alive;}), invaderBullets.end());

        // --- Rendering ---
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw player
        if (player.alive) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_FRect rect{(float)player.x, (float)player.y, (float)player.w, (float)player.h};
            SDL_RenderFillRect(renderer, &rect);
        }

        // Draw invaders
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (const auto& inv : invaders) {
            if (!inv.alive) continue;
            SDL_FRect rect{(float)inv.x, (float)inv.y, (float)inv.w, (float)inv.h};
            SDL_RenderFillRect(renderer, &rect);
        }

        // Draw player bullets
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        for (const auto& b : playerBullets) {
            if (!b.alive) continue;
            SDL_FRect rect{(float)b.x, (float)b.y, (float)b.w, (float)b.h};
            SDL_RenderFillRect(renderer, &rect);
        }

        // Draw invader bullets
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (const auto& b : invaderBullets) {
            if (!b.alive) continue;
            SDL_FRect rect{(float)b.x, (float)b.y, (float)b.w, (float)b.h};
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
} 