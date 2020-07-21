#ifndef animatedtransform_hpp
#define animatedtransform_hpp

#include "transform.hpp"

KAWAII_BEGIN

class AnimatedTransform {

    const Transform* _startTransform;
    const Transform* _endTransform;
    const Float _startTime;
    const Float _endTime;
    const bool _actuallyAnimated;

};

KAWAII_END

#endif
