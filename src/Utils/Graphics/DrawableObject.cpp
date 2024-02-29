#include "Utils/Graphics/DrawableObject.hpp"
#include "Utils/Graphics/DrawableManager.hpp"

DrawableObject::DrawableObject(const int& layer) : _layer(layer)
{
    _onParentRemoved(&DrawableObject::setLayer, this, 0);
    _onParentSet([this]()
    { 
        if (auto parent = this->getParent()->cast<DrawableObject>())
        {
            this->setLayer(this->getLayer() + parent->getLayer());
        }
    });

    DrawableManager::addDrawable(this);
}

DrawableObject::~DrawableObject()
{
    DrawableManager::removeDrawable(this);
}

void DrawableObject::setLayer(const int& layer)
{
    DrawableManager::removeDrawable(this);
    _layer = layer;
    DrawableManager::addDrawable(this);
}

int DrawableObject::getLayer() const
{
    return _layer;
}
