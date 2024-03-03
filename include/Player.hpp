#ifndef PLAYER_H
#define PLAYER_H

#pragma once

#include <math.h>

#include "SFML/Graphics.hpp"

#include "Utils/Physics/WorldHandler.hpp"
#include "Utils/Graphics/WindowHandler.hpp"
#include "Utils/GameObject.hpp"
#include "Utils/Physics/Collider.hpp"
#include "Utils/ParticleEmitter.hpp"

class Player : public sf::RectangleShape, public GameObject, public Collider
{
public:
    Player(const float& x, const float& y, const int& layer = 0);
    ~Player();

    createDestroy();

    // TODO make a breaking function
    void applyForward(const b2Vec2& forward);
    void applyBackward(const b2Vec2& forward);
    void applyLeftTurn();
    void applyRightTurn();
    void applyLeft(const b2Vec2& forward);
    void applyRight(const b2Vec2& forward);
    /// @brief attempts to shoot a missile
    void shoot();

protected:
    virtual void Update(const float& deltaTime) override;
    virtual void LateDraw(sf::RenderWindow& window) override;

    void BeginContact(CollisionData collisionData) override;
    void EndContact(CollisionData collisionData) override;

private:
    float _shootCooldown = 0.f;
    ParticleEmitter* _forwardBooster = nullptr;
    ParticleEmitter* _backwardBooster = nullptr;
    ParticleEmitter* _leftFBooster = nullptr;
    ParticleEmitter* _leftBBooster = nullptr;
    ParticleEmitter* _rightFBooster = nullptr;
    ParticleEmitter* _rightBBooster = nullptr;

    static const b2Vec2 _size;
    static sf::CircleShape _shape;
};

#endif
