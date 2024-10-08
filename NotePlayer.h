/*
  ==============================================================================

    NotePlayer.h
    Created: 24 Aug 2024 7:36:14pm
    Author:  domin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class NotePlayer  : public juce::Component
{
public:
    NotePlayer(PluginTestAudioProcessor& p);
    ~NotePlayer() override;

    void paint (juce::Graphics&) override;
    void resized() override;
private:
    //bool noteActive;
    int noteNumber;
    float maxVelocity;
    float midiVelocity = -1.0f;

    PluginTestAudioProcessor& audioProcessor;

    juce::TextButton mRateButton {"Rate Select"};
    std::unique_ptr < juce::AudioProcessorValueTreeState::ButtonAttachment> mRhythmAttachment;

    //Rate DropDown initializers
    juce::StringArray mRateList;
    juce::ComboBox mRateDropDown{ "Rate Tempo" };
    std::unique_ptr < juce::AudioProcessorValueTreeState::ComboBoxAttachment> mRhythmRateAttachment;

    //Rate Slider Initializers
    juce::Slider mRateSlider;
    juce::Label mRateLabel;
    std::unique_ptr < juce::AudioProcessorValueTreeState::SliderAttachment> mRateAttachment;

    //Pitch Slider Initializers
    juce::Slider mPitchSlider;
    juce::Label mPitchLabel;
    std::unique_ptr < juce::AudioProcessorValueTreeState::SliderAttachment> mPitchAttachment;


    void buttonStateChanged();
    void dropDownStateChanged();
    bool inBpmMode();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NotePlayer)
};
