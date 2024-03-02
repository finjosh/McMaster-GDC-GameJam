#include "Player.hpp"

const b2Vec2 Player::_size = {50, 25};
sf::CircleShape Player::_shape;

Player::Player(const float& x, const float& y, const int& layer) : DrawableObject(layer)
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
        b2Vec2 forward = this->getBody()->GetWorldPoint({_size.x/2,0});
        forward -= this->getBody()->GetPosition();
        forward.Normalize();

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::W))){
            forward.x *= 8000 * deltaTime;
            forward.y *= 8000 * deltaTime;
            this->getBody()->ApplyForceToCenter(forward, true);
            _forwardBooster->emit();
        }

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::S))){
            forward.x *= -7500 * deltaTime;
            forward.y *= -7500 * deltaTime;
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
