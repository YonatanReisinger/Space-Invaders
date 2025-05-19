#include "SpaceInvaders.h"
#include <iostream>

namespace SpaceInvadersGame {

// === Systems Implementations ===

/**
 * @brief Moves entities according to their velocity.
 * Required: Position, Velocity
 */
void MovementSystem() {
    bagel::Mask required;
    required.set(bagel::Component<Position>::Bit);
    required.set(bagel::Component<Velocity>::Bit);
    for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
        bagel::ent_type ent{id};
        if (bagel::World::mask(ent).test(required)) {
            std::cout << "MovementSystem: Handling entity ID " << id << std::endl;
        }
    }
}

/**
 * @brief Draws all visible entities to the screen.
 * Required: Position, RenderData
 */
void RenderSystem() {
    bagel::Mask required;
    required.set(bagel::Component<Position>::Bit);
    required.set(bagel::Component<RenderData>::Bit);
    for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
        bagel::ent_type ent{id};
        if (bagel::World::mask(ent).test(required)) {
            std::cout << "RenderSystem: Handling entity ID " << id << std::endl;
        }
    }
}

/**
 * @brief Detects and handles collisions between entities.
 * Required: Position, Collider
 * Optional: Health, ScoreValue, Dead
 */
void CollisionSystem() {
    bagel::Mask required;
    required.set(bagel::Component<Position>::Bit);
    required.set(bagel::Component<Collider>::Bit);
    for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
        bagel::ent_type ent{id};
        if (bagel::World::mask(ent).test(required)) {
            std::cout << "CollisionSystem: Handling entity ID " << id << std::endl;
        }
    }
}

/**
 * @brief Handles player shooting logic.
 * Required: PlayerTag, Shoots, Position, WantsToShoot
 */
void PlayerShootingSystem() {
    bagel::Mask required;
    required.set(bagel::Component<PlayerTag>::Bit);
    required.set(bagel::Component<Shoots>::Bit);
    required.set(bagel::Component<Position>::Bit);
    required.set(bagel::Component<WantsToShoot>::Bit);
    for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
        bagel::ent_type ent{id};
        if (bagel::World::mask(ent).test(required)) {
            std::cout << "PlayerShootingSystem: Handling entity ID " << id << std::endl;
        }
    }
}

/**
 * @brief Handles enemy shooting logic.
 * Required: EnemyTag, Shoots, Position, WantsToShoot
 */
void EnemyShootingSystem() {
    bagel::Mask required;
    required.set(bagel::Component<EnemyTag>::Bit);
    required.set(bagel::Component<Shoots>::Bit);
    required.set(bagel::Component<Position>::Bit);
    required.set(bagel::Component<WantsToShoot>::Bit);
    for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
        bagel::ent_type ent{id};
        if (bagel::World::mask(ent).test(required)) {
            std::cout << "EnemyShootingSystem: Handling entity ID " << id << std::endl;
        }
    }
}

/**
 * @brief Reduces health when needed and marks entities as dead.
 * Required: Health
 * Optional: Dead
 */
void HealthSystem() {
    bagel::Mask required;
    required.set(bagel::Component<Health>::Bit);
    for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
        bagel::ent_type ent{id};
        if (bagel::World::mask(ent).test(required)) {
            std::cout << "HealthSystem: Handling entity ID " << id << std::endl;
        }
    }
}

/**
 * @brief Adds to the score when entities with ScoreValue are destroyed.
 * Required: ScoreValue, Dead
 */
void ScoreSystem() {
    bagel::Mask required;
    required.set(bagel::Component<ScoreValue>::Bit);
    required.set(bagel::Component<Dead>::Bit);
    for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
        bagel::ent_type ent{id};
        if (bagel::World::mask(ent).test(required)) {
            std::cout << "ScoreSystem: Handling entity ID " << id << std::endl;
        }
    }
}

/**
 * @brief Handles general enemy logic (movement, speed, shooting, etc).
 * Required: EnemyTag
 */
void EnemyLogicSystem() {
    bagel::Mask required;
    required.set(bagel::Component<EnemyTag>::Bit);
    for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
        bagel::ent_type ent{id};
        if (bagel::World::mask(ent).test(required)) {
            std::cout << "EnemyLogicSystem: Handling entity ID " << id << std::endl;
        }
    }
}

/**
 * @brief Translates player input (buttons) into an intention.
 * Required: PlayerTag, Input
 */
void PlayerIntentSystem() {
    bagel::Mask required;
    required.set(bagel::Component<PlayerTag>::Bit);
    required.set(bagel::Component<Input>::Bit);
    for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
        bagel::ent_type ent{id};
        if (bagel::World::mask(ent).test(required)) {
            std::cout << "PlayerIntentSystem: Handling entity ID " << id << std::endl;
        }
    }
}

/**
 * @brief Translates intention into actions (movement, shooting, etc).
 * Required: PlayerTag, Intention
 */
void PlayerActionSystem() {
    bagel::Mask required;
    required.set(bagel::Component<PlayerTag>::Bit);
    // Intention component to be added to the player entity for this system
    // required.set(bagel::Component<Intention>::Bit); // Uncomment when Intention is defined
    for (bagel::id_type id = 0; id <= bagel::World::maxId().id; ++id) {
        bagel::ent_type ent{id};
        // if (bagel::World::mask(ent).test(required)) { // Uncomment when Intention is defined
        //     std::cout << "PlayerActionSystem: Handling entity ID " << id << std::endl;
        // }
        std::cout << "PlayerActionSystem: (stub, add Intention check) Handling entity ID " << id << std::endl;
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
        Health{3},
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
        RenderData{1},
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
        ProjectileTag{}
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