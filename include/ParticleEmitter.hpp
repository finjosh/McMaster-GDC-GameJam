#ifndef PARTICLEEMITTER_H
#define PARTICLEEMITTER_H

#pragma once

#include "Utils/UpdateInterface.hpp"
#include "Utils/Graphics/DrawableObject.hpp"
#include "Utils/Object.hpp"

#include "SFML/Graphics/Shape.hpp"

// TODO add the ability for physics
// TODO implement more than just shapes
class ParticleEmitter : public Object, public UpdateInterface, public DrawableObject
{
public:
    /// @param drawable make sure that the given drawable is accessible over the lifetime of this object
    ParticleEmitter(sf::Shape* shape);

protected:
    virtual void Update(const float& deltaTime) override;
    virtual void Draw(sf::RenderWindow& window) override;

    class Particle
    {
    public:
        Particle() = default;
        Particle(const sf::Vector2f& pos, const sf::Vector2f& vel);
        void setPosition(const sf::Vector2f& position);
        sf::Vector2f getPosition() const;
        void setVelocity(const sf::Vector2f& velocity);
        sf::Vector2f getVelocity() const;
        void move(const sf::Vector2f& vec);
        void draw(sf::Shape* shape, sf::RenderWindow& window);

    protected:
        /// @brief updates position based on velocity
        void updatePosition(const float& deltaTime);

    private:
        sf::Vector2f _position = {0,0};
        sf::Vector2f _velocity = {0,0};
        float _rotation = 0;
    };

private:
    sf::Shape* _particleShape;

    /// @brief if the particles are spawning
    bool _spawning = false;
    /// @brief the spawn rate of particles if spawning
    float _spawnRate = 0.1f;
    /// @brief the amount of random in the spawn rate
    float _randomSpawnRate = 0.f;
    /// @brief the amount of partials spawned at once
    int _spawnAmount = 1;
    /// @brief 0-360 in degrees
    float _spread = 0.f;
    /// @brief how random the rotation will be
    float _randomRotation = 0.1f;
    /// @brief the forward direction of the ParticleEmitter
    sf::Vector2f _forward = {0,0};
    /// @brief the initial velocity of the particles
    float _velocity = 0.f;
    /// @brief the position of the emitter
    sf::Vector2f _position = {0,0};

    #define PIXELS_PER_METER 10
};

#endif
