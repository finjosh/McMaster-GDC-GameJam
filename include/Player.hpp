#ifndef PLAYER_H
#define PLAYER_H

#pragma once

#include <math.h>

#include "SFML/Graphics.hpp"

#include "Utils/Physics/WorldHandler.hpp"
#include "Utils/Graphics/WindowHandler.hpp"
#include "Utils/GameObject.hpp"
#include "Utils/Physics/Collider.hpp"

class Player : public sf::RectangleShape, public GameObject, public Collider
{
public:
    Player(const float& x, const float& y, const int& layer = 0);
    ~Player();

    createDestroy();

protected:
    virtual void Update(const float& deltaTime) override;
    virtual void Draw(sf::RenderWindow& window) override;

    void BeginContact(CollisionData collisionData) override;
    void EndContact(CollisionData collisionData) override;

private:
    // TODO make a const for the cool downs and change these to timers
    float _shootCooldown = 0.f;

    static const b2Vec2 size;
};

#endif
