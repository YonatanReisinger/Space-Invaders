// Game.cpp
#include "Game.h"
#include "bagel.h"
#include <iostream>

namespace SonicGame {

// === Systems Implementations ===

    /**
     * @brief Prints which system is handling a specific entity.
     *
     * This function outputs the name of the system and the entity ID currently being processed to the console.
     *
     * @param systemName The name of the system handling the entity.
     * @param entityId The ID of the entity being handled.
     */
    void PrintSystemHandlingEntity(const char* systemName, int entityId) {
        std::cout << systemName << ": Handling entity ID " << entityId << std::endl;
    }

    /**
     * @brief Processes movement-related behavior for entities.
     *
     * This system iterates through all entities and applies movement logic to those that have the necessary components.
     * Necessary components: Position, Velocity, MovementAbility, and GravityTag.
     * Optional components: Intent or TemporaryPowerup (for SpeedBoost speed up).
     */
    void MovementSystem() {
        bagel::Mask required;
        required.set(bagel::Component<Position>::Bit);
        required.set(bagel::Component<Velocity>::Bit);
        required.set(bagel::Component<MovementAbility>::Bit);
        required.set(bagel::Component<GravityTag>::Bit);

        for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
            bagel::ent_type ent{id};

            if (bagel::World::mask(ent).test(required)) {
                bool hasIntent = bagel::World::mask(ent).test(bagel::Component<Intent>::Bit);
                bool hasTemporaryPowerup = bagel::World::mask(ent).test(bagel::Component<TemporaryPowerup>::Bit);
                PrintSystemHandlingEntity("MovementSystem", id);
            }
        }
    }

    /**
     * @brief Processes collision-related behavior for entities.
     *
     * This system iterates through all entities and applies collision logic to those that have the necessary components.
     * Necessary components: Position and CollisionInfo.
     * Optional components (for collision with enemy): PlayerTag, EnemyTag, DamageTag, RollingTag, Health, ObstacleTag, TemporaryPowerup.
     * Optional components (for collision with collactable): CollectorTag, CollectableTag.
     */
    void CollisionSystem() {
        bagel::Mask required;
        required.set(bagel::Component<Position>::Bit);
        required.set(bagel::Component<CollisionInfo>::Bit);

        for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
            bagel::ent_type ent{id};

            if (bagel::World::mask(ent).test(required)) {
                bool isPlayer = bagel::World::mask(ent).test(bagel::Component<PlayerTag>::Bit);
                bool isEnemy = bagel::World::mask(ent).test(bagel::Component<EnemyTag>::Bit);
                bool hasDamage = bagel::World::mask(ent).test(bagel::Component<DamageTag>::Bit);
                bool hasHealth = bagel::World::mask(ent).test(bagel::Component<Health>::Bit);
                bool isObstacle = bagel::World::mask(ent).test(bagel::Component<ObstacleTag>::Bit);
                bool hasTemporaryPowerup = bagel::World::mask(ent).test(bagel::Component<TemporaryPowerup>::Bit);
                bool isCollector = bagel::World::mask(ent).test(bagel::Component<CollectorTag>::Bit);
                bool isCollectable = bagel::World::mask(ent).test(bagel::Component<CollectableTag>::Bit);
                bool isRolling = bagel::World::mask(ent).test(bagel::Component<RollingTag>::Bit);
                PrintSystemHandlingEntity("CollisionSystem", id);

            ///Collision Scenarios
            ///
            /// 1. Collision with a Collectable (e.g., ring):
            /// - Handled by ItemCollectionSystem (this system only notes the collision)
            /// - Collector collides with Collectable
            ///
            /// 2. Collision with an Enemy:
            /// - If this entity has Health and the other has Damage: reduce health
            /// - If this entity has RollingTag: hurt the enemy instead
            /// - If this entity has TemporaryPowerup::Invincibility: ignore damage
            }
        }
    }

    /**
     * @brief Processes animation-related behavior for entities.
     *
     * This system iterates through all entities and applies animation logic to those that have the necessary components.
     * Necessary components: Animation.
     * Optional components: MovementAbility.
     */
    void AnimationSystem() {
        bagel::Mask required;
        required.set(bagel::Component<Animation>::Bit);

        for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
            bagel::ent_type ent{id};

            if (bagel::World::mask(ent).test(required)) {
                bool hasMovementAbility = bagel::World::mask(ent).test(bagel::Component<MovementAbility>::Bit);
                PrintSystemHandlingEntity("AnimationSystem", id);
            }
        }
    }

    /**
     * @brief Processes collection of items (like rings) by collectors (e.g., Sonic).
     *
     * This system loops through all entities that can collect items.
     * When a collector collides with a collectable, it handles.
     * and immediately destroys the collectable so it disappears from the game.
     * Required: CollectorTag, CollisionInfo
     * Optional: RingCount
     */
    void ItemCollectionSystem() {
        bagel::Mask required;
        required.set(bagel::Component<CollectorTag>::Bit);
        required.set(bagel::Component<RingCount>::Bit);
        required.set(bagel::Component<CollisionInfo>::Bit);

        for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
            bagel::ent_type ent{id};

            if (bagel::World::mask(ent).test(required)) {
                bool hasRingCount = bagel::World::mask(ent).test(bagel::Component<RingCount>::Bit);
                PrintSystemHandlingEntity("ItemCollectionSystem", id);
            }

            /// We check its CollisionInfo to see with each entity it collided with.
            /// If the collided entity has a CollectableTag (e.g., a ring, a super-power),
            /// we handle it here, for example: increase the collector’s RingCount / update the entity temporary powerup.
            /// Finally we destroy the collectable, so it disappears from the game.
        }
    }

    /**
     * @brief Processes temporary power up effects for entities.
     *
     * This system iterates through all entities and applies temporary power up logic to those that have the necessary components.
     * Necessary components: TemporaryPowerup.
     * Optional components: Velocity, MovementAbility.
     */
    void TemporaryEffectsSystem() {
        bagel::Mask required;
        required.set(bagel::Component<TemporaryPowerup>::Bit);

        for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
            bagel::ent_type ent{id};

            if (bagel::World::mask(ent).test(required)) {
                bool hasVelocity = bagel::World::mask(ent).test(bagel::Component<Velocity>::Bit);
                bool hasMovementAbility = bagel::World::mask(ent).test(bagel::Component<MovementAbility>::Bit);
                PrintSystemHandlingEntity("TemporaryEffectsSystem", id);
            }
        }
    }

    /**
     * @brief Reads raw input (e.g., key presses) and updates the Input component.
     *
     * Captures real player input and store it in an Input component.
     * Required: PlayerTag, Input
     */
    void InputSystem() {
        bagel::Mask required;
        required.set(bagel::Component<PlayerTag>::Bit);
        required.set(bagel::Component<Input>::Bit);

        for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
            bagel::ent_type ent{id};

            if (bagel::World::mask(ent).test(required)) {
                PrintSystemHandlingEntity("InputSystem", id);
                /// Read input and set Input component fields
            }
        }
    }

    /**
     * @brief Processes rendering of entities.
     *
     * This system renders entities that have a position.
     * Necessary components: Position.
     * Optional components: Animation.
     */
    void RenderSystem() {
        bagel::Mask required;
        required.set(bagel::Component<Position>::Bit);

        for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
            bagel::ent_type ent{id};

            if (bagel::World::mask(ent).test(required)) {
                bool hasAnimation = bagel::World::mask(ent).test(bagel::Component<Animation>::Bit);
                PrintSystemHandlingEntity("RenderSystem", id);
            }
        }
    }

    /**
     * @brief Translates raw input into player intent.
     *
     * This system reads the Input component and sets the player's Intent accordingly.
     * The actual action (e.g., jump or roll or regular running) is handled in a separate system.
     * Required: PlayerTag, Input, Intent
     */
    void IntentSystem() {
        bagel::Mask required;
        required.set(bagel::Component<PlayerTag>::Bit);
        required.set(bagel::Component<Input>::Bit);
        required.set(bagel::Component<Intent>::Bit);

        for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
            bagel::ent_type ent{id};

            if (bagel::World::mask(ent).test(required)) {
                PrintSystemHandlingEntity("IntentSystem", id);
            }
            /// Interpret input and set Intent.current
        }
    }

    /**
     * @brief Handles execution of player intentions such as running, jumping, or rolling.
     *
     * This system reacts to the current Intent state of entities and applies
     * game logic accordingly (e.g., running, jumping, rolling).
     * Required: Intent.
     * Optional: Velocity, JumpingTag, RollingTag.
     */
    void ActionSystem() {
        bagel::Mask required;
        required.set(bagel::Component<Intent>::Bit);

        for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
            bagel::ent_type ent{id};

            if (bagel::World::mask(ent).test(required)) {
                bool hasVelocity = bagel::World::mask(ent).test(bagel::Component<Velocity>::Bit);
                bool isJumping = bagel::World::mask(ent).test(bagel::Component<JumpingTag>::Bit);
                bool isRolling = bagel::World::mask(ent).test(bagel::Component<RollingTag>::Bit);
                PrintSystemHandlingEntity("ActionSystem", id);
            }
            /// Add logic of applying velocity, adding tags (JumpingTag, RollingTag), or checking constraints.
        }
    }

// === Entity Creation Implementations ===

    /**
     * @brief Prints when a new entity is created.
     *
     * Outputs the name of the created entity and its ID to the console.
     *
     * @param entityName The name/type of the entity.
     * @param id The ID assigned to the entity.
     */
    void PrintEntityCreated(const char* entityName, bagel::id_type id) {
        std::cout << "Created " << entityName << " Entity with ID: " << id << std::endl;
    }

    /**
     * @brief Creates a Sonic player entity.
     *
     * Initializes a Sonic entity with position, velocity, movement, animation, health,
     * player tags, ring collectorTag, ring count, collision, temporary power up, gravity tag,
     * intent, jumping tag and rolling tag components.
     *
     * @param pos_x The starting x-position of the entity.
     * @param pos_y The starting y-position of the entity.
     * @return The ID of the created Sonic entity.
     */
    bagel::id_type CreateSonicEntity(float pos_x, float pos_y) {
        bagel::Entity sonic = bagel::Entity::create();
        sonic.addAll(
                Position{pos_x, pos_y},
                Velocity{0.0f, 0.0f},
                MovementAbility{},
                Animation{eAnimationState::Idle},
                Health{3},
                PlayerTag{},
                CollectorTag{},
                RingCount{0},
                CollisionInfo{-1},
                TemporaryPowerup{ePowerupType::None, 0.0f},
                GravityTag{},
                Intent{},
                JumpingTag{},
                RollingTag{}
        );

        PrintEntityCreated("Sonic", sonic.entity().id);
        return sonic.entity().id;
    }

    /**
     * @brief Creates an enemy entity.
     *
     * Initializes an enemy entity with position, velocity, movement, animation, health, enemy tag, collision
     * damage, gravity tag and components.
     *
     * @param pos_x The starting x-position of the entity.
     * @param pos_y The starting y-position of the entity.
     * @return The ID of the created enemy entity.
     */
    bagel::id_type CreateEnemyEntity(float pos_x, float pos_y) {
        bagel::Entity enemy = bagel::Entity::create();
        enemy.addAll(
                Position{pos_x, pos_y},
                Velocity{0.0f, 0.0f},
                MovementAbility{},
                Animation{eAnimationState::Idle},
                Health{1},
                EnemyTag{},
                CollisionInfo{-1},
                DamageTag{},
                GravityTag{}
        );

        PrintEntityCreated("Enemy", enemy.entity().id);
        return enemy.entity().id;
    }

    /**
     * @brief Creates a ring entity.
     *
     * Initializes a ring entity with ringTag, position, animation, collision and collectable tag components.
     *
     * @param pos_x The starting x-position of the entity.
     * @param pos_y The starting y-position of the entity.
     * @return The ID of the created ring entity.
     */
    bagel::id_type CreateRingEntity(float pos_x, float pos_y) {
        bagel::Entity ring = bagel::Entity::create();
        ring.addAll(
                RingTag{},
                Position{pos_x, pos_y},
                Animation{eAnimationState::Idle},
                CollisionInfo{-1},
                CollectableTag{}
        );

        PrintEntityCreated("Ring", ring.entity().id);
        return ring.entity().id;
    }

    /**
     * @brief Creates an obstacle entity.
     *
     * Initializes an obstacle entity with position, collision and obstacle tag components.
     *
     * @param pos_x The starting x-position of the entity.
     * @param pos_y The starting y-position of the entity.
     * @return The ID of the created obstacle entity.
     */
    bagel::id_type CreateObstacleEntity(float pos_x, float pos_y) {
        bagel::Entity obstacle = bagel::Entity::create();
        obstacle.addAll(
                Position{pos_x, pos_y},
                CollisionInfo{-1},
                ObstacleTag{}
        );

        PrintEntityCreated("Obstacle", obstacle.entity().id);
        return obstacle.entity().id;
    }

    /**
     * @brief Creates a platform entity.
     *
     * Initializes a platform entity with position, collision and obstacle tag components.
     *
     * @param pos_x The starting x-position of the entity.
     * @param pos_y The starting y-position of the entity.
     * @return The ID of the created platform entity.
     */
    bagel::id_type CreatePlatformEntity(float pos_x, float pos_y) {
        bagel::Entity platform = bagel::Entity::create();
        platform.addAll(
                Position{pos_x, pos_y},
                CollisionInfo{-1},
                ObstacleTag{}
        );

        PrintEntityCreated("Platform", platform.entity().id);
        return platform.entity().id;
    }

    /**
     * @brief Creates a spikes entity.
     *
     * Initializes a spikes entity with position, collision, obstacle tag and damage component.
     *
     * @param pos_x The starting x-position of the entity.
     * @param pos_y The starting y-position of the entity.
     * @return The ID of the created spikes entity.
     */
    bagel::id_type CreateSpikesEntity(float pos_x, float pos_y) {
        bagel::Entity spikes = bagel::Entity::create();
        spikes.addAll(
                Position{pos_x, pos_y},
                CollisionInfo{-1},
                ObstacleTag{},
                DamageTag{}
        );

        PrintEntityCreated("Spikes", spikes.entity().id);
        return spikes.entity().id;
    }

    /**
     * @brief Creates a collectable powerup entity that grants a temporary effect to the player.
     *
     * This entity is placed in the world and marked as collectable. When the player collides
     * with it, the associated powerup (e.g., invincibility or speed boost) is granted via
     * the PowerupTypeComponent.
     *
     * @param x The X position of the powerup in the world.
     * @param y The Y position of the powerup in the world.
     * @return The ID of the created powerup entity.
     */
    bagel::id_type CreatePowerupEntity(float x, float y) {
        bagel::Entity e = bagel::Entity::create();
        e.addAll(
                Position{x, y},
                CollisionInfo{-1},
                CollectableTag{},
                PowerupTypeComponent{ePowerupType::Invincibility}
        );
        return e.entity().id;
    }

} // namespace SonicGame
