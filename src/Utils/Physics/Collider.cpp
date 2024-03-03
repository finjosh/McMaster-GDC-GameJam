#include "Utils/Physics/Collider.hpp"
#include "Utils/Physics/CollisionManager.hpp"

Collider::Collider()
{
    Object::_onDisabled(&Collider::updatePhysicsState, this);
    Object::_onEnabled(&Collider::updatePhysicsState, this);
    // Object::_onParentRemoved(&CollisionManager::addCollider, this);
    // Object::_onParentSet(&CollisionManager::removeCollider, this);

    CollisionManager::addCollider(this);
}

Collider::~Collider()
{
    if (_body != nullptr)
        WorldHandler::getWorld().DestroyBody(_body);

    CollisionManager::removeCollider(this);
}

b2Body* Collider::operator->()
{
    return _body;
}

const b2Body* Collider::operator->() const
{
    return _body;
}

b2Body* Collider::operator*()
{
    return _body;
}

const b2Body* Collider::operator*() const
{
    return _body;
}

b2Body* Collider::getBody()
{
    return _body;
}

const b2Body* Collider::getBody() const
{
    return _body;
}

b2Fixture* Collider::createFixture(const b2FixtureDef& fixture)
{
    return _body->CreateFixture(&fixture);
}

b2Fixture* Collider::createFixture(const b2Shape& shape, const float& density, const float& friction, 
                                    const float& restitution, const float& restitutionThreshold, const b2Filter& filter)
{
    b2FixtureDef def;
    def.density = density;
    def.friction = friction;
    def.restitution = restitution;
    def.restitutionThreshold = restitutionThreshold;
    def.filter = filter;
    def.shape = &shape;
    return _body->CreateFixture(&def);
}

b2Fixture* Collider::createFixtureSensor(const b2Shape& shape, const float& density, const b2Filter& filter)
{
    b2FixtureDef def;
    def.isSensor = true;
    def.density = density;
    def.filter = filter;
    def.shape = &shape;
    return _body->CreateFixture(&def);
}

void Collider::initCollider(const b2Vec2& pos, const b2Rot& rot)
{
    this->initCollider(pos.x, pos.y, rot.GetAngle());
}

void Collider::initCollider(const float& x, const float& y, const float& rot)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x,y);
    bodyDef.angle = rot;
    this->initCollider(bodyDef);
}

void Collider::initCollider(const b2BodyDef& bodyDef)
{
    _body = WorldHandler::getWorld().CreateBody(&bodyDef);
    _body->GetUserData().pointer = (uintptr_t)this;
}

void Collider::setPhysicsEnabled(const bool& enabled)
{
    _enabled = enabled;

    if (_body == nullptr)
        return;

    _body->SetEnabled(_enabled && Object::isEnabled());
}

bool Collider::isPhysicsEnabled() const
{
    return _enabled && Object::isEnabled();
}

void Collider::updatePhysicsState()
{
    _body->SetEnabled(_enabled && Object::isEnabled());
}

void Collider::_updatePosition()
{
    Object::setTransform(_body->GetTransform());
}

bool Collider::canSetTransform() const
{
    return false;
}

//* Collision Data

CollisionData::CollisionData(Collider* collider, b2Fixture* thisFixture, b2Fixture* otherFixture) : _collider(collider), _thisFixture(thisFixture), _otherFixture(otherFixture) {}

Collider* CollisionData::getCollider()
{
    return _collider;
}

b2Fixture* CollisionData::getFixtureA()
{
    return _thisFixture;
}

b2Fixture* CollisionData::getFixtureB()
{
    return _otherFixture;
}
