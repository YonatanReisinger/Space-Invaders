#pragma once
#include <cstdint>
#include "bagel.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

/**
 * @file SpaceInvaders.h
 * @brief Core module for the Space Invaders ECS game.
 *
 * This module defines all components, systems, and entity creation
 * functions needed for the Space Invaders gameplay mechanics.
 */

namespace SpaceInvadersGame {

// === Enums ===

/**
 * @brief Represents the direction of movement for entities.
 */
enum class Direction {
    Left = -1,
    None = 0,
    Right = 1
};

// === Components ===

/**
 * @brief Represents an entity's position in the world (Dense).
 */
struct Position {
    float x = 0.0f;
    float y = 0.0f;
};

/**
 * @brief Represents an entity's velocity (Sparse).
 */
struct Velocity {
    float x = 0.0f;
    float y = 0.0f;
};

/**
 * @brief Represents a bounding box for collision (Dense).
 */
struct Collider {
    float width = 1.0f;
    float height = 1.0f;
};

/**
 * @brief Tracks how much life the entity has left (Sparse).
 */
struct Health {
    int hp = 1;
};

/**
 * @brief Tag for the player entity (Tag).
 */
struct PlayerTag {};

/**
 * @brief Tag for enemy entities (Tag).
 */
struct EnemyTag {};

/**
 * @brief Tag for projectile entities (Tag).
 */
struct ProjectileTag {};

/**
 * @brief Temporary flag for entities that have fired a shot (Sparse).
 */
struct Shoots {
    bool value = false;
};

/**
 * @brief Determines how many points the entity gives when destroyed (Sparse).
 */
struct ScoreValue {
    int value = 0;
};

/**
 * @brief Visual/graphical data for drawing the entity (Dense).
 */
struct RenderData {
    int spriteId = 0; ///< Placeholder for sprite/texture reference
};

/**
 * @brief Tag for entities marked as dead (dynamic, optional).
 */
struct Dead {};

/**
 * @brief Represents player input (button states).
 */
struct Input {
    bool leftPressed = false;
    bool rightPressed = false;
    bool firePressed = false;
};

/**
 * @brief Represents the path and target for enemy movement.
 */
struct EnemyPath {
    float targetX = 0.0f;
    float targetY = 0.0f;
    int pathIndex = 0; ///< Index in a path array, if used
};

/**
 * @brief Tag indicating the entity wants to shoot (set by input or AI).
 */
struct WantsToShoot {};

// === Systems ===

void MovementSystem();
void RenderSystem(SDL_Renderer* renderer, SDL_Texture* gInvaderTexture, SDL_Texture* gPlayerTexture,
    SDL_FRect invaderSpriteRects[], SDL_FRect playerSpriteRect);
void CollisionSystem();
void PlayerShootingSystem();
void EnemyShootingSystem();
void HealthSystem();
void ScoreSystem();
void EnemyLogicSystem();
void PlayerIntentSystem();
void PlayerActionSystem();

// === Entity creation ===

int CreatePlayerEntity(float pos_x, float pos_y);
int CreateEnemyEntity(float pos_x, float pos_y, int score);
int CreateProjectileEntity(float pos_x, float pos_y, float vel_x, float vel_y, bool isPlayer);
int CreateExplosionEntity(float pos_x, float pos_y);
int CreateWallEntity(float pos_x, float pos_y, float width, float height, int hp);

} // namespace SpaceInvadersGame