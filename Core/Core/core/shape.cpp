#include "shape.hpp"

KAWAII_BEGIN

Shape::Shape(const Transform* objectToWorld, const Transform* worldToObject,
    bool reverseOrientation)
    : _objectToWorld(objectToWorld),
    _worldToObject(worldToObject),
    _reverseOrientation(reverseOrientation),
    _transformSwapsHandedness(objectToWorld->swapsHandedness()) {

}

Bounds3f Shape::worldBound() const {
    return _objectToWorld->exec(objectBound());
}

KAWAII_END
