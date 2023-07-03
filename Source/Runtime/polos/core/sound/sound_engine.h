#pragma once

#include <fmod.hpp>

#include "polos/core/sound/sound_attributes.h"
#include "polos/core/events/engine/engine_update.h"

namespace polos
{
	class SoundEngine
    {
    public:
        SoundEngine();
    public:
        void Startup();
        void Shutdown();

        void LoadSound(std::string const p_SoundName);

        // Can just pass in a string with _sid operator overload
        // This function is preferred to use for playing audio, because
        // the string id gets calculated at compile time.
        void PlaySoundById(StringId const p_SoundName);
        void PlaySoundByName(std::string const& p_SoundName);

        static SoundEngine* getInstance()
        {
            return s_Instance;
        }
    private:
        void update(engine_update& p_Event);

    private:
        static SoundEngine* s_Instance;

        FMOD::System*                   m_System;
        Map<StringId, sound_attributes> m_Sounds;
    };
}// namespace polos
