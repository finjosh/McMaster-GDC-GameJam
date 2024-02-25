#include "Player.hpp"

const b2Vec2 Player::_size = {50, 25};
sf::CircleShape Player::_shape;

Player::Player(const float& x, const float& y, const int& layer) : DrawableObject(layer)
{
    _shape.setFillColor(sf::Color::Red);
    _shape.setRadius(5);

    b2PolygonShape b2shape;
    b2shape.SetAsBox(_size.x/PIXELS_PER_METER/2.0, _size.y/PIXELS_PER_METER/2.0);

    Collider::initCollider(x/PIXELS_PER_METER,y/PIXELS_PER_METER);
    Collider::createFixture(b2shape, 1, 0.25);
    Collider::getBody()->SetLinearDamping(0.25);
    Collider::getBody()->SetAngularDamping(0.50);

    RectangleShape::setSize({_size.x,_size.y});
    RectangleShape::setOrigin(_size.x/2,_size.y/2);
    RectangleShape::setPosition(x,y);

    _forwardBooster = new ParticleEmitter(&_shape, {0,0}, 25, -90, 0.1, 0.25, 5, 0.1, 25);
    _backwardBooster = new ParticleEmitter(&_shape, {0,0}, 25, 90, 0.1, 0.25, 5, 0.1, 25);
    _leftFBooster = new ParticleEmitter(&_shape, {0,0}, 25, 0, 0.1, 0.25, 5, 0.1, 25);
    _leftBBooster = new ParticleEmitter(&_shape, {0,0}, 25, 180, 0.1, 0.25, 5, 0.1, 25);
    _rightFBooster = new ParticleEmitter(&_shape, {0,0}, 25, 180, 0.1, 0.25, 5, 0.1, 25);
    _rightBBooster = new ParticleEmitter(&_shape, {0,0}, 25, 0, 0.1, 0.25, 5, 0.1, 25);
}

Player::~Player()
{

}

void Player::Draw(sf::RenderWindow& window)
{
    RectangleShape::setPosition({getBody()->GetPosition().x*PIXELS_PER_METER, this->getBody()->GetPosition().y*PIXELS_PER_METER});
    RectangleShape::setRotation(this->getBody()->GetAngle()*180/b2_pi);
    window.draw(*this);
}

void Player::Update(const float& deltaTime)
{
    sf::Vector2f pos = {getBody()->GetPosition().x, getBody()->GetPosition().y};
    _forwardBooster->setPosition({pos.x, pos.y + _size.y/PIXELS_PER_METER/2});
    _forwardBooster->setRotation(getBody()->GetAngle() / b2_pi * 180 - 180);
    _backwardBooster->setPosition({pos.x, pos.y - _size.y/PIXELS_PER_METER/2});
    _backwardBooster->setRotation(getBody()->GetAngle() / b2_pi * 180);
    _leftFBooster->setPosition(pos);
    _leftFBooster->setRotation(getBody()->GetAngle() / b2_pi * 180);
    _leftBBooster->setPosition(pos);
    _leftBBooster->setRotation(getBody()->GetAngle() / b2_pi * 180 + 90);
    _rightFBooster->setPosition(pos);
    _rightFBooster->setRotation(getBody()->GetAngle() / b2_pi * 180 + 90);
    _rightBBooster->setPosition(pos);
    _rightBBooster->setRotation(getBody()->GetAngle() / b2_pi * 180);

    _shootCooldown += deltaTime;
    
    if (WindowHandler::getRenderWindow()->hasFocus())
    {
        b2Vec2 forward = this->getBody()->GetWorldPoint({-_size.x/2,0});
        forward.x -= this->getBody()->GetPosition().x;
        forward.y -= this->getBody()->GetPosition().y;
        forward.Normalize();

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::W))){
            forward.x *= -7500 * deltaTime;
            forward.y *= -7500 * deltaTime;
            this->getBody()->ApplyForceToCenter(forward, true);
            _forwardBooster->emit();
        }

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::S))){
            forward.x *= 7500 * deltaTime;
            forward.y *= 7500 * deltaTime;
            this->getBody()->ApplyForceToCenter(forward, true);
            _backwardBooster->emit();
        }

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::D))){
            this->getBody()->ApplyForce({-100*deltaTime,0}, {0,_size.y/2}, true);
            this->getBody()->ApplyForce({100*deltaTime,0}, {0,-_size.y/2}, true);
            _rightBBooster->emit();
            _rightFBooster->emit();
        }

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::A))){
            this->getBody()->ApplyForce({100*deltaTime,0}, {0,_size.y/2}, true);
            this->getBody()->ApplyForce({-100*deltaTime,0}, {0,-_size.y/2}, true);
            _leftBBooster->emit();
            _leftFBooster->emit();
        }

        // if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        // {
        //     // _state.shoot = true;
        // }
    }
}

void Player::BeginContact(CollisionData collisionData) 
{

}

void Player::EndContact(CollisionData collisionData) 
{

}
