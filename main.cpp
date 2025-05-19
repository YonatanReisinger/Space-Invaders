#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>

#include "bagel.h"
#include "SpaceInvaders.h"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr int PLAYER_WIDTH = 60;
constexpr int PLAYER_HEIGHT = 20;
constexpr int INVADER_WIDTH = 40;
constexpr int INVADER_HEIGHT = 30;
constexpr int INVADER_ROWS = 3;
constexpr int INVADER_COLS = 5;
constexpr int INVADER_X_GAP = 30;
constexpr int INVADER_Y_GAP = 20;

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


bool loadMedia(SDL_Renderer* renderer) {
    //Load PNG texture
    gInvaderTexture = IMG_LoadTexture(renderer, "res/invaders.png");
    if (gInvaderTexture == nullptr) {
        std::cerr << "Failed to load invader texture! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    gPlayerTexture = IMG_LoadTexture(renderer, "res/player.png");
     if (gPlayerTexture == nullptr) {
        std::cerr << "Failed to load player texture! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    return true;
}

void closeSDL(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_DestroyTexture(gInvaderTexture);
    gInvaderTexture = nullptr;
    SDL_DestroyTexture(gPlayerTexture);
    gPlayerTexture = nullptr;

    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    SDL_DestroyWindow(window);
    window = nullptr;

    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Space Invaders ECS", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    if (!loadMedia(renderer)) {
        std::cerr << "Failed to load media!" << std::endl;
        closeSDL(window, renderer);
        return 1;
    }

    // === Entity Creation ===
    // Create Player
    int player_id = SpaceInvadersGame::CreatePlayerEntity(WINDOW_WIDTH / 2.0f - PLAYER_WIDTH / 2.0f, WINDOW_HEIGHT - 60.0f);
    bagel::Entity player_entity(bagel::ent_type{player_id});
    player_entity.get<SpaceInvadersGame::Collider>().width = PLAYER_WIDTH;
    player_entity.get<SpaceInvadersGame::Collider>().height = PLAYER_HEIGHT;
    player_entity.get<SpaceInvadersGame::RenderData>().spriteId = 0; // Assuming spriteId 0 is for the player

    // Create Invaders
    int invaderStartX = 100;
    int invaderStartY = 60;
    for (int row = 0; row < INVADER_ROWS; ++row) {
        for (int col = 0; col < INVADER_COLS; ++col) {
            float x = invaderStartX + col * (INVADER_WIDTH + INVADER_X_GAP);
            float y = invaderStartY + row * (INVADER_HEIGHT + INVADER_Y_GAP);
            int invader_id = SpaceInvadersGame::CreateEnemyEntity(x, y, 10); // 10 points per invader
            bagel::Entity invader_entity(bagel::ent_type{invader_id});
            invader_entity.get<SpaceInvadersGame::Collider>().width = INVADER_WIDTH;
            invader_entity.get<SpaceInvadersGame::Collider>().height = INVADER_HEIGHT;
            invader_entity.get<SpaceInvadersGame::RenderData>().spriteId = (row + col) % 5; // Assign different sprites
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

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            } else if (e.type == SDL_EVENT_KEY_DOWN) {
                switch (e.key.key) {
                    case SDLK_LEFT:
                        player_input.leftPressed = true;
                        break;
                    case SDLK_RIGHT:
                        player_input.rightPressed = true;
                        break;
                    case SDLK_SPACE:
                        player_input.firePressed = true;
                        break;
                }
            } else if (e.type == SDL_EVENT_KEY_UP) {
                 switch (e.key.key) {
                    case SDLK_LEFT:
                        player_input.leftPressed = false; // This will be reset by PollEvent, need a better input system
                        break;
                    case SDLK_RIGHT:
                        player_input.rightPressed = false; // This will be reset by PollEvent, need a better input system
                        break;
                    case SDLK_SPACE:
                        player_input.firePressed = false; // This will be reset by PollEvent, need a better input system
                        break;
                }
            }
        }
         // Simple input state update (needs refinement for continuous press)
        const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);
        player_input.leftPressed = currentKeyStates[SDL_SCANCODE_LEFT];
        player_input.rightPressed = currentKeyStates[SDL_SCANCODE_RIGHT];
        player_input.firePressed = currentKeyStates[SDL_SCANCODE_SPACE];


        // --- System Execution ---
        // Order of systems is important
        SpaceInvadersGame::PlayerIntentSystem(); // Process input and set intentions
        SpaceInvadersGame::PlayerActionSystem(); // Act on player intentions (movement, shooting)
        SpaceInvadersGame::EnemyLogicSystem();   // Update enemy state (movement, shooting intention)
        SpaceInvadersGame::EnemyShootingSystem(); // Enemies act on shooting intention
        SpaceInvadersGame::MovementSystem();     // Update positions based on velocity
        SpaceInvadersGame::CollisionSystem();    // Check for and handle collisions
        SpaceInvadersGame::HealthSystem();       // Process health changes and mark dead entities
        SpaceInvadersGame::ScoreSystem();        // Update score based on dead entities
        // SpaceInvadersGame::CleanupSystem();   // System to remove dead entities (Need to implement)

        // === Rendering ===
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        SpaceInvadersGame::RenderSystem(renderer); // Draw entities

        // TODO: Draw player health
        // TODO: Draw score
        // TODO: Draw Game Over/Win state

        SDL_RenderPresent(renderer);

        // TODO: Cap frame rate

         // Basic Game Over check (needs more robust implementation)
        if (!player_entity.has<SpaceInvadersGame::Health>()) {
           // Game Over Logic (e.g., show message, wait for input)
           std::cout << "Game Over!" << std::endl;
           quit = true; // Simple exit for now
        }
        // TODO: Check for win condition (all invaders dead)

    }

    closeSDL(window, renderer);

    return 0;
}
