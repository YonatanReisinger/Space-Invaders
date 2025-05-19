#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>

#include "bagel.h"
#include "SpaceInvaders.h"
#include "SpaceInvadersConfig.h"

SDL_Texture* gInvaderTexture = nullptr;
SDL_Texture* gPlayerTexture = nullptr;

// Placeholder for sprite rectangles (you might want to move these to a config or asset manager later)
SDL_FRect invaderSpriteRects[5] = {
    {74, 13, 42, 40},
    {75, 62, 42, 40},
    {124, 63, 42, 40},
    {219, 110, 42, 40},
    {264, 162, 42, 40}
};
SDL_FRect playerSpriteRect = {136, 13, 55, 58};

int main() {
    using namespace SpaceInvadersGame;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Space Invaders ECS", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
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
    gInvaderTexture = SDL_CreateTextureFromSurface(renderer, invaderSurf);
    SDL_DestroySurface(invaderSurf);

    SDL_Surface* playerSurf = IMG_Load("res/player.png");
    if (!playerSurf) { std::cerr << SDL_GetError() << std::endl; return 1; }
    gPlayerTexture = SDL_CreateTextureFromSurface(renderer, playerSurf);
    SDL_DestroySurface(playerSurf);

    // === Entity Creation ===
    int player_id = SpaceInvadersGame::CreatePlayerEntity(WINDOW_WIDTH / 2.0f - PLAYER_WIDTH / 2.0f, WINDOW_HEIGHT - 60.0f);
    bagel::Entity player_entity(bagel::ent_type{player_id});
    player_entity.get<SpaceInvadersGame::Collider>().width = PLAYER_WIDTH;
    player_entity.get<SpaceInvadersGame::Collider>().height = PLAYER_HEIGHT;
    player_entity.get<SpaceInvadersGame::RenderData>().spriteId = 0;

    int invaderStartX = 100;
    int invaderStartY = 60;
    for (int row = 0; row < INVADER_ROWS; ++row) {
        for (int col = 0; col < INVADER_COLS; ++col) {
            float x = invaderStartX + col * (INVADER_WIDTH + INVADER_X_GAP);
            float y = invaderStartY + row * (INVADER_HEIGHT + INVADER_Y_GAP);
            int invader_id = SpaceInvadersGame::CreateEnemyEntity(x, y, 10);
            bagel::Entity invader_entity(bagel::ent_type{invader_id});
            invader_entity.get<SpaceInvadersGame::Collider>().width = INVADER_WIDTH;
            invader_entity.get<SpaceInvadersGame::Collider>().height = INVADER_HEIGHT;
            invader_entity.get<SpaceInvadersGame::RenderData>().spriteId = (row + col) % 5;
        }
    }

    // === Game Loop ===
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        // --- Input Handling ---
        SpaceInvadersGame::Input& player_input = player_entity.get<SpaceInvadersGame::Input>();
        player_input.leftPressed = false;
        player_input.rightPressed = false;
        player_input.firePressed = false;

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) quit = true;
            if (e.type == SDL_EVENT_KEY_DOWN) {
                if (e.key.key == SDLK_LEFT) player_input.leftPressed = true;
                if (e.key.key == SDLK_RIGHT) player_input.rightPressed = true;
                if (e.key.key == SDLK_SPACE) player_input.firePressed = true;
            }
            if (e.type == SDL_EVENT_KEY_UP) {
                if (e.key.key == SDLK_LEFT) player_input.leftPressed = false;
                if (e.key.key == SDLK_RIGHT) player_input.rightPressed = false;
                if (e.key.key == SDLK_SPACE) player_input.firePressed = false;
            }
        }

        // --- System Execution ---
        SpaceInvadersGame::PlayerIntentSystem();
        SpaceInvadersGame::PlayerActionSystem();
        SpaceInvadersGame::EnemyLogicSystem();
        SpaceInvadersGame::EnemyShootingSystem();
        SpaceInvadersGame::MovementSystem();
        SpaceInvadersGame::CollisionSystem();
        SpaceInvadersGame::HealthSystem();
        SpaceInvadersGame::ScoreSystem();

        // === Rendering ===
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SpaceInvadersGame::RenderSystem(renderer);
        SDL_RenderPresent(renderer);

        // Stop updating the game after game over
        if (!player_entity.has<SpaceInvadersGame::Health>()) {
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
        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyTexture(gInvaderTexture);
    SDL_DestroyTexture(gPlayerTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
