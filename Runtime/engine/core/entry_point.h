#pragma once
#ifndef POLOS_CORE_ENTRYPOINT_H
#define POLOS_CORE_ENTRYPOINT_H

#include "core/application.h"
#include "core/log.h"
#include "engine/engine.h"

int main(int argc, char **argv)
{
    polos::Engine::Run();
}

#endif /* POLOS_CORE_ENTRYPOINT_H */
