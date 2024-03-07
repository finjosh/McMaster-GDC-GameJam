#include "Missile.hpp"
#include "Enemy.hpp"

const b2Vec2 Missile::_size = {25,15};
sf::CircleShape Missile::_shape;

Missile::Missile(const b2Vec2& position, const b2Rot& rotation, const b2Vec2& startingVel, const float& appliedForce) : _force(appliedForce)
{
    _startPos = position;

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
    if (_explosionTime > 0.f)
    {
        if (_stopPhysics) // so we can have one frame of the explosion physics
            this->setPhysicsEnabled(false);
        else
            _stopPhysics = true;

        _explosionTime += deltaTime;
        if (_explosionTime >= 3)
            this->destroy();
        else if (_explosionTime >= 1)
            _booster->setSpawning(false);
    }
    // destroy the missile if it travels to far
    else if ((_startPos - Object::getPosition()).LengthSquared() >= 1000000)
    {
        explode();
        return;
    }

    b2Vec2 forward = this->getBody()->GetWorldPoint({_size.x/2,0}) - Object::getPosition();
    forward.Normalize();
    this->getBody()->ApplyForceToCenter(_force * forward, true);
}

void Missile::Draw(sf::RenderWindow& window)
{
    if (_explosionTime == 0.f)
    {
        RectangleShape::setPosition({Object::getPosition().x*PIXELS_PER_METER, Object::getPosition().y*PIXELS_PER_METER});
        RectangleShape::setRotation(Object::getRotation()*180/PI);
        window.draw(*this);
    }
}

void Missile::explode()
{
    _booster->clear();
    _booster->setSpawnRate(0.1);
    _booster->setSpawnAmount(20);
    _booster->setFadeOutTime(1);
    _booster->setLifetime(2);
    _booster->setLocalPosition({0,0});
    _booster->setSpread(360);
    _booster->setVelocity(8);
    _explosionTime += 0.01;

    this->getBody()->SetLinearVelocity({0,0});
    b2CircleShape temp;
    temp.m_radius = 5;
    this->createFixtureSensor(temp, 1);
}

void Missile::BeginContact(CollisionData collisionData)
{
    if (Enemy* enemy = collisionData.getCollider()->cast<Enemy>())
    {
        enemy->destroy();
        if (_explosionTime == 0.f) 
        {
            explode();
        }
    }
}
