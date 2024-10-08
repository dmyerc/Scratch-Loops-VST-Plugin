/*
  ==============================================================================

    ADSRComponent.h
    Created: 24 Aug 2024 6:07:04pm
    Author:  domin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


//==============================================================================
/*
*/
class ADSRComponent  : public juce::Component
{
public:
    ADSRComponent(PluginTestAudioProcessor& p);
    ~ADSRComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Slider mAttackSlider, mSustainSlider, mDecaySlider, mReleaseSlider;
    juce::Label mAttackLabel, mSustainLabel, mDecayLabel, mReleaseLabel;

    std::unique_ptr < juce::AudioProcessorValueTreeState::SliderAttachment> mAttackAttachment;
    std::unique_ptr < juce::AudioProcessorValueTreeState::SliderAttachment> mDecayAttachment;
    std::unique_ptr < juce::AudioProcessorValueTreeState::SliderAttachment> mSustainAttachment;
    std::unique_ptr < juce::AudioProcessorValueTreeState::SliderAttachment> mReleaseAttachment;

    PluginTestAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRComponent)
};
