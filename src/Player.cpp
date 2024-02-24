#include "Player.hpp"

const b2Vec2 Player::size = {50, 25};

Player::Player(const float& x, const float& y, const int& layer) : DrawableObject(layer)
{
    b2PolygonShape b2shape;
    b2shape.SetAsBox(size.x/PIXELS_PER_METER/2.0, size.y/PIXELS_PER_METER/2.0);

    Collider::initCollider(x/PIXELS_PER_METER,y/PIXELS_PER_METER);
    Collider::createFixture(b2shape, 1, 0.25);
    Collider::getBody()->SetLinearDamping(0.25);
    Collider::getBody()->SetAngularDamping(0.50);

    RectangleShape::setSize({size.x,size.y});
    RectangleShape::setOrigin(size.x/2,size.y/2);
    RectangleShape::setPosition(x,y);
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
    _shootCooldown += deltaTime;
    
    if (WindowHandler::getRenderWindow()->hasFocus())
    {
        b2Vec2 forward = this->getBody()->GetWorldPoint({-size.x/2,0});
        forward.x -= this->getBody()->GetPosition().x;
        forward.y -= this->getBody()->GetPosition().y;
        forward.Normalize();

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::W))){
            forward.x *= -7500 * deltaTime;
            forward.y *= -7500 * deltaTime;
            this->getBody()->ApplyForceToCenter(forward, true);
        }

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::S))){
            forward.x *= 7500 * deltaTime;
            forward.y *= 7500 * deltaTime;
            this->getBody()->ApplyForceToCenter(forward, true);
        }

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::D))){
            this->getBody()->ApplyForce({-100*deltaTime,0}, {0,size.y/2}, true);
            this->getBody()->ApplyForce({100*deltaTime,0}, {0,-size.y/2}, true);
        }

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::A))){
            this->getBody()->ApplyForce({100*deltaTime,0}, {0,size.y/2}, true);
            this->getBody()->ApplyForce({-100*deltaTime,0}, {0,-size.y/2}, true);
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
