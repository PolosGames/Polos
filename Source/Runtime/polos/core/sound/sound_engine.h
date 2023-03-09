#pragma once

#include <fmod.hpp>

#include "polos/core/sound/sound_attributes.h"

namespace polos
{
	class SoundEngine
    {
    public:
        void Startup();
        void Shutdown();

        void Update(float);

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
        static SoundEngine* s_Instance;

        FMOD::System*                   m_System;
        Map<StringId, sound_attributes> m_Sounds;
    };
}// namespace polos
