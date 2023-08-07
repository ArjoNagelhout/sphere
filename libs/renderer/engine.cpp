#include "engine.hpp"

#include <iostream>
#include <memory>
#include <cassert>

namespace renderer {

    std::unique_ptr<Engine> engine;

    Engine::Engine(const std::string &test) {
        std::cout << "created engine" << std::endl;
    }

    Engine::~Engine() {
        std::cout << "destroyed engine" << std::endl;
    }

    void Engine::doSomething() {
        std::cout << "do something" << std::endl;
    }

    void initializeEngine() {
        engine = std::make_unique<Engine>("test");
    }

    void destroyEngine() {
        engine.reset();
    }

    Engine &getEngine() {
        assert((engine) && "Engine should be initialized before calling this function, did you call initializeEngine()?");
        return *engine;
    }
}