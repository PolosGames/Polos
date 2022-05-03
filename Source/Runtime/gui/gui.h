#pragma once
#ifndef POLOS_GUI_H_
#define POLOS_GUI_H_

namespace polos
{
    class Gui
    {
    public:
        static void Setup();
        static void Shutdown();
        
        static void Begin();
        static void End();
    };
}

#endif //POLOS_GUI_H_
