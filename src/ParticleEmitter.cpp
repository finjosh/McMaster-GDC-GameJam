#include "ParticleEmitter.hpp"

ParticleEmitter::Particle::Particle(const sf::Vector2f& pos, const sf::Vector2f& vel) : _position(pos), _velocity(vel) {}

void ParticleEmitter::Particle::setPosition(const sf::Vector2f& position)
{
    _position = position;
}

sf::Vector2f ParticleEmitter::Particle::getPosition() const
{
    return _position;
}

void ParticleEmitter::Particle::setVelocity(const sf::Vector2f& velocity)
{
    _velocity = velocity;
}

sf::Vector2f ParticleEmitter::Particle::getVelocity() const
{
    return _velocity;
}

void ParticleEmitter::Particle::move(const sf::Vector2f& vec)
{
    _position.x += vec.x;
    _position.y += vec.y;
}

void ParticleEmitter::Particle::draw(sf::Shape* shape, sf::RenderWindow& window)
{
    shape->setPosition(_position);
    shape->setRotation(_rotation);
    window.draw(*shape);
}

void ParticleEmitter::Particle::updatePosition(const float& deltaTime)
{
    _position.x += _velocity.x * deltaTime;
    _position.y += _velocity.y * deltaTime;
}

ParticleEmitter::ParticleEmitter(sf::Shape* shape)
{

}

void ParticleEmitter::Update(const float& deltaTime)
{

}

void ParticleEmitter::Draw(sf::RenderWindow& window)
{

}
