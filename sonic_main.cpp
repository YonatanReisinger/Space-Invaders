#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <box2d/box2d.h>
#include <iostream>
#include <vector>

using namespace std;

/// This main test the Game.cpp file
//int main() {
//    using namespace SonicGame;
//
//    CreateSonicEntity(0,0);
//    CreateEnemyEntity(1,1);
//    CreateRingEntity(2,2);
//
//    MovementSystem();
//    CollisionSystem();
//    AnimationSystem();
//    ItemCollectionSystem();
//    TemporaryEffectsSystem();
//    InputSystem();
//    RenderSystem();
//    ActionSystem();
//    IntentSystem();
//
//    return 0;
//}

struct Ring {
    b2BodyId body;
    SDL_FRect texRect;
    float animPhase;
};

struct Enemy {
    b2BodyId body;
    SDL_FRect texRect;
    bool movingRight;
};


class Sonic {
public:
    Sonic();
    ~Sonic();
    void run();

private:
    static constexpr int FPS = 60;
    static constexpr float BOX_SCALE = 10;
    static constexpr float TEX_SCALE = 2.0f;
    static constexpr SDL_FRect SONIC_RUN_TEX = {190, 97, 48, 48};
    static constexpr SDL_FRect SONIC_SUPERRUN_TEX = {46, 144, 48, 48};
    static constexpr int TOTAL_FRAMES = 8;

    std::vector<Ring> rings;
    SDL_Texture* ringTexture;

    std::vector<Enemy> enemies;
    SDL_Texture* enemyTexture;

    SDL_Texture* walkingTex;
    int currentFrame = 0;
    int frameCounter = 0;
    bool isSuperRun = false;

    SDL_Renderer* ren;
    SDL_Window* win;

    b2WorldId world; /// physics world
    b2BodyId ballBody; /// sonic's physics body
};

Sonic::Sonic() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        cout << SDL_GetError() << endl;
        return;
    }

    /// create SDL window and renderer
    if (!SDL_CreateWindowAndRenderer("Sonic Game", 800, 600, 0, &win, &ren)) {
        cout << SDL_GetError() << endl;
        return;
    }

    SDL_Surface* walkSurf = IMG_Load("res/sonic_sprite.png");
    if (walkSurf == nullptr) {
        cout << SDL_GetError() << endl;
        return;
    }

    walkingTex = SDL_CreateTextureFromSurface(ren, walkSurf);
    if (walkingTex == nullptr) {
        cout << SDL_GetError() << endl;
        return;
    }

    /// creating the surface and texture of the ring
    SDL_Surface* ringSurf = IMG_Load("res/ring.png");
    if (ringSurf == nullptr) {
        cout << SDL_GetError() << endl;
        return;
    }

    ringTexture = SDL_CreateTextureFromSurface(ren, ringSurf);
    if (ringTexture == nullptr) {
        cout << SDL_GetError() << endl;
        return;
    }

    /// creating the surface and texture of the enemy
    SDL_Surface* enemySurf = IMG_Load("res/enemies.png");
    if (enemySurf == nullptr) {
        cout << SDL_GetError() << endl;
        return;
    }

    enemyTexture = SDL_CreateTextureFromSurface(ren, enemySurf);
    if (enemyTexture == nullptr) {
        cout << SDL_GetError() << endl;
        return;
    }

    /// enable transparency for the ring and enemy texture
    SDL_SetTextureBlendMode(ringTexture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(enemyTexture, SDL_BLENDMODE_BLEND);
    SDL_DestroySurface(enemySurf);

    /// clean up surface
    SDL_DestroySurface(walkSurf);
    SDL_DestroySurface(ringSurf);
    SDL_DestroySurface(enemySurf);

    /// create Box2D world with gravity
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = {0.0f, 9.8f};
    world = b2CreateWorld(&worldDef);

    /// create sonic physics body
    b2BodyDef sonicBodyDef = b2DefaultBodyDef();
    sonicBodyDef.type = b2_dynamicBody;
    sonicBodyDef.position = {100 / BOX_SCALE, 500 / BOX_SCALE};
    ballBody = b2CreateBody(world, &sonicBodyDef);

    b2ShapeDef sonicShapeDef = b2DefaultShapeDef();
    sonicShapeDef.density = 1.0f;
    sonicShapeDef.material.friction = 0.5f;
    sonicShapeDef.material.restitution = 0.0f;

    /// could be useful for later - create sonic's circle body - for rolling
    //b2Circle sonicCircle = {0, 0, SONIC_RUN_TEX.w * TEX_SCALE / BOX_SCALE};
    //b2CreateCircleShape(ballBody, &sonicShapeDef, &sonicCircle);

    /// create floor body
    b2BodyDef floorBodyDef = b2DefaultBodyDef();
    floorBodyDef.type = b2_staticBody;
    floorBodyDef.position = {800 / 2 / BOX_SCALE, 600 / BOX_SCALE};
    b2BodyId floorBody = b2CreateBody(world, &floorBodyDef);

    b2Polygon groundBox = b2MakeBox(800 / 2 / BOX_SCALE, 1.0f);
    b2CreatePolygonShape(floorBody, &sonicShapeDef, &groundBox);

    /// create rings
    for (int i = 0; i < 5; ++i) {
        b2BodyDef ringBodyDef = b2DefaultBodyDef();
        ringBodyDef.type = b2_staticBody;
        ringBodyDef.position = { (200 + i * 60) / BOX_SCALE, (480) / BOX_SCALE };
        b2BodyId ringBody = b2CreateBody(world, &ringBodyDef);

        Ring ring;
        ring.body = ringBody;
        ring.texRect = {0, 0, 32, 32};
        ring.animPhase = i * 0.5f;
        rings.push_back(ring);
    }

    /// create enemy
    b2BodyDef enemyBodyDef = b2DefaultBodyDef();
    enemyBodyDef.type = b2_kinematicBody;
    enemyBodyDef.position = {500 / BOX_SCALE, 500 / BOX_SCALE};

    b2BodyId enemyBody = b2CreateBody(world, &enemyBodyDef);

    Enemy enemy;
    enemy.body = enemyBody;
    enemy.texRect = {0, 0, 32, 32};
    enemy.movingRight = true;
    enemies.push_back(enemy);

}

Sonic::~Sonic() {
    if (walkingTex != nullptr) SDL_DestroyTexture(walkingTex);
    if (ringTexture != nullptr) SDL_DestroyTexture(ringTexture);
    if (ren != nullptr) SDL_DestroyRenderer(ren);
    if (win != nullptr) SDL_DestroyWindow(win);
    if (enemyTexture != nullptr) SDL_DestroyTexture(enemyTexture);
    SDL_Quit();
}

void Sonic::run() {
    float time = 0.0f;
    SDL_FRect currentTex = SONIC_RUN_TEX;
    SDL_SetRenderDrawColor(ren, 135, 206, 235, 255);
    SDL_RenderClear(ren);

    SDL_FRect sonicRect{0, 0, currentTex.w * TEX_SCALE, currentTex.h * TEX_SCALE};

    constexpr float STEP_TIME = 1.f / FPS;

    for (int frame = 0; frame < 1000; ++frame) {
        /// physics simulation step
        b2World_Step(world, STEP_TIME, 4);

        /// move Sonic right
        b2Vec2 sonicVelocity = b2Body_GetLinearVelocity(ballBody);
        sonicVelocity.x = 2.0f;
        b2Body_SetLinearVelocity(ballBody, sonicVelocity);

        b2Vec2 sonicPosition = b2Body_GetPosition(ballBody);
        sonicRect.x = sonicPosition.x * BOX_SCALE;
        sonicRect.y = sonicPosition.y * BOX_SCALE - (currentTex.h * TEX_SCALE / 2);

        SDL_RenderClear(ren); /// clear screen each frame

        /// update animation frame every few ticks
        frameCounter++;
        if (frameCounter >= 5) {
            currentFrame = (currentFrame + 1) % TOTAL_FRAMES;
            frameCounter = 0;
        }

        /// find correct location of sonic in sprite sheet
        SDL_FRect srcFrameF = {
                currentTex.x + currentFrame * currentTex.w,
                currentTex.y,
                currentTex.w,
                currentTex.h
        };

        /// destination rectangle where sonic will be drawn
        SDL_FRect destRect = {
                sonicRect.x,
                sonicRect.y,
                currentTex.w * TEX_SCALE,
                currentTex.h * TEX_SCALE
        };

        /// draw sonic
        SDL_RenderTexture(ren, walkingTex, &srcFrameF, &destRect);

        /// draw the rings
        for (Ring& ring : rings) {
            b2Vec2 pos = b2Body_GetPosition(ring.body);
            /// add wave offset to Y
            float offsetY = 5.0f * sinf(time + ring.animPhase);
            SDL_FRect dest = { pos.x * BOX_SCALE, (pos.y * BOX_SCALE) + offsetY, ring.texRect.w, ring.texRect.h };
            SDL_RenderTexture(ren, ringTexture, NULL, &dest);
        }

        /// draw the enemy
        for (Enemy& enemy : enemies) {
            b2Vec2 pos = b2Body_GetPosition(enemy.body);

            // Check boundaries and reverse direction
            if (pos.x * BOX_SCALE > 600) enemy.movingRight = false;
            if (pos.x * BOX_SCALE < 400) enemy.movingRight = true;

            b2Vec2 vel = b2Body_GetLinearVelocity(enemy.body);
            vel.x = enemy.movingRight ? 1.0f : -1.0f;
            b2Body_SetLinearVelocity(enemy.body, vel);
        }

        for (Enemy& enemy : enemies) {
            b2Vec2 pos = b2Body_GetPosition(enemy.body);
            SDL_FRect dest = { pos.x * BOX_SCALE, pos.y * BOX_SCALE, enemy.texRect.w, enemy.texRect.h };
            SDL_RenderTexture(ren, enemyTexture, NULL, &dest);
        }

        SDL_RenderPresent(ren); /// present everything to screen

        if (isSuperRun)
            SDL_Delay(1); /// sonic runs faster in super-run
        else
            SDL_Delay(3);

        time = SDL_GetTicks() / 100.0f;

        /// switch to superrun animation after 500 frames
        if (frame == 500) {
            isSuperRun = true;
            currentTex = SONIC_SUPERRUN_TEX;
        }
    }
}

int main() {
    Sonic sonic;
    sonic.run();
    return 0;
}