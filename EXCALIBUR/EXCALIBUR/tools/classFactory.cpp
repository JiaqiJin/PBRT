#include "classFactory.h"

namespace Rendering
{
    ClassFactory* ClassFactory::_instance = nullptr;

    ClassFactory* ClassFactory::getInstance() {
        if (_instance == nullptr) {
            _instance = new ClassFactory();
        }
        return _instance;
    }

    void ClassFactory::registClass(const std::string& className, createObject func) {
        _classMap.insert(std::make_pair(className, func));
    }

    createObject ClassFactory::getCreatorByName(const std::string& className) {
        auto iter = _classMap.find(className);
        if (iter == _classMap.end()) {
            std::cout << className + " is not register" << endl;
            return nullptr;
        }
        return iter->second;
    }
}