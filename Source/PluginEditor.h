/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "WaveThumbnail.h"
#include "ADSRComponent.h"
#include "NotePlayer.h"

//==============================================================================
/**
*/
class PluginTestAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                        public juce::Slider::Listener, 
                                        public juce::Button::Listener
                                        
{
public:
    PluginTestAudioProcessorEditor (PluginTestAudioProcessor&);
    ~PluginTestAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged(juce::Slider* slider) override;
    void buttonStateChanged(juce::Button* textButton) override;
    void buttonClicked(juce::Button* textbutton) override;
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.

    PluginTestAudioProcessor& audioProcessor;

    juce::Label volumeLabel;
    juce::Slider gainSlider;
    juce::TextButton arp1Button;
    
    NotePlayer mNote1;
    WaveThumbnail mWaveThumbnail;
    ADSRComponent mADSR;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginTestAudioProcessorEditor)
};
