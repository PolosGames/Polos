#ifndef POLOS_CORE_SOUND_SOUND_H_
#define POLOS_CORE_SOUND_SOUND_H_

namespace polos
{
    struct sound_attributes
    {
        bool paused;
        uint32 currentPositionInMillis;
        uint32 durationInMillis;

        FMOD::Sound* soundPtr;
        FMOD::Channel* channelPtr;
    };
} // namespace polos

#endif /* POLOS_CORE_SOUND_SOUND_H_ */