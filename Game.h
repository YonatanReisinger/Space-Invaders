// Game.h
#pragma once

#include <cstdint>
#include <SDL3/SDL.h>

/**
 * @file Game.h
 * @brief Core module for the Sonic side-scroller game.
 *
 * This module defines all components, systems, and entity creation
 * functions needed for the Sonic side-scroller gameplay mechanics.
 */

namespace SonicGame {

// === Enums ===

    /**
     * @brief Represents the current animation state of an entity.
     */
    enum class eAnimationState {
        Idle = 0,
        Running = 1,
        SuperRunning = 2, /// (with powerup)
        Jumping = 3,
        Rolling = 4,
    };

    /**
     * @brief Represents the type of temporary power-up applied to an entity.
     */
    enum class ePowerupType {
        None = 0,
        Invincibility = 1,
        SpeedBoost = 2,
    };

    /**
     * @brief Represents the player's intention (based on input).
     */
    enum class eIntentState {
        None = 0,
        MoveLeft,
        MoveRight,
        Jump,
        Roll
    };

// === Components ===

    /**
     * @brief Represents an entity's position in the world.
     */
    struct Position {
        float x = 0.0f;
        float y = 0.0f;
    };

    /**
     * @brief Represents an entity's velocity.
     */
    struct Velocity {
        float x = 0.0f;
        float y = 0.0f;
    };

    /**
     * @brief Tag to indicate the entity can move.
     */
    struct MovementAbility {};

    /**
     * @brief Represents the current animation state of the entity.
     */
    struct Animation {
        eAnimationState currentState = eAnimationState::Idle;
    };

    /**
     * @brief Represents the entity's health points.
     */
    struct Health {
        int hp = 100;
    };

    /**
     * @brief Tag to indicate the entity is player-controlled.
     */
    struct PlayerTag {};

    /**
     * @brief Tag to indicate the entity is an enemy.
     */
    struct EnemyTag {};

    /**
     * @brief Tag to indicate the entity can collect items.
     */
    struct CollectorTag {};

    /**
     * @brief Represents how many rings the player has collected.
     */
    struct RingCount {
        int count = 0;
    };

    /**
     * @brief Tag to indicate the entity can be collected.
     */
    struct CollectableTag {};

    /**
     * @brief Tag to indicate the entity is a ring.
     */
    struct RingTag {};

    /**
     * @brief Represents collision information for the entity.
     */
    struct CollisionInfo {
        int collidedEntityId = -1;
    };

    /**
     * @brief Tag to indicate the entity is an obstacle.
     */
    struct ObstacleTag {};

    /**
     * @brief Tag to indicate the entity cause damage.
     */
    struct DamageTag {};

    /**
     * @brief Represents a temporary power-up on the entity.
     */
    struct TemporaryPowerup {
        ePowerupType type = ePowerupType::None;
        float duration = 0.0f;
    };

    /**
     * @brief Tag to indicate the entity is affected by gravity.
     */
    struct GravityTag {};

    /**
     * @brief Represents the current intent state of the player.
     */
    struct Intent {
        eIntentState current = eIntentState::None;
    };

    /**
     * @brief Tag indicating the entity is currently jumping.
     */
    struct JumpingTag {};

    /**
     * @brief Tag indicating the entity is currently rolling.
     */
    struct RollingTag {};

    /**
     * @brief  Indicating the user input.
     */
    struct Input {
        bool jumpPressed = false;
        bool leftPressed = false;
        bool rightPressed = false;
    };

    /**
     * @brief Indicates which powerup an item grants when collected.
     *
     * This component should be added to collectable entities that give
     * Sonic a temporary powerup (e.g., speed boost or invincibility).
     */
    struct PowerupTypeComponent {
        ePowerupType type = ePowerupType::None;
    };

// === Systems ===

    void PrintSystemHandlingEntity(const char* systemName, int entityId);
    void MovementSystem();
    void CollisionSystem();
    void AnimationSystem();
    void ItemCollectionSystem();
    void TemporaryEffectsSystem();
    void InputSystem();
    void RenderSystem();
    void IntentSystem();
    void ActionSystem();

// === Entity creation ===

    void PrintEntityCreated(const char* entityName, int id);
    int CreateSonicEntity(float pos_x, float pos_y);
    int CreateEnemyEntity(float pos_x, float pos_y);
    int CreateRingEntity(float pos_x, float pos_y);
    int CreateObstacleEntity(float pos_x, float pos_y);
    int CreatePlatformEntity(float pos_x, float pos_y);
    int CreateSpikesEntity(float pos_x, float pos_y);

} // namespace SonicGame
