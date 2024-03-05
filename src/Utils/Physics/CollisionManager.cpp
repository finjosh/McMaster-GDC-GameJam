#include "Utils/Physics/CollisionManager.hpp"
#include "Utils/Physics/Collider.hpp"
#include "Utils/ObjectManager.hpp"

std::unordered_set<Collider*> CollisionManager::_objects;

void CollisionManager::BeginContact(b2Contact* contact)
{
    Collider* A = static_cast<Collider*>((void*)contact->GetFixtureA()->GetBody()->GetUserData().pointer);
    Collider* B = static_cast<Collider*>((void*)contact->GetFixtureB()->GetBody()->GetUserData().pointer);
    if (A != nullptr)
    {
        A->BeginContact({B, contact->GetFixtureA(), contact->GetFixtureB()});
    }
    if (B != nullptr)
    {
        B->BeginContact({A, contact->GetFixtureB(), contact->GetFixtureA()});
    }
}

void CollisionManager::EndContact(b2Contact* contact)
{
    Collider* A = static_cast<Collider*>((void*)contact->GetFixtureA()->GetBody()->GetUserData().pointer);
    Collider* B = static_cast<Collider*>((void*)contact->GetFixtureB()->GetBody()->GetUserData().pointer);
    if (A != nullptr)
    {
        A->EndContact({B, contact->GetFixtureA(), contact->GetFixtureB()});
    }
    if (B != nullptr)
    {
        B->EndContact({A, contact->GetFixtureB(), contact->GetFixtureA()});
    }
}

void CollisionManager::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    b2Body* body = contact->GetFixtureA()->GetBody();
    if (body != nullptr)
    {
        Collider* collider = static_cast<Collider*>((void*)body->GetUserData().pointer);
        collider->PreSolve(contact, oldManifold);
    }
    body = contact->GetFixtureB()->GetBody();
    if (body != nullptr)
    {
        Collider* collider = static_cast<Collider*>((void*)body->GetUserData().pointer);
        collider->PreSolve(contact, oldManifold);
    }
}

void CollisionManager::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    b2Body* body = contact->GetFixtureA()->GetBody();
    if (body != nullptr)
    {
        Collider* collider = static_cast<Collider*>((void*)body->GetUserData().pointer);
        collider->PostSolve(contact, impulse);
    }
    body = contact->GetFixtureB()->GetBody();
    if (body != nullptr)
    {
        Collider* collider = static_cast<Collider*>((void*)body->GetUserData().pointer);
        collider->PostSolve(contact, impulse);
    }
}

void CollisionManager::Update()
{
    ObjectManager::ClearDestroyQueue();

    for (auto obj: _objects)
    {
        obj->_updatePosition();
    }
}

void CollisionManager::addCollider(Collider* Collider)
{
    _objects.insert({Collider});
}

void CollisionManager::removeCollider(Collider* collider)
{
    _objects.erase({collider});
}
