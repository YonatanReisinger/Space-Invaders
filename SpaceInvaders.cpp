#include "SpaceInvaders.h"

#include <cassert>

#include "SpaceInvadersConfig.h"
#include <iostream>
#include <random>

namespace SpaceInvadersGame {

    //Returns a random number between 0 and n-1
    int getRandomNumber(int n) {
        static std::random_device rd;
        static std::mt19937 gen(rd()); // Static to avoid reseeding on every call
        std::uniform_int_distribution<> distrib(0, n);

        return distrib(gen);
    }
// === Systems Implementations ===

/**
 * @brief Moves entities according to their velocity.
 * Required: Position, Velocity
 */
void MovementSystem() {
    for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
        bagel::ent_type ent{id};
        if (!bagel::World::mask(ent).test(bagel::Component<Position>::Bit) ||
            !bagel::World::mask(ent).test(bagel::Component<Velocity>::Bit)) {
            continue;
        }
        auto& pos = bagel::World::getComponent<Position>(ent);
        const auto& vel = bagel::World::getComponent<Velocity>(ent);
        pos.x += vel.x;
        pos.y += vel.y;
    }
}

/**
 * @brief Draws all visible entities to the screen.
 * Required: Position, RenderData
 */
void RenderSystem(SDL_Renderer* renderer, SDL_Texture* gInvaderTexture, SDL_Texture* gPlayerTexture,
    SDL_FRect invaderSpriteRects[], SDL_FRect playerSpriteRect) {

    //SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderClear(renderer);
    // Draw player
    for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
        bagel::ent_type ent{id};
        if (!bagel::World::mask(ent).test(bagel::Component<PlayerTag>::Bit) ||
            !bagel::World::mask(ent).test(bagel::Component<Position>::Bit) ||
            !bagel::World::mask(ent).test(bagel::Component<RenderData>::Bit) ||
            bagel::World::mask(ent).test(bagel::Component<ProjectileTag>::Bit)) {
            continue;
        }
        const auto& pos = bagel::World::getComponent<Position>(ent);
        SDL_FRect dest = {pos.x, pos.y, (float)60, (float)20};
        //SDL_RenderClear(renderer);
        if (!SDL_RenderTexture(renderer, gPlayerTexture, &playerSpriteRect, &dest))
            std::cerr << "RenderTexture failed: " << SDL_GetError() << std::endl;
        //SDL_RenderPresent(renderer);
    }

    // Draw invaders
    for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
        bagel::ent_type ent{id};
        if (!bagel::World::mask(ent).test(bagel::Component<EnemyTag>::Bit) ||
            !bagel::World::mask(ent).test(bagel::Component<Position>::Bit) ||
            !bagel::World::mask(ent).test(bagel::Component<RenderData>::Bit) ||
            bagel::World::mask(ent).test(bagel::Component<ProjectileTag>::Bit)) {
            continue;
        }
        const auto& pos = bagel::World::getComponent<Position>(ent);
        const auto& rend = bagel::World::getComponent<RenderData>(ent);
        int spriteIdx = rend.spriteId % 5;
        SDL_FRect dest = {pos.x, pos.y, (float)40, (float)30};
        if (!SDL_RenderTexture(renderer, gInvaderTexture, &invaderSpriteRects[spriteIdx], &dest))
            std::cerr << "RenderTexture failed: " << SDL_GetError() << std::endl;
    }

    // Draw projectiles
    for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
        bagel::ent_type ent{id};
        if (!bagel::World::mask(ent).test(bagel::Component<ProjectileTag>::Bit) ||
            !bagel::World::mask(ent).test(bagel::Component<Position>::Bit)) {
            continue;
        }
        const auto& pos = bagel::World::getComponent<Position>(ent);
        SDL_FRect rect = {pos.x, pos.y, 6.0f, 16.0f};
        if (bagel::World::mask(ent).test(bagel::Component<PlayerTag>::Bit)) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
        }
        if (!SDL_RenderFillRect(renderer, &rect))
            std::cerr << "RenderFillRect failed: " << SDL_GetError() << std::endl;
    }
    SDL_RenderPresent(renderer);
}

bool AreEntitiesPlayerAndEnemy(bagel::ent_type ent1, bagel::ent_type ent2){
        return (bagel::World::mask(ent1).test(bagel::Component<EnemyTag>::Bit) &&
            bagel::World::mask(ent2).test(bagel::Component<PlayerTag>::Bit)) ||
                (bagel::World::mask(ent1).test(bagel::Component<PlayerTag>::Bit) &&
            bagel::World::mask(ent2).test(bagel::Component<EnemyTag>::Bit));
    }

bool IsEntityOutOfView(bagel::ent_type ent){
        if (bagel::World::mask(ent).test(bagel::Component<Position>::Bit))
        {
            const auto& pos = bagel::World::getComponent<Position>(ent);
            return pos.x < 0 || pos.x > WINDOW_WIDTH || pos.y < 0 || pos.y > WINDOW_HEIGHT;
        }
        return false;
    }

void DeleteOffscreenEntitiesSystem(){
        for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
            bagel::ent_type ent{id};
            if (IsEntityOutOfView(ent)) {
                //std::cerr << id << " Entity out of view!" << std::endl;
                bagel::World::addComponent(ent, Dead{});
            }}
    }
/**
 * @brief Detects and handles collisions between entities.
 * Required: Position, Collider
 * Optional: Health, ScoreValue, Dead
 */
void CollisionSystem() {
    for (bagel::id_type id1 = 0; id1 <= bagel::World::maxId().id; ++id1) {
        bagel::ent_type ent1{id1};
        if (!bagel::World::mask(ent1).test(bagel::Component<Position>::Bit) ||
            !bagel::World::mask(ent1).test(bagel::Component<Collider>::Bit)) {
            continue;
        }
        const auto& pos1 = bagel::World::getComponent<Position>(ent1);
        const auto& col1 = bagel::World::getComponent<Collider>(ent1);
        for (bagel::id_type id2 = id1 + 1; id2 <= bagel::World::maxId().id; ++id2) {
            bagel::ent_type ent2{id2};
            if (!bagel::World::mask(ent2).test(bagel::Component<Position>::Bit) ||
                !bagel::World::mask(ent2).test(bagel::Component<Collider>::Bit)) {
                continue;
            }
            const auto& pos2 = bagel::World::getComponent<Position>(ent2);
            const auto& col2 = bagel::World::getComponent<Collider>(ent2);
            if (pos1.x < pos2.x + col2.width &&
                pos1.x + col1.width > pos2.x &&
                pos1.y < pos2.y + col2.height &&
                pos1.y + col1.height > pos2.y &&
                AreEntitiesPlayerAndEnemy(ent1, ent2)) {
                // Collision detected
                if (bagel::World::mask(ent1).test(bagel::Component<Health>::Bit)) {
                    auto& health1 = bagel::World::getComponent<Health>(ent1);
                    health1.hp--;
                    if (health1.hp <= 0) {
                        bagel::World::addComponent(ent1, Dead{});
                    }
                }
                if (bagel::World::mask(ent2).test(bagel::Component<Health>::Bit)) {
                    auto& health2 = bagel::World::getComponent<Health>(ent2);
                    health2.hp--;
                    if (health2.hp <= 0) {
                        bagel::World::addComponent(ent2, Dead{});
                    }
                }
            }
        }
    }
}

/**
 * @brief Handles player shooting logic.
 * Required: PlayerTag, Shoots, Position, WantsToShoot
 */
void PlayerShootingSystem() {
    // Only allow one player bullet at a time (like the original demo)
    bool playerBulletExists = false;
    for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
        bagel::ent_type ent{id};
        if (bagel::World::mask(ent).test(bagel::Component<ProjectileTag>::Bit) &&
            bagel::World::mask(ent).test(bagel::Component<PlayerTag>::Bit)) {
            playerBulletExists = true;
            break;
        }
    }

    for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
        bagel::ent_type ent{id};
        if (!bagel::World::mask(ent).test(bagel::Component<PlayerTag>::Bit) ||
            !bagel::World::mask(ent).test(bagel::Component<Input>::Bit) ||
            !bagel::World::mask(ent).test(bagel::Component<Position>::Bit)) {
            continue;
        }
        const auto& input = bagel::World::getComponent<Input>(ent);
        const auto& pos = bagel::World::getComponent<Position>(ent);
        if (input.firePressed && !playerBulletExists) {
            // Fire a bullet from the center top of the player
            SpaceInvadersGame::CreateProjectileEntity(
                pos.x + 0.5f * PLAYER_WIDTH - 0.5f * 6.0f, // center horizontally
                pos.y - 16.0f, // just above the player
                0.0f, -8.0f, true);
            break; // Only fire one bullet per frame
        }
    }
}

/**
 * @brief Handles enemy shooting logic.
 * Required: EnemyTag, Shoots, Position, WantsToShoot
 */
void EnemyShootingSystem() {
    for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
        bagel::ent_type ent{id};
        if (!bagel::World::mask(ent).test(bagel::Component<EnemyTag>::Bit) ||
            !bagel::World::mask(ent).test(bagel::Component<Position>::Bit) ||
            !bagel::World::mask(ent).test(bagel::Component<Shoots>::Bit)) {
            continue;
        }
        const auto& pos = bagel::World::getComponent<Position>(ent);
        const auto& shoots = bagel::World::getComponent<Shoots>(ent);
        int toShoot = getRandomNumber(10);

        if (shoots.value && !toShoot) {
            // Fire a bullet from the center bottom of the enemy
            SpaceInvadersGame::CreateProjectileEntity(
                pos.x + 0.5f * INVADER_WIDTH - 0.5f * BULLET_WIDTH, // center horizontally
                pos.y + INVADER_HEIGHT, // just below the enemy
                0.0f, INVADER_BULLET_SPEED, false);
        }
    }
}

/**
 * @brief Reduces health when needed and marks entities as dead.
 * Required: Health
 * Optional: Dead
 */
void HealthSystem() {
    for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
        bagel::ent_type ent{id};
        if (bagel::World::mask(ent).test(bagel::Component<Dead>::Bit)) {
            bagel::World::destroyEntity(ent);
            std::cout << id << " Entity Destroyed" << std::endl;
        }
    }
}

/**
 * @brief Adds to the score when entities with ScoreValue are destroyed.
 * Required: ScoreValue, Dead
 */
void ScoreSystem() {
    static int score = 0;
    for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
        bagel::ent_type ent{id};
        if (bagel::World::mask(ent).test(bagel::Component<Dead>::Bit) &&
            bagel::World::mask(ent).test(bagel::Component<ScoreValue>::Bit)) {
            const auto& scoreVal = bagel::World::getComponent<ScoreValue>(ent);
            score += scoreVal.value;
            std::cout << "Score: " << score << std::endl;
        }
    }
}

/**
 * @brief Handles general enemy logic (movement, speed, shooting, etc).
 * Required: EnemyTag
 */
void EnemyLogicSystem() {
    static int invaderMoveCounter = 0;
    static int invaderDir = 1; // 1=right, -1=left
    constexpr int INVADER_MOVE_INTERVAL = 30;
    constexpr float INVADER_MOVE_STEP = 20.0f;

    invaderMoveCounter++;
    if (invaderMoveCounter >= INVADER_MOVE_INTERVAL) {
        invaderMoveCounter = 0;
        float minX = 800.0f, maxX = 0.0f;
        for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
            bagel::ent_type ent{id};
            if (!bagel::World::mask(ent).test(bagel::Component<EnemyTag>::Bit) ||
                !bagel::World::mask(ent).test(bagel::Component<Position>::Bit)) {
                continue;
            }
            auto& pos = bagel::World::getComponent<Position>(ent);
            pos.x += invaderDir * INVADER_MOVE_STEP;
            if (pos.x < minX) minX = pos.x;
            if (pos.x + 40.0f > maxX) maxX = pos.x + 40.0f;
        }
        if (minX < 10.0f || maxX > 790.0f) {
            invaderDir *= -1;
        }
    }

    // Random shooting for enemies
    for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
        bagel::ent_type ent{id};
        if (!bagel::World::mask(ent).test(bagel::Component<EnemyTag>::Bit) ||
            !bagel::World::mask(ent).test(bagel::Component<Position>::Bit) ||
            !bagel::World::mask(ent).test(bagel::Component<Shoots>::Bit)) {
            continue;
        }
        auto& shoots = bagel::World::getComponent<Shoots>(ent);
        if (rand() % 60 == 0) {
            shoots.value = true;
        } else {
            shoots.value = false;
        }
    }
}

/**
 * @brief Translates player input (buttons) into an intention.
 * Required: PlayerTag, Input
 */
void PlayerIntentSystem() {
    for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
        bagel::ent_type ent{id};
        if (!bagel::World::mask(ent).test(bagel::Component<PlayerTag>::Bit) ||
            !bagel::World::mask(ent).test(bagel::Component<Input>::Bit) ||
            !bagel::World::mask(ent).test(bagel::Component<Velocity>::Bit)) {
            continue;
        }
        const auto& input = bagel::World::getComponent<Input>(ent);
        auto& vel = bagel::World::getComponent<Velocity>(ent);
        vel.x = 0.0f;
        if (input.leftPressed) vel.x = -6.0f;
        if (input.rightPressed) vel.x = 6.0f;
    }
}

/**
 * @brief Translates intention into actions (movement, shooting, etc).
 * Required: PlayerTag, Intention
 */
void PlayerActionSystem() {
    for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
        bagel::ent_type ent{id};
        if (!bagel::World::mask(ent).test(bagel::Component<PlayerTag>::Bit) ||
            !bagel::World::mask(ent).test(bagel::Component<Input>::Bit) ||
            !bagel::World::mask(ent).test(bagel::Component<Position>::Bit) ||
            !bagel::World::mask(ent).test(bagel::Component<Shoots>::Bit)) {
            continue;
        }
        const auto& input = bagel::World::getComponent<Input>(ent);
        const auto& pos = bagel::World::getComponent<Position>(ent);
        auto& shoots = bagel::World::getComponent<Shoots>(ent);
        if (input.firePressed && !shoots.value) {
            shoots.value = true;
            // Create a projectile
            SpaceInvadersGame::CreateProjectileEntity(pos.x + 30.0f, pos.y, 0.0f, -8.0f, true);
        } else if (!input.firePressed) {
            shoots.value = false;
        }
    }
}

// === Entity Creation Implementations ===

/**
 * @brief Creates the player entity.
 */
int CreatePlayerEntity(float pos_x, float pos_y) {
    bagel::Entity player = bagel::Entity::create();
    player.addAll(
        Position{pos_x, pos_y},
        Velocity{0.0f, 0.0f},
        RenderData{0},
        Collider{1.0f, 1.0f},
        PlayerTag{},
        Health{1},
        Shoots{false},
        Input{},
        WantsToShoot{}
    );
    std::cout << "Created Player Entity with ID: " << player.entity().id << std::endl;
    return player.entity().id;
}

/**
 * @brief Creates an enemy entity.
 */
int CreateEnemyEntity(float pos_x, float pos_y, int score) {
    bagel::Entity enemy = bagel::Entity::create();
    enemy.addAll(
        Position{pos_x, pos_y},
        Velocity{0.0f, 0.0f},
        RenderData{0},
        Collider{1.0f, 1.0f},
        EnemyTag{},
        Health{1},
        ScoreValue{score},
        Shoots{false},
        EnemyPath{},
        WantsToShoot{}
    );
    std::cout << "Created Enemy Entity with ID: " << enemy.entity().id << std::endl;
    return enemy.entity().id;
}

/**
 * @brief Creates a projectile entity.
 */
int CreateProjectileEntity(float pos_x, float pos_y, float vel_x, float vel_y, bool isPlayer) {
    bagel::Entity proj = bagel::Entity::create();
    proj.addAll(
        Position{pos_x, pos_y},
        Velocity{vel_x, vel_y},
        RenderData{2},
        Collider{0.2f, 0.5f},
        ProjectileTag{},
        Health{1}
    );
    if (isPlayer) {
        proj.add(PlayerTag{});
    } else {
        proj.add(EnemyTag{});
    }
    std::cout << "Created Projectile Entity with ID: " << proj.entity().id << std::endl;
    return proj.entity().id;
}

/**
 * @brief Creates an explosion entity.
 */
int CreateExplosionEntity(float pos_x, float pos_y) {
    bagel::Entity explosion = bagel::Entity::create();
    explosion.addAll(
        Position{pos_x, pos_y},
        RenderData{3},
        Dead{}
    );
    std::cout << "Created Explosion Entity with ID: " << explosion.entity().id << std::endl;
    return explosion.entity().id;
}

/**
 * @brief Creates a wall entity.
 */
int CreateWallEntity(float pos_x, float pos_y, float width, float height, int hp) {
    bagel::Entity wall = bagel::Entity::create();
    wall.addAll(
        Position{pos_x, pos_y},
        Collider{width, height},
        Health{hp},
        RenderData{4}
    );
    std::cout << "Created Wall Entity with ID: " << wall.entity().id << std::endl;
    return wall.entity().id;
}

} // namespace SpaceInvadersGame 