#ifndef OBJECT_H
#define OBJECT_H

#pragma once

#include <atomic>
#include <complex>
#include <list>

#include "Utils/EventHelper.hpp"
#include "box2d/b2_math.h"

/// @brief used as a simple implementation of the pure virtual destroy function
#define createDestroy() inline void destroy() override { delete(this); };

#define PI b2_pi

/// @note the pure virtual "destroy" function only has to handle the destruction of the derived object
class Object
{
public:
    class Ptr
    {
    public:
        Ptr(Object* obj);
        ~Ptr();
        Object* operator->();
        const Object* operator->() const;
        Object* operator*();
        const Object* operator*() const;
        Object::Ptr& operator=(const Object::Ptr& objectPtr);
        explicit operator bool() const;
        bool operator==(const Object::Ptr& objectPtr) const;
        bool operator!=(const Object::Ptr& objectPtr) const;
        bool operator<(const Object::Ptr& objectPtr) const;
        bool operator>(const Object::Ptr& objectPtr) const;
        bool operator==(Object* objectPtr) const;
        bool operator!=(Object* objectPtr) const;
        bool operator<(Object* objectPtr) const;
        bool operator>(Object* objectPtr) const;
        Object* get();
        /// @brief if there is no ptr returns nullptr
        /// @returns obj or nullptr if no obj
        const Object* get() const;
        bool isValid() const;
        /// @brief assigns which obj is stored in this ptr
        /// @param obj the new obj
        void setObject(Object* obj);
        /// @brief if this is invalid then returns 0
        /// @returns the id of the stored object
        size_t getID() const;

    protected:
        void removePtr();

    private:
        Object* _ptr = nullptr;
        unsigned int _eventID = 0;
    };

    Object();
    /// @brief Create an object as a child
    /// @note this will be faster than creating an object and setting it as a child
    Object(Object::Ptr parent);
    ~Object();

    void setEnabled(bool enabled = true);
    bool isEnabled() const;

    unsigned long int getID() const;
    Object::Ptr getPtr();

    /// @brief if nullptr then no parent
    void setParent(Object::Ptr parent);
    /// @brief if invalid then no parent
    Object::Ptr getParent();

    /// @note if derived class, use the virtual function
    EventHelper::Event onEnabled;
    /// @note if derived class, use the virtual function
    EventHelper::Event onDisabled;
    /// @note if using this in the object just use deconstructor instead
    /// @note you should NOT access any thing about the object through this event
    EventHelper::Event onDestroy;
    /// @note this is called when the parent is set to anything but nullptr
    EventHelper::Event onParentSet;
    /// @note this is called when the parent is set to nullptr
    /// @note not called when this object is destroyed
    EventHelper::Event onParentRemoved;

    /// @brief tries to cast this object to a given type
    /// @returns nullptr if cast was unsuccessful  
    template<typename type>
    type* cast()
    {
        type* temp = nullptr;
    
        try
        {
            temp = dynamic_cast<type*>(this);    
        }
        catch(const std::exception& e)
        {
            temp = nullptr;
        }

        return temp;
    }

    /// @brief MUST be implemented in the final class which derives from object
    virtual void destroy() = 0;

    /// @param vec global b2Vec2
    /// @returns the equivalent local b2Vec2
    b2Vec2 getLocalVector(const b2Vec2& vec) const;
    /// @param vec local b2Vec2
    /// @return the equivalent global b2Vec2
    b2Vec2 getGlobalVector(const b2Vec2& vec) const;
    /// @brief rotates the given b2Vec2 around this object
    /// @param vec global vector
    /// @param rot rotation in RAD
    /// @returns the rotated vector
    void rotateAround(const b2Vec2& center, const float& rot);
    /// @brief rotates the given b2Vec2 around the given center
    /// @param vec the point to rotate
    /// @param center the point to rotate around
    /// @param rot rotation in RAD
    /// @returns the rotated vector
    static b2Vec2 rotateAround(const b2Vec2& vec, const b2Vec2& center, const float& rot);
    virtual void setPosition(const b2Vec2& position);
    /// @brief if this is a child then the position will be set according to the parent
    /// @note if "canSetTransform" is false then this does nothing
    /// @note if this is not a child then position is set according to global
    /// @param position according to the parent position
    void setLocalPosition(const b2Vec2& position);
    b2Vec2 getPosition() const;
    /// @note if no parent returns global position
    /// @returns position according to parent
    b2Vec2 getLocalPosition() const;
    /// @param rotation in radians
    virtual void setRotation(const float& rotation);
    /// @brief if this is a child then the rotation will be set according to the parent
    /// @note if "canSetTransform" is false then this does nothing
    /// @note if this is not a child then rotation is set according to global
    /// @param rotation according to the parent rotation
    void setLocalRotation(const float& rotation);
    /// @returns rotation in radians
    float getRotation() const;
    /// @returns the rotation in terms of b2Rot
    b2Rot getRotation_b2() const;
    /// @note if no parent returns global rotation
    /// @returns rotation according to parent
    float getLocalRotation() const;
    virtual void setTransform(const b2Transform& transform);
    b2Transform getTransform() const;
    /// @brief if there is another class that overrides the transform you should 1. not override it 2. not set transforms
    /// @note the transform includes the position and rotation functions
    /// @returns true if you can set transform, position, or rotation
    virtual bool canSetTransform() const;
    void move(const b2Vec2& move);
    /// @param rot in radians
    void rotate(const float& rot);

protected:
    /// @warning only use this if you know what you are doing
    Object(unsigned long long id);
    /// @warning only use this if you know what you are doing
    void setID(unsigned long long id);
    inline virtual void OnEnable() {};
    inline virtual void OnDisable() {};
    /// @warning do NOT disconnect all EVER
    EventHelper::Event _onEnabled;
    /// @warning do NOT disconnect all EVER
    EventHelper::Event _onDisabled;
    /// @warning do NOT disconnect all EVER
    EventHelper::Event _onDestroy;
    /// @warning do NOT disconnect all EVER
    EventHelper::Event _onParentSet;
    /// @warning do NOT disconnect all EVER
    EventHelper::Event _onParentRemoved;

private:
    void _addChild(Object* object);
    void _removeChild(Object* object);

    std::atomic_bool _enabled = true;
    size_t _id = 0;

    // TODO make a renderer that uses this transform
    b2Transform _transform = b2Transform(b2Vec2(0,0), b2Rot(0));

    Object::Ptr _parent = Object::Ptr(nullptr);
    std::list<Object*> _children;

    static std::atomic_ullong _lastID;
};

class _objectComp
{
public:
    bool operator()(const Object* lhs, const Object* rhs) const;
};

namespace std {
    template <>
    struct hash<Object> {
        inline size_t operator()(const Object& obj) const noexcept
        {
            return hash<size_t>{}(obj.getID());
        }
    };
    template <>
    struct hash<Object*> {
        inline size_t operator()(const Object* obj) const noexcept
        {
            if (obj == nullptr)
                return 0;
            return hash<size_t>{}(obj->getID());
        }
    };
    template <>
    struct equal_to<Object> {
        inline bool operator()(const Object& obj, const Object& obj2) const noexcept
        {
            return obj.getID() == obj2.getID();
        }
    };
    template <>
    struct equal_to<Object*> {
        inline bool operator()(const Object* obj, const Object* obj2) const noexcept
        {
            return obj->getID() == obj2->getID();
        }
    };
}

#endif
