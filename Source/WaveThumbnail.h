/*
  ==============================================================================

    WaveThumbnail.h
    Created: 24 Aug 2024 5:23:34pm
    Author:  domin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class WaveThumbnail  : public juce::Component, 
                       public juce::FileDragAndDropTarget
{
public:
    WaveThumbnail(PluginTestAudioProcessor& p);
    ~WaveThumbnail() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

private:
    std::vector<float> mAudioPoints;
    juce::String mFileName{ "" };

    PluginTestAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveThumbnail)
};
