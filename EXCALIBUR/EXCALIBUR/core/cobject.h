#ifndef CObject_h
#define CObject_h

#include "Header.h"

RENDERING_BEGIN

class CObject {
public:
    virtual nloJson toJson() const {
        DCHECK(false);
        return nloJson();
    }

    virtual ~CObject() {

    }
};

RENDERING_END

#endif