/*
  ==============================================================================

    StandalonePlayHead.h
    Created: 30 Aug 2024 2:05:21pm
    Author:  domin

  ==============================================================================
*/

#pragma once
#include "juce_audio_basics/juce_audio_basics.h"

#define DebugLoop false
#define DebugIsPlaying false

namespace dsp
{
    using PlayHead = juce::AudioPlayHead;
    using PosInfo = PlayHead::CurrentPositionInfo;
    using Int64 = juce::int64;
    static constexpr double SixtyInv = 1.0 / 60.0;

    inline void movePlayHead(PosInfo& posInfo, double sampleRateInv, int numSamples) noexcept
    {
        posInfo.timeInSamples += numSamples;
        auto timeSecs = static_cast<double>(posInfo.timeInSamples) * sampleRateInv;
        posInfo.timeInSeconds = timeSecs;
        posInfo.ppqPosition += static_cast<double>(numSamples) * sampleRateInv * posInfo.bpm * SixtyInv;
    }

    inline void setPlayHead(PosInfo& posInfo, double bpm, double sampleRateInv, Int64 timeSamples, bool isPlaying)
    {
        posInfo.bpm = bpm;
        posInfo.timeInSamples += timeSamples;
        posInfo.timeInSeconds = static_cast<double>(timeSamples) * sampleRateInv;
        posInfo.ppqPosition = posInfo.timeInSeconds * bpm * SixtyInv;
        posInfo.isPlaying = isPlaying;
    }

    struct StandalonePlayHead
    {
        static constexpr double bpm = 120.0;
        static constexpr double loopStart = 1.0;
        static constexpr double loopEnd = 5.0;
        static constexpr double loopRange = loopEnd - loopStart;
        static constexpr double isPlayingLength = 3.5;
        static constexpr bool isPlayingDefault = true;

        StandalonePlayHead() : posInfo(), sampleRate(1.), sampleRateInv(1.)
#if DebugIsPlaying
            , isPlayingPhase(0.)
#endif
        {
            posInfo.bpm = bpm;
            posInfo.timeSigNumerator = 4;
            posInfo.timeSigDenominator = 4;
            posInfo.timeInSamples = 0;
            posInfo.timeInSeconds = 0.0;
            posInfo.ppqPosition = 0.0;
            posInfo.isPlaying = isPlayingDefault;
            posInfo.isLooping = false;
            posInfo.isRecording = false;
        }

        void prepare(double _sampleRate)
        {
            sampleRate = _sampleRate;
            sampleRateInv = 1.0 / _sampleRate;
        }

        double getPpq()
        {
            return posInfo.ppqPosition;
        }

        void operator()(int numSamples) noexcept
        {
#if DebugIsPlaying
            isPlayingPhase += static_cast<double>(numSamples) * sampleRateInv * bpm * SixtyInv;
            auto state = static_cast<int>(std::floor(isPlayingPhase / isPlayingLength));
            state += isPlayingDefault ? 1 : 0;
            if (state % 2 == 0)
            {
                posInfo.isPlaying = posInfo.isLooping = posInfo.isRecording = false;
            }
            else
            {
                posInfo.isPlaying = true;
                movePlayHead(posInfo, sampleRateInv, numSamples);
            }
#else
            movePlayHead(posInfo, sampleRateInv, numSamples);
            //DBG("ppq ::" << posInfo.ppqPosition);
#endif
#if DebugLoop
            if (posInfo.ppqPosition >= loopEnd)
            {
                posInfo.ppqPosition -= loopRange;
                posInfo.timeInSamples = static_cast<Int64>(posInfo.ppqPosition * 60.0 / posInfo.bpm * sampleRate);
                posInfo.timeInSeconds = static_cast<double>(posInfo.timeInSamples) * sampleRateInv;
            }
#endif
            if (fmod(posInfo.ppqPosition,1.0) == 0.0)
            {
                DBG("ppq ::" << posInfo.ppqPosition);
            }
        }

        PosInfo posInfo;
    protected:
        double sampleRate, sampleRateInv;
#if DebugIsPlaying
        double isPlayingPhase;
#endif
    };
}