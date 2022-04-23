#ifndef POLOS_EDITOR_H_
#define POLOS_EDITOR_H_

#include "core/application.h"

namespace polos
{   
    class Editor : public Application
    {
    public:
        Editor();
        ~Editor();
    private:
        void Update(float delta_time);
    };
}

#endif // POLOS_EDITOR_H_