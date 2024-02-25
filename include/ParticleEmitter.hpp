#ifndef PARTICLEEMITTER_H
#define PARTICLEEMITTER_H

#pragma once

#include <list>
#include <math.h>

#include "SFML/Graphics/Shape.hpp"

#include "Utils/UpdateInterface.hpp"
#include "Utils/Graphics/DrawableObject.hpp"
#include "Utils/Object.hpp"

// TODO add the ability for physics
// TODO implement more than just shapes
class ParticleEmitter : virtual public Object, public UpdateInterface, public DrawableObject
{
public:
    /// @param shape make sure that the given drawable is accessible over the lifetime of this object
    ParticleEmitter(sf::Shape* shape);
    /// @param shape make sure that the given drawable is accessible over the lifetime of this object
    /// @param pos the position of the emitter
    /// @param vel the initial velocity of the particles
    /// @param rot rotation starting from +x axis (degrees)
    /// @param spawnRate the spawn rate of particles if spawning
    /// @param lifetime the particle lifetime
    /// @param spawnAmount the amount of partials spawned at once
    /// @param fadeOutTime the time for a particle to fade out 
    /// @param spread the max angle of spread
    ParticleEmitter(sf::Shape* shape,
                    const sf::Vector2f& pos, const float& vel, const float& rot, const float& spawnRate, 
                    const float& lifetime, const int& spawnAmount, const float& fadeOutTime, const float& spread);

    /// @brief particles will still be emitted but not rendered
    /// @note remove the shape before deleting it
    void removeShape();
    /// @warning shape has to be accessible over the lifetime of this object
    void setShape(sf::Shape* shape);
    const sf::Shape* getShape() const;
    sf::Shape* getShape();

    /// @brief if the particles are spawning
    bool getSpawning() const;
    /// @brief if the particles are spawning
    void setSpawning(const bool& value = true);
    /// @brief the spawn rate of particles if spawning
    float getSpawnRate() const;
    /// @brief the spawn rate of particles if spawning
    void setSpawnRate(const float& value = 0.1f);
    /// @brief the amount of random in the spawn rate
    float getRandomSpawnRate() const;
    /// @brief the amount of random in the spawn rate
    void setRandomSpawnRate(const float& value = 0.f);
    /// @brief the amount of partials spawned at once
    int getSpawnAmount() const;
    /// @brief the amount of partials spawned at once
    void setSpawnAmount(const int& value = 1);
    /// @brief the greatest random spawn amount (subtracts from spawn amount) 1-(RAND_MAX)
    unsigned int getRandomSpawnAmount() const;
    /// @brief the greatest random spawn amount (subtracts from spawn amount) 1-(RAND_MAX)
    void setRandomSpawnAmount(const unsigned int& value = 1);
    /// @brief the max angle of spread
    float getSpread() const;
    /// @brief the max angle of spread
    void setSpread(const float& value = 0.f);
    /// @brief how random the rotation will be
    float getRandomRotation() const;
    /// @brief how random the rotation will be
    void setRandomRotation(const float& value = 0.1f);
    /// @brief the particle lifetime
    float getLifetime() const;
    /// @brief the particle lifetime
    void setLifetime(const float& value = 1.f);
    /// @brief the time for a particle to fade out
    float getFadeOutTime() const;
    /// @brief the time for a particle to fade out
    void setFadeOutTime(const float& value = 0.f);
    /// @brief the rotation starting from +x axis (degrees)
    float getRotation() const;
    /// @brief the rotation starting from +x axis (degrees)
    void setRotation(const float& value = 0.f);
    /// @brief the initial velocity of the particles
    float getVelocity() const;
    /// @brief the initial velocity of the particles
    void setVelocity(const float& value = 0.f);
    /// @brief the position of the emitter
    sf::Vector2f getPosition() const;
    /// @brief the position of the emitter
    void setPosition(const sf::Vector2f& value);
    /// @brief force emits the particles ignoring the spawn rate
    void emit();

    createDestroy();

protected:
    virtual void Update(const float& deltaTime) override;
    virtual void Draw(sf::RenderWindow& window) override;

    /// @returns a random float from 0 to the given x
    float randX(const float& x);

    class Particle
    {
    public:
        Particle() = default;
        Particle(const sf::Vector2f& pos, const sf::Vector2f& vel, const float& rotation);
        void setPosition(const sf::Vector2f& position);
        sf::Vector2f getPosition() const;
        void setVelocity(const sf::Vector2f& velocity);
        sf::Vector2f getVelocity() const;
        /// @param rot in degrees
        void setRotation(const float& rot);
        /// @returns rotation in degrees
        float getRotation() const;
        float getLifetime() const;
        int8_t getAlpha() const;
        void setAlpha(const int8_t& value = 255);

        void move(const sf::Vector2f& vec);
        void draw(sf::Shape* shape, sf::RenderWindow& window);

    protected:
        /// @brief updates position based on velocity
        void update(const float& deltaTime);

        friend ParticleEmitter;

    private:
        sf::Vector2f _position = {0,0};
        sf::Vector2f _velocity = {0,0};
        /// @brief in degrees
        float _rotation = 0.f;
        float _lifetime = 0.f;
        int8_t _alpha = 255;
    };

private:
    sf::Shape* _particleShape = nullptr;

    /// @brief if the particles are spawning
    bool _spawning = false;
    /// @brief the spawn rate of particles if spawning
    float _spawnRate = 0.1f;
    float _spawnTimer = 0.f;
    /// @brief the amount of random in the spawn rate
    float _randomSpawnRate = 0.f;
    /// @brief the amount of partials spawned at once
    int _spawnAmount = 1;
    /// @brief the greatest random spawn amount (subtracts from spawn amount)
    unsigned int _randomSpawnAmount = 1;
    /// @brief the max angle of spread
    float _spread = 0.f;
    /// @brief how random the rotation will be
    float _randomRotation = 0.1f;
    /// @brief the particle lifetime
    float _lifetime = 1.f;
    /// @brief the time for a particle to fade out
    float _fadeOutTime = 0.f;
    /// @brief rotation starting from +x axis (degrees)
    float _rotation = 0.f;
    /// @brief the initial velocity of the particles
    float _velocity = 0.f;
    /// @brief the position of the emitter
    sf::Vector2f _position = {0,0};

    std::list<Particle> _particles;

    #define PIXELS_PER_METER 10
};

#endif
