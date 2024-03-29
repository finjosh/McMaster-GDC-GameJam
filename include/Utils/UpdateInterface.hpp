#ifndef UPDATEINTERFACE_H
#define UPDATEINTERFACE_H

#pragma once

#include "Utils/Object.hpp"

class UpdateInterface : public virtual Object
{
public:

    UpdateInterface();
    ~UpdateInterface();

    /// @brief called every frame
    virtual void Update(const float& deltaTime);
    /// @brief called every frame after update
    virtual void LateUpdate(const float& deltaTime);
    /// @brief called up to 50 times a second
    virtual void FixedUpdate();
    /// @brief called right before window opens
    /// @note called even if the object is disabled
    virtual void Start();

protected:

private:
    
};

#endif
