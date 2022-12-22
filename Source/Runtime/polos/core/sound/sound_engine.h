#ifndef POLOS_CORE_SOUND_SOUNDENGINE_H_
#define POLOS_CORE_SOUND_SOUNDENGINE_H_

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

        void LoadSound(std::string sound_name);

        // Can just pass in a string with _sid operator overload
        // This function is preferred to use for playing audio, because
        // the string id gets calculated at compile time.
        void PlaySoundById(StringId const sound_name);
        void PlaySoundByName(std::string sound_name);

        static SoundEngine* getInstance()
        {
            return s_Instance;
        }
    private:
        static SoundEngine* s_Instance;

        FMOD::System*  m_System;

        std::map<StringId, sound_attributes> m_Sounds;
    };
}// namespace polos

#endif /* POLOS_CORE_SOUND_SOUNDENGINE_H_ */