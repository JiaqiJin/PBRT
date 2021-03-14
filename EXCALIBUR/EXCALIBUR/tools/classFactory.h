#pragma once

#include "../core/Header.h"

RENDERING_BEGIN

using namespace std;

typedef CObject* CObject_ptr;

typedef initializer_list<const CObject_ptr> Arguments;

typedef CObject_ptr(*createObject)(const nloJson&, const Arguments&);

class ClassFactory {

public:

    createObject getCreatorByName(const std::string& className);

    void registClass(const std::string& className, createObject func);

    static ClassFactory* getInstance();

private:

    ClassFactory(const ClassFactory&) {

    }

    ClassFactory& operator=(const ClassFactory&) {
        return *this;
    }

    std::map<std::string, createObject> _classMap;

    ClassFactory() {

    };

    static ClassFactory* _instance;
};

class RegisterAction {

public:

    RegisterAction(const std::string& className, createObject creator) {
        std::cout << "register " << className << std::endl;
        ClassFactory::getInstance()->registClass(className, creator);
    }
};


#define REGISTER(className, creatorName)                            \
RegisterAction g_Register##creatorName(className,(createObject)creatorName);

#define GET_CREATOR(className) ClassFactory::getInstance()->getCreatorByName(className);


RENDERING_END