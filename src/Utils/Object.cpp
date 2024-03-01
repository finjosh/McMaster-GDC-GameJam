#include "Utils/Object.hpp"
#include "Utils/ObjectManager.hpp"

bool _objectComp::operator()(const Object* lhs, const Object* rhs) const
{
    return lhs->getID() < rhs->getID();
}

std::atomic_ullong Object::_lastID = 1;

Object::Ptr::Ptr(Object* obj)
{
    this->setObject(obj);
}

Object::Ptr::~Ptr()
{
    if (isValid())
    {
        _ptr->_onDestroy.disconnect(_eventID);
    }
}

Object* Object::Ptr::operator->()
{
    return _ptr;
}

const Object* Object::Ptr::operator->() const
{
    return _ptr;
}

Object* Object::Ptr::operator*()
{
    return _ptr;
}

const Object* Object::Ptr::operator*() const
{
    return _ptr;
}

Object::Ptr& Object::Ptr::operator=(const Object::Ptr& objectPtr)
{
    this->setObject(objectPtr._ptr);
    return *this;
}

bool Object::Ptr::operator==(const Object::Ptr& objectPtr) const
{
    return this->getID() == objectPtr.getID();
}

bool Object::Ptr::operator!=(const Object::Ptr& objectPtr) const
{
    return this->getID() != objectPtr.getID();
}

bool Object::Ptr::operator<(const Object::Ptr& objectPtr) const
{
    return this->getID() < objectPtr.getID();
}

bool Object::Ptr::operator>(const Object::Ptr& objectPtr) const
{
    return this->getID() > objectPtr.getID();
}

bool Object::Ptr::operator==(Object* object) const
{
    return *this == Object::Ptr(object);
}

bool Object::Ptr::operator!=(Object* object) const
{
    return *this != Object::Ptr(object);
}

bool Object::Ptr::operator<(Object* object) const
{
    return *this < Object::Ptr(object);
}

bool Object::Ptr::operator>(Object* object) const
{
    return *this > Object::Ptr(object);
}

Object* Object::Ptr::get()
{
    return _ptr;
}

const Object* Object::Ptr::get() const
{
    return _ptr;
}

bool Object::Ptr::isValid() const
{
    return (_ptr != nullptr);
}

void Object::Ptr::setObject(Object* obj)
{
    if (this->isValid())
    {
        _ptr->_onDestroy.disconnect(_eventID);
        _ptr = nullptr;
    }

    _ptr = obj;
    if (_ptr != nullptr)
        _eventID = _ptr->_onDestroy(Object::Ptr::removePtr, this);
}

void Object::Ptr::removePtr()
{
    _ptr = nullptr;
    _eventID = 0;
}

size_t Object::Ptr::getID() const
{
    return (this->_ptr == nullptr ? 0 : this->_ptr->getID());
}

Object::Object()
{
    _id = _lastID++; //! could become an issue if lots of creation and deletion happens
    ObjectManager::addObject(this);
}

Object::Object(unsigned long long id) : _id(id) {}

void Object::setID(unsigned long long id)
{
    _id = id;
}

Object::~Object()
{
    // this is not a real object
    if (_id == 0)
        return;

    ObjectManager::removeObject(this);
    
    if (_parent.isValid())
    {
        _parent->_removeChild(this);
    }
    
    for (auto child: _children)
    {
        child->destroy();
    }

    _onDestroy.invoke();
    onDestroy.invoke();
}

void Object::setEnabled(bool enabled)
{
    _enabled = enabled;
    if (_enabled)
    {
        _onEnabled.invoke();
        onEnabled.invoke();
    }
    else
    {
        _onDisabled.invoke();
        onDisabled.invoke();
    }
}

bool Object::isEnabled() const
{
    return _enabled;
}

unsigned long int Object::getID() const
{
    return _id;
}

Object::Ptr Object::getPtr()
{
    return Object::Ptr(this);
}

void Object::setParent(Object::Ptr parent)
{
    if (parent == this)
        return;

    if (_parent.isValid())
    {
        _parent->_removeChild(this);
    }

    _parent = parent;
    // if not valid have no parent
    if (parent.isValid())
    {
        parent->_addChild(this);
        _onParentSet.invoke();
        onParentSet.invoke();
    }
    else
    {
        _onParentRemoved.invoke();
        onParentRemoved.invoke();
    }
}

Object::Ptr Object::getParent()
{
    return _parent;
}

b2Vec2 Object::getLocalVector(const b2Vec2& vec) const
{
    return b2MulT(_transform.q, vec);
}

b2Vec2 Object::getGlobalVector(const b2Vec2& vec) const
{
    return b2Mul(_transform.q, vec);
}

void Object::rotateAround(const b2Vec2& center, const float& rot)
{
    b2Vec2 posChange = rotateAround(_transform.p, center, rot) - _transform.p;
    _transform.p += posChange;

    for (auto child: _children)
    {
        child->move(posChange);
        child->rotateAround(_transform.p, rot);
    }
}

b2Vec2 Object::rotateAround(const b2Vec2& vec, const b2Vec2& center, const float& rot)
{
    auto polar = std::polar<float>(1.0, rot);
    std::complex<float> temp(vec.x - center.x, vec.y - center.y);
    temp *= polar;
    return {temp.real() + center.x, temp.imag() + center.y};
}

void Object::setPosition(const b2Vec2& position)
{   
    b2Vec2 posChange = position - _transform.p;

    for (auto child: _children)
    {
        child->move(posChange);
    }

    _transform.p = position;
}

b2Vec2 Object::getPosition() const
{   
    return _transform.p;
}

void Object::setRotation(const float& rotation)
{
    float rotChange = rotation - _transform.q.GetAngle();

    for (auto child: _children)
    {
        child->rotateAround(_transform.p, rotChange);
        child->rotate(rotChange);
    }

    _transform.q.Set(rotation);
}

float Object::getRotation() const
{
    return _transform.q.GetAngle();
}

void Object::setTransform(const b2Transform& transform)
{
    b2Vec2 posChange = transform.p - _transform.p;
    float rotChange = transform.q.GetAngle() - _transform.q.GetAngle();
    
    for (auto child: _children)
    {
        if (child->getPosition() != this->getPosition()) // TODO check if this is needed
            child->rotateAround(_transform.p, rotChange);
        child->move(posChange);
        child->rotate(rotChange);
    }

    _transform = transform;
}

b2Transform Object::getTransform() const
{
    return _transform;
}

bool Object::canSetTransform() const
{
    return true;
}

void Object::move(const b2Vec2& move)
{
    _transform.p += move;

    for (auto child: _children)
    {
        child->move(move);
    }
}

void Object::rotate(const float& rot)
{
    _transform.q.Set(_transform.q.GetAngle() + rot);

    for (auto child: _children)
    {
        child->rotate(rot);
    }
}

void Object::_addChild(Object* object)
{
    if (object == nullptr) return;
    _children.push_back(object);
}

void Object::_removeChild(Object* object)
{
    if (object == nullptr) return;
    _children.remove_if([object](const Object* obj){ return obj == object; });
}
