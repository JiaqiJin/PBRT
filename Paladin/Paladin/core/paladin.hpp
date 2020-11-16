#ifndef paladin_hpp
#define paladin_hpp


#include "header.h"


class Scene;
class Integrator;

PALADIN_BEGIN
    
class Paladin {
    
public:
    
    int run(int argc, const char * argv[]);
    
    Scene * m_scene;
    
    Integrator * m_integrator;
};


PALADIN_END

#endif /* paladin_hpp */
