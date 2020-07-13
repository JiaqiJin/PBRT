#ifndef paladin_hpp
#define paladin_hpp

#include "header.h"

class Scene;
class Integrator;

KAWAII_BEGIN

class KawaiiDesune {

public:

    int run(int argc, const char* argv[]);

    Scene* m_scene;
    
    Integrator* m_integrator;
    };

KAWAII_END

#endif /* paladin_hpp */
