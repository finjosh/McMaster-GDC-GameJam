#include "Player.hpp"
#include "Missile.hpp"

const b2Vec2 Player::_size = {50, 25};
sf::CircleShape Player::_shape;

Player::Player(const float& x, const float& y, const int& layer) : DrawableObject(layer)
{
    _shape.setFillColor(sf::Color::Red);
    _shape.setRadius(5);
    _shape.setOrigin(2.5,2.5);

    RectangleShape::setSize({_size.x,_size.y});
    RectangleShape::setOrigin(_size.x/2,_size.y/2);
    RectangleShape::setPosition(x,y);

    _forwardBooster = new ParticleEmitter(&_shape, {-2.5,0}, 35, b2_pi, 0.1, 0.15, 5, 0.1, 25, {0,0,100,0});
    _backwardBooster = new ParticleEmitter(&_shape, {2.5,0}, 35, 0, 0.1, 0.15, 5, 0.1, 25, {0,0,100,0});
    _leftFBooster = new ParticleEmitter(&_shape, {2,1.25}, 35, b2_pi/2, 0.1, 0.15, 5, 0.1, 25, {0,0,100,0});
    _leftBBooster = new ParticleEmitter(&_shape, {-2,-1.25}, 35, -b2_pi/2, 0.1, 0.15, 5, 0.1, 25, {0,0,100,0});
    _rightFBooster = new ParticleEmitter(&_shape, {2,-1.25}, 35, -b2_pi/2, 0.1, 0.15, 5, 0.1, 25, {0,0,100,0});
    _rightBBooster = new ParticleEmitter(&_shape, {-2,1.25}, 35, b2_pi/2, 0.1, 0.15, 5, 0.1, 25, {0,0,100,0});
    _forwardBooster->setParent(this);
    _backwardBooster->setParent(this);
    _leftFBooster->setParent(this);
    _leftBBooster->setParent(this);
    _rightFBooster->setParent(this);
    _rightBBooster->setParent(this);

    b2PolygonShape b2shape;
    b2shape.SetAsBox(_size.x/PIXELS_PER_METER/2.0, _size.y/PIXELS_PER_METER/2.0);

    Collider::initCollider(x,y);
    Collider::createFixture(b2shape, 1, 0.25);
    Collider::getBody()->SetLinearDamping(0.25);
    Collider::getBody()->SetAngularDamping(0.50);
}

Player::~Player()
{

}

void Player::LateDraw(sf::RenderWindow& window)
{
    RectangleShape::setPosition({Object::getPosition().x*PIXELS_PER_METER, Object::getPosition().y*PIXELS_PER_METER});
    RectangleShape::setRotation(Object::getRotation()*180/b2_pi);
    window.draw(*this);
}

void Player::Update(const float& deltaTime)
{
    _shootCooldown += deltaTime;
    
    if (WindowHandler::getRenderWindow()->hasFocus())
    {
        b2Vec2 forward = this->getBody()->GetWorldPoint({_size.x/2,0}) - Object::getPosition();
        forward.Normalize();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
            applyForward(forward);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
            applyBackward(forward);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
            applyRightTurn();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
            applyLeftTurn();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        {
            applyLeft(forward);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        {
            applyRight(forward);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            shoot();
        }
    }
}

void Player::applyForward(const b2Vec2& forward)
{
    this->getBody()->ApplyForceToCenter((100) * forward, true);
    _forwardBooster->emit();
}

void Player::applyBackward(const b2Vec2& forward)
{
    this->getBody()->ApplyForceToCenter((-100) * forward, true);
    _backwardBooster->emit();
}

void Player::applyLeftTurn()
{
    this->getBody()->ApplyForce({0,2}, {-_size.x/2,0}, true);
    this->getBody()->ApplyForce({0,-2}, {_size.x/2,0}, true);
    _leftBBooster->emit();
    _leftFBooster->emit();
}

void Player::applyRightTurn()
{
    this->getBody()->ApplyForce({0,-2}, {-_size.x/2,0}, true);
    this->getBody()->ApplyForce({0,2}, {_size.x/2,0}, true);
    _rightBBooster->emit();
    _rightFBooster->emit();
}

void Player::applyLeft(const b2Vec2& forward)
{
    b2Vec2 temp(forward.y, -forward.x);
    this->getBody()->ApplyForceToCenter((80) * temp, true);
    _leftFBooster->emit();
    _rightBBooster->emit();
}

void Player::applyRight(const b2Vec2& forward)
{
    b2Vec2 temp(forward.y, -forward.x);
    this->getBody()->ApplyForceToCenter((-80) * temp, true);
    _leftBBooster->emit();
    _rightFBooster->emit();
}

void Player::shoot()
{
    if (_shootCooldown >= 1)
    {
        _shootCooldown = 0.f;
        new Missile(Object::getPosition(), Object::getRotation_b2(), this->getBody()->GetLinearVelocity(), 50.f);
    }
}

void Player::BeginContact(CollisionData collisionData) 
{

}

void Player::EndContact(CollisionData collisionData) 
{

}
