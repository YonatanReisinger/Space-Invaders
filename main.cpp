#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
using namespace std;

#include "bagel.h"
#include "SpaceInvaders.h"
#include "SpaceInvadersConfig.h"
#include "box2d/box2d.h"
#include "box2d/types.h"

SDL_Texture* invaderTexture = nullptr;
SDL_Texture* playerTexture = nullptr;
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
b2BodyId playerBody;
b2BodyId invaderBody;

constexpr float BOX_SCALE = 1;
constexpr float TEX_SCALE = 3;
constexpr SDL_FRect PLAYER_REC = {16,16, 42, 56};
b2WorldId world;

SDL_FRect invaderSpriteRects[NUM_OF_INVADERS_TYPES][NUM_OF_INVADERS_POSTURES_PER_TYPE] = {
    {{5, 1, 8, 8}, {5, 11, 8, 8}},
    {{22, 1, 11, 8}, {22, 11, 11, 8}},
    {{39, 1, 13, 8}, {39, 11, 13, 8}}
    };
SDL_FRect playerSpriteRect = PLAYER_REC;
SDL_FRect gameOverSpriteRects[] = {
    {61, 69, 8, 8}, //G
{1, 69, 8, 8}, //A
{41, 79, 8, 8}, //M
{41, 69, 8, 8}, //E
{21, 119, 8, 8}, //[SPACE]
{61, 79, 8, 8}, //O
{51, 89, 8, 8}, //V
{41, 69, 8, 8}, //E
{11, 89, 8, 8}, //R
};

SDL_Texture* get_texture(const char* SheetPath)
{
    SDL_Surface* surf = IMG_Load(SheetPath);
    if (surf == nullptr) {
        cout << SDL_GetError() << endl;
        return nullptr;
    }

    Uint32 colorKey = SDL_MapRGB(SDL_GetPixelFormatDetails(surf->format),
        nullptr, 40, 40, 40);
    SDL_SetSurfaceColorKey(surf, true, colorKey);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surf);
    if (texture == nullptr) {
        cout << SDL_GetError() << endl;
        return nullptr;
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    SDL_DestroySurface(surf);

    return texture;
}
b2BodyId getEntityBody(int x, int y, b2BodyDef& bodyDef, b2ShapeDef& shapeDef)
{
    bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = { x / BOX_SCALE, y / BOX_SCALE };
    b2BodyId body = b2CreateBody(world, &bodyDef);

    shapeDef = b2DefaultShapeDef();
    float halfWidth = (PLAYER_REC.w * TEX_SCALE) / (2 * BOX_SCALE);
    float halfHeight = (PLAYER_REC.h * TEX_SCALE) / (2 * BOX_SCALE);
    b2Polygon shape = b2MakeBox(halfWidth, halfHeight);
    b2CreatePolygonShape(body, &shapeDef, &shape);

    return body;
}

void printGameOver()
{
    SDL_FRect dest = {290, 290, 8 * TEX_SCALE, 8 * TEX_SCALE}; // scale if needed
    for (SDL_FRect rect : gameOverSpriteRects) {
        SDL_RenderTexture(renderer, invaderTexture, &rect, &dest);
        dest.x += 8 * TEX_SCALE; // scale x offset too
    }
    SDL_RenderPresent(renderer); // <-- THIS is crucial
}

int main() {
    using namespace SpaceInvadersGame;
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (!SDL_CreateWindowAndRenderer(
    "Space Invaders", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer)) {
        std::cerr << SDL_GetError() << std::endl;
        return 1;
    }

    invaderTexture = get_texture("res/invaders2.png");
    playerTexture = get_texture("res/player.png");

    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = { 0,0 };
    world = b2CreateWorld(&worldDef);

    b2BodyDef bodyDef;
    b2ShapeDef shapeDef;
    playerBody = getEntityBody(0,0, bodyDef, shapeDef);
    invaderBody = getEntityBody( 10,0 , bodyDef, shapeDef);

    // === Entity Creation ===
    bagel::World::createEntity(); //Created So Player Entity won't have the id 0.
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
            invader_entity.get<SpaceInvadersGame::RenderData>().spriteId = (row) % NUM_OF_INVADERS_TYPES;
        }
    }

    // === Game Loop ===
    bool quit = false;
    SDL_Event e;

    //int speed = 16;
    //int step = 0;
    const bool *keystates = SDL_GetKeyboardState(NULL);
    while (!quit) {
        // --- Input Handling ---
        SDL_PumpEvents(); // Make sure keyboard state is up to date

        SpaceInvadersGame::Input& player_input = player_entity.get<SpaceInvadersGame::Input>();
        player_input.leftPressed = keystates[SDL_SCANCODE_LEFT];
        player_input.rightPressed = keystates[SDL_SCANCODE_RIGHT];
        player_input.firePressed = keystates[SDL_SCANCODE_SPACE];

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
        SpaceInvadersGame::ChangeEnemyPostureSystem();
        SpaceInvadersGame::MovementSystem();
        SpaceInvadersGame::CollisionSystem();
        SpaceInvadersGame::HealthSystem();
        SpaceInvadersGame::ScoreSystem();
        //SpaceInvadersGame::DeleteOffscreenEntitiesSystem();

        // === Rendering ===
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SpaceInvadersGame::RenderSystem(renderer, invaderTexture, playerTexture,
            invaderSpriteRects, playerSpriteRect);

        // Stop updating the game after game over
        if (!player_entity.has<SpaceInvadersGame::Health>()) {
            SDL_Delay(DELAY_BEFORE_GAMEOVER);
            while (true) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);
                printGameOver();
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

    SDL_DestroyTexture(invaderTexture);
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
