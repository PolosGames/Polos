
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

    void SoundEngine::LoadSound(std::string sound_name)
    {
        FMOD_RESULT l_result;

        StringId l_sound_string_id = get_string_id(sound_name);
        
        auto  l_iterator_pair    = m_Sounds.insert({l_sound_string_id, sound_attributes{} });
        auto& l_sound_attributes = l_iterator_pair.first->second;

        l_result = m_System->createSound(("resources/media/" + sound_name).c_str(), FMOD_DEFAULT, 0, &l_sound_attributes.soundPtr);
        auto* l_sound_ptr = l_sound_attributes.soundPtr;

        // Initialize the sound's channel
        m_System->playSound(l_sound_ptr, nullptr, true, &l_sound_attributes.channelPtr);
        auto* l_channel_ptr = l_sound_attributes.channelPtr;

        // Initialize the rest of the sound_attributes struct's variables.
        l_channel_ptr->getPosition(&l_sound_attributes.currentPositionInMillis, FMOD_TIMEUNIT_MS);
        l_sound_ptr->getLength(&l_sound_attributes.durationInMillis, FMOD_TIMEUNIT_MS);
        l_sound_attributes.paused = false;
    }

    void SoundEngine::PlaySoundById(StringId const sound_name)
    {
        auto& sound = m_Sounds.at(sound_name);

        m_System->playSound(sound.soundPtr, nullptr, false, &sound.channelPtr);
    }

    void SoundEngine::PlaySoundByName(std::string sound_name)
    {
        StringId sound_id = get_string_id(sound_name);

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