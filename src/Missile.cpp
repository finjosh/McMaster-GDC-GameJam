#include "Missile.hpp"
#include "Enemy.hpp"

const b2Vec2 Missile::_size = {25,15};
sf::CircleShape Missile::_shape;

Missile::Missile(const b2Vec2& position, const b2Rot& rotation, const b2Vec2& startingVel, const float& appliedForce) : _force(appliedForce)
{
    _shape.setFillColor(sf::Color::Red);
    _shape.setRadius(4);
    _shape.setOrigin(2,2);

    RectangleShape::setSize({_size.x,_size.y});
    RectangleShape::setOrigin(_size.x/2,_size.y/2);
    RectangleShape::setFillColor({128,128,128,255});

    //! NOTE this position is hard coded make sure to update when updating size
    _booster = new ParticleEmitter(&_shape, {-1.25,0}, 20, PI, 0.01, 0.15, 2, 0.3, 35, {0,0,100,0});
    _booster->setParent(this);
    _booster->setSpawning();

    b2PolygonShape b2shape;
    b2shape.SetAsBox(_size.x/PIXELS_PER_METER/2.0, _size.y/PIXELS_PER_METER/2.0);

    Collider::initCollider(position,rotation);
    Collider::createFixtureSensor(b2shape, 1);
    Collider::getBody()->SetLinearDamping(0.25);
    Collider::getBody()->SetAngularDamping(0.50);
    Collider::getBody()->SetBullet(true);
    Collider::getBody()->SetLinearVelocity(startingVel);
}

void Missile::Update(const float& deltaTime) 
{
    // TODO dont hard code this
    // destroy the missile if it goes to far off screen
    if (Object::getPosition().x < -20 || Object::getPosition().x > 212 || Object::getPosition().y < -20 || Object::getPosition().y > 128)
    {
        this->destroy();
        return;
    }

    b2Vec2 forward = this->getBody()->GetWorldPoint({_size.x/2,0}) - Object::getPosition();
    forward.Normalize();
    this->getBody()->ApplyForceToCenter(_force * forward, true);
}

void Missile::Draw(sf::RenderWindow& window)
{
    RectangleShape::setPosition({Object::getPosition().x*PIXELS_PER_METER, Object::getPosition().y*PIXELS_PER_METER});
    RectangleShape::setRotation(Object::getRotation()*180/PI);
    window.draw(*this);
}

void Missile::BeginContact(CollisionData collisionData)
{
    if (Enemy* enemy = collisionData.getCollider()->cast<Enemy>())
    {
        enemy->destroy();
        this->destroy();
    }
}
