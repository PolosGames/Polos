
#include "sound_engine.h"

#include "polos/utils/stringid.h"
#include "polos/core/engine/engine.h"
#include "polos/core/event_bus.h"

namespace polos
{
    SoundEngine* SoundEngine::s_Instance;

    SoundEngine::SoundEngine()
    {
        SUBSCRIBE_TO_ENGINE_STARTUP(Startup);
        SUBSCRIBE_TO_ENGINE_SHUTDOWN(Shutdown);
    }

    void SoundEngine::Startup()
    {
        s_Instance = this;
        FMOD_RESULT result;

        result = FMOD::System_Create(&m_System);
        if(FMOD_RESULT::FMOD_OK != result)
        {
            LOG_ENGINE_CRITICAL("Could not create FMOD_System!");
        }

        int32_t const max_channels{32};
        result = m_System->init(max_channels, FMOD_INIT_NORMAL, nullptr);
        if(FMOD_RESULT::FMOD_OK != result)
        {
            LOG_ENGINE_CRITICAL("FMOD_System was not able to initialize with {} channels", max_channels);
        }

        SUB_TO_EVENT_MEM_FUN(engine_update, update);
    }

    void SoundEngine::update(engine_update&)
    {
        //result = m_System->playSound(m_Sound, 0, false, &m_Channel);

        m_System->update();
    }

    void SoundEngine::LoadSound(std::string const p_SoundName)
    {
        FMOD_RESULT result;

        StringId sound_string_id = GetStringId(p_SoundName);
        
        auto  iterator_pair    = m_Sounds.insert({sound_string_id, sound_attributes{} });
        auto& sound_attributes = iterator_pair.first->second;

        result = m_System->createSound(("resources/media/" + p_SoundName).c_str(), FMOD_DEFAULT, nullptr, &sound_attributes.soundPtr);
        auto* sound_ptr = sound_attributes.soundPtr;
        if(FMOD_RESULT::FMOD_OK != result)
        {
            LOG_ENGINE_ERROR("Could not create sound: {}", p_SoundName.c_str());
        }

        // Initialize the sound's channel
        result = m_System->playSound(sound_ptr, nullptr, true, &sound_attributes.channelPtr);
        if(FMOD_RESULT::FMOD_OK != result)
        {
            LOG_ENGINE_ERROR("Could not start sound: {}", p_SoundName.c_str());
        }
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
        StringId sound_id = GetStringId(p_SoundName);

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