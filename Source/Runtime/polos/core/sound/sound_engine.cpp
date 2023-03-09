
#include "sound_engine.h"

#include "polos/core/update_queue.h"
#include "polos/utils/stringid.h"

namespace polos
{
    SoundEngine* SoundEngine::s_Instance;

    void SoundEngine::Startup()
    {
        s_Instance = this;
        FMOD_RESULT result;

        FMOD::System_Create(&m_System);

        result = m_System->init(32, FMOD_INIT_NORMAL, nullptr);

        UPDATE_Q_MEM_ADD_LAST(Update);
    }

    void SoundEngine::Update(float)
    {
        //result = m_System->playSound(m_Sound, 0, false, &m_Channel);

        m_System->update();
    }

    void SoundEngine::LoadSound(std::string const p_SoundName)
    {
        FMOD_RESULT result;

        StringId sound_string_id = get_string_id(p_SoundName);
        
        auto  iterator_pair    = m_Sounds.insert({sound_string_id, sound_attributes{} });
        auto& sound_attributes = iterator_pair.first->second;

        result = m_System->createSound(("resources/media/" + p_SoundName).c_str(), FMOD_DEFAULT, 0, &sound_attributes.soundPtr);
        auto* sound_ptr = sound_attributes.soundPtr;

        // Initialize the sound's channel
        m_System->playSound(sound_ptr, nullptr, true, &sound_attributes.channelPtr);
        auto* channel_ptr = sound_attributes.channelPtr;

        // Initialize the rest of the sound_attributes struct's variables.
        channel_ptr->getPosition(&sound_attributes.currentPositionInMillis, FMOD_TIMEUNIT_MS);
        sound_ptr->getLength(&sound_attributes.durationInMillis, FMOD_TIMEUNIT_MS);
        sound_attributes.paused = false;
    }

    void SoundEngine::PlaySoundById(StringId const p_SoundName)
    {
        auto& sound = m_Sounds.at(p_SoundName);

        m_System->playSound(sound.soundPtr, nullptr, false, &sound.channelPtr);
    }

    void SoundEngine::PlaySoundByName(std::string const& p_SoundName)
    {
        StringId sound_id = get_string_id(p_SoundName);

        PlaySoundById(sound_id);
    }

    void SoundEngine::Shutdown()
    {
        FMOD_RESULT result;

        std::ranges::for_each(m_Sounds,
            [&result](auto& sound)
            {
                result = sound.second.soundPtr->release();
            }
        );

        result = m_System->close();
        result = m_System->release();
    
        s_Instance = nullptr;
    }
} // namespace polos