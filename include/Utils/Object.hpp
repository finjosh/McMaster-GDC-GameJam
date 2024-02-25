#ifndef OBJECT_H
#define OBJECT_H

#pragma once

#include <atomic>
#include <complex.h>

#include "Utils/EventHelper.hpp"
#include "box2d/b2_math.h"

/// @brief used as a simple implementation of the pure virtual destroy function
#define createDestroy() inline void destroy() override { delete(this); };

// TODO setup object children and position update based on collider
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
        Object* get();
        /// @brief if there is no ptr returns nullptr
        /// @returns obj or nullptr if no obj
        const Object* get() const;
        bool isValid() const;
        /// @brief assigns which obj is stored in this ptr
        /// @param obj the new obj
        void setObject(Object* obj);

    protected:
        void removePtr();

    private:
        Object* _ptr = nullptr;
        unsigned int _eventID = 0;
    };

    Object();
    ~Object();

    void setEnabled(bool enabled = true);
    bool isEnabled() const;

    unsigned long int getID() const;
    Object::Ptr getPtr();

    /// @note if derived class, use the virtual function
    EventHelper::Event onEnabled;
    /// @note if derived class, use the virtual function
    EventHelper::Event onDisabled;
    /// @note if using this in the object just use deconstructor instead
    /// @note you should NOT access any thing about the object through this event
    EventHelper::Event onDestroy;

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
    b2Vec2 rotateAround(const b2Vec2& vec, const float& rot) const;
    /// @brief rotates the given b2Vec2 around the given center
    /// @param vec the point to rotate
    /// @param center the point to rotate around
    /// @param rot rotation in RAD
    /// @returns the rotated vector
    static b2Vec2 rotateAround(const b2Vec2& vec, const b2Vec2& center, const float& rot);
    void setPosition(const b2Vec2& position);
    b2Vec2 getPosition() const;
    /// @param rotation in radians
    void setRotation(const float& rotation);
    /// @returns rotation in radians
    float getRotation() const;
    void setTransform(const b2Transform& transform);
    b2Transform getTransform() const;

protected:
    /// @warning only use this if you know what you are doing
    Object(unsigned long long id);
    /// @warning only use this if you know what you are doing
    void setID(unsigned long long id);
    inline virtual void OnEnable() {};
    inline virtual void OnDisable() {};

private:
    std::atomic_bool _enabled = true;
    unsigned long long _id = 0;

    b2Transform _transform;

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
