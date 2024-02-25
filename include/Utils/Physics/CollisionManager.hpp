#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#pragma once

#include <unordered_set>

#include "box2d/b2_world_callbacks.h"
#include "box2d/b2_contact.h"
#include "box2d/b2_collision.h"

#include "Utils/Physics/Collider.hpp"

// TODO make begin and end contact be called after the box2d update function so objects can delete themselves during the callback
class CollisionManager : public b2ContactListener
{
public:
    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;
    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

protected:
    static Object::Ptr addObject(Object* object);
    static void removeObject(Object* object);
    
    friend Collider;

private:
    static std::unordered_set<Object*> _objects;
};

#endif
