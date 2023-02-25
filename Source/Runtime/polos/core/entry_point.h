#pragma once

#include "polos/engine/engine.h"

int main(int argc, char **argv)
{
    static_cast<void>(argc);
    static_cast<void>(argv);
    polos::Engine::Run();
}