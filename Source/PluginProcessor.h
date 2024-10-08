/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "StandalonePlayHead.h"

//==============================================================================
/**
*/
class PluginTestAudioProcessor  : public juce::AudioProcessor, 
                                  public juce::ValueTree::Listener
{
public:
    //==============================================================================
    PluginTestAudioProcessor();
    ~PluginTestAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void loadFile(const juce::String& path);
    int getNumSamplerSounds() { return msampler.getNumSounds(); };

    void updateADSR();
    juce::ADSR::Parameters& getADSRParams() { return mADSR; };

    double rawVolume;
    bool arp1Active;

    juce::AudioBuffer<float>& getWaveForm() { return mWaveForm; };

    juce::AudioProcessorValueTreeState& getAPVTS() { return mAPVTS; };

private:
    juce::Synthesiser msampler;
    const int mNumVoices { 3 };
    juce::AudioBuffer<float> mWaveForm;

    juce::AudioFormatManager mFormatManager;
    juce::AudioFormatReader* mFormatReader{ nullptr };

    juce::ADSR::Parameters mADSR;

    juce::AudioProcessorValueTreeState mAPVTS;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

    double lastPpq { -1.0 };
    bool noteOn{ false };
    int samplesToNextMidiEvent{ 0 };
    int samplesToStopEvent;
    void playLoopNote(int numSamples, int totalNumInputChannels, juce::MidiBuffer& midiMessages);
    void stopNote(int numSamples, int totalNumInputChannels, juce::MidiBuffer& midiMessages);

    bool noteStopped { false };
    double rhythmValues[12]{ 1.0, 2.0, 3.0, 4.0, 6.0, 8.0, 12.0, 16.0, 24.0, 32.0, 48.0, 64.0 };

    std::atomic<bool> mShouldUpdate{ false };

    dsp::StandalonePlayHead standalonePlayHead;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginTestAudioProcessor)
};
