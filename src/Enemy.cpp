#include "Enemy.hpp"

const b2Vec2 Enemy::_size = {50, 25};
sf::CircleShape Enemy::_shape;

Enemy::Enemy(const float& x, const float& y, Object::Ptr<> target) : DrawableObject(0), _target(target.get())
{
    _shape.setFillColor(sf::Color::Red);
    _shape.setRadius(5);
    _shape.setOrigin(2.5,2.5);

    b2PolygonShape b2shape;
    b2shape.SetAsBox(_size.x/PIXELS_PER_METER/2.0, _size.y/PIXELS_PER_METER/2.0);

    Collider::initCollider(x,y);
    Collider::createFixture(b2shape, 1, 0.25);
    Collider::getBody()->SetLinearDamping(0.25);
    Collider::getBody()->SetAngularDamping(0.50);

    RectangleShape::setSize({_size.x,_size.y});
    RectangleShape::setOrigin(_size.x/2,_size.y/2);
    RectangleShape::setPosition(x,y);
    RectangleShape::setFillColor({100,50,50,255});

    _forwardBooster = new ParticleEmitter(&_shape, {-2.5,0}, 35, b2_pi, 0.1, 0.15, 2, 0.2, 35, {0,0,100,0});
    _backwardBooster = new ParticleEmitter(&_shape, {2.5,0}, 35, 0, 0.1, 0.15, 2, 0.2, 35, {0,0,100,0});
    _leftFBooster = new ParticleEmitter(&_shape, {2,1.25}, 35, b2_pi/2, 0.1, 0.15, 2, 0.2, 35, {0,0,100,0});
    _leftBBooster = new ParticleEmitter(&_shape, {-2,-1.25}, 35, -b2_pi/2, 0.1, 0.15, 2, 0.2, 35, {0,0,100,0});
    _rightFBooster = new ParticleEmitter(&_shape, {2,-1.25}, 35, -b2_pi/2, 0.1, 0.15, 2, 0.2, 35, {0,0,100,0});
    _rightBBooster = new ParticleEmitter(&_shape, {-2,1.25}, 35, b2_pi/2, 0.1, 0.15, 2, 0.2, 35, {0,0,100,0});
    _forwardBooster->setParent(this);
    _backwardBooster->setParent(this);
    _leftFBooster->setParent(this);
    _leftBBooster->setParent(this);
    _rightFBooster->setParent(this);
    _rightBBooster->setParent(this);
}

Enemy::~Enemy()
{

}

void Enemy::LateDraw(sf::RenderWindow& window)
{
    RectangleShape::setPosition({Object::getPosition().x*PIXELS_PER_METER, Object::getPosition().y*PIXELS_PER_METER});
    RectangleShape::setRotation(Object::getRotation()*180/b2_pi);
    window.draw(*this);
}

void Enemy::Update(const float& deltaTime)
{
    _shootCooldown += deltaTime;
    
    if (WindowHandler::getRenderWindow()->hasFocus())
    {
        b2Vec2 forward = this->getBody()->GetWorldPoint({_size.x/2,0}) - Object::getPosition();
        forward.Normalize();

        if (_target)
        {
            // move toward target
            b2Vec2 move = _target->getPosition() - Object::getPosition();
            float angle = std::atan2(move.y, move.x) - Object::getRotation();
            angle = abs(angle) > PI ? -angle : angle;
            if (angle < -0.1)
            {
                applyLeftTurn();
            }
            else if (angle > 0.1)
            {
                applyRightTurn();
            }
            if (abs(angle) < 0.6)
            {
                applyForward(forward);
            }
            else if (this->getBody()->GetLinearVelocity().LengthSquared() >= 100)
            {
                applyBackward(forward);
            }
        }
        else
        {
            // stop
        }
    }
}

void Enemy::applyForward(const b2Vec2& forward)
{
    this->getBody()->ApplyForceToCenter((95) * forward, true);
    _forwardBooster->emit();
}

void Enemy::applyBackward(const b2Vec2& forward)
{
    this->getBody()->ApplyForceToCenter((-95) * forward, true);
    _backwardBooster->emit();
}

void Enemy::applyLeftTurn()
{
    this->getBody()->ApplyForce({0,2}, {-_size.x/2,0}, true);
    this->getBody()->ApplyForce({0,-2}, {_size.x/2,0}, true);
    _leftBBooster->emit();
    _leftFBooster->emit();
}

void Enemy::applyRightTurn()
{
    this->getBody()->ApplyForce({0,-2}, {-_size.x/2,0}, true);
    this->getBody()->ApplyForce({0,2}, {_size.x/2,0}, true);
    _rightBBooster->emit();
    _rightFBooster->emit();
}

void Enemy::BeginContact(CollisionData collisionData) 
{

}

void Enemy::EndContact(CollisionData collisionData) 
{

}
