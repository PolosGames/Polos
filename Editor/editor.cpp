#include "core/log.h"
#include "polos_pch.h"

#include <polos.h>

#include "core/update_queue.h"
#include "editor.h"

namespace polos
{
    Editor::Editor()
    {
        UpdateQueue::PutLast(UpdateQueue::FuncType::From<Editor, &Editor::Update>(this));
    }

    Editor::~Editor()
    {
    }

    void Editor::Update(float delta_time)
    {
    }

    Application* CreateApplication(void* ptr)
    {
        Application* app = !ptr ? new Editor() : new (ptr) Editor();
        return app;
    }
}