#ifndef MISSILE_H
#define MISSILE_H

#pragma once

#include "Utils/GameObject.hpp"
#include "Utils/Physics/Collider.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "Utils/ParticleEmitter.hpp"
#include "SFML/Graphics/CircleShape.hpp"

// TODO make this only hit an enemy on the front of the missile
class Missile : public sf::RectangleShape, public GameObject, public Collider
{
public:
    /// @param appliedForce the force that will be applied to the missile over its lifetime
    Missile(const b2Vec2& position, const b2Rot& rotation, const b2Vec2& startingVel, const float& appliedForce);

    virtual void Update(const float& deltaTime) override;
    virtual void Draw(sf::RenderWindow& window) override;

    void explode();

    createDestroy();

protected:
    void BeginContact(CollisionData collisionData) override;

private:
    float _force = 0.f;
    float _explosionTime = 0.f;
    bool _stopPhysics = false;
    ParticleEmitter* _booster = nullptr;
    b2Vec2 _startPos;

    static const b2Vec2 _size;
    static sf::CircleShape _shape;
};

#endif
