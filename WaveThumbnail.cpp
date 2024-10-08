/*
  ==============================================================================

    WaveThumbnail.cpp
    Created: 24 Aug 2024 5:23:34pm
    Author:  domin

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveThumbnail.h"

//==============================================================================
WaveThumbnail::WaveThumbnail(PluginTestAudioProcessor& p) : audioProcessor (p)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

WaveThumbnail::~WaveThumbnail()
{
}

void WaveThumbnail::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (juce::Colours::cadetblue);   // clear the background
    auto waveform = audioProcessor.getWaveForm();

    if (waveform.getNumSamples() > 0)
    {
        juce::Path p;
        mAudioPoints.clear();
        auto waveform = audioProcessor.getWaveForm();
        auto ratio = waveform.getNumSamples() / getWidth();
        auto buffer = waveform.getReadPointer(0);

        //scale audio waveform to fit window x-axis
        for (int sample = 0; sample < waveform.getNumSamples(); sample += ratio)
        {
            mAudioPoints.push_back(buffer[sample]);
        }

        g.setColour(juce::Colours::darkgrey);
        p.startNewSubPath(0, getHeight() / 2);

        //scale the y axis
        for (int sample = 0; sample < mAudioPoints.size(); ++sample)
        {
            auto point = juce::jmap<float>(mAudioPoints[sample], -1.0f, 1.0f, getHeight(), 0);

            p.lineTo(sample, point);
        }

        g.strokePath(p, juce::PathStrokeType(2)); //Thickness of line is 2

        g.setColour(juce::Colours::white);
        g.setFont(15.0f);

        auto textBounds = getLocalBounds().reduced(10, 10);
        g.drawFittedText(mFileName, textBounds, juce::Justification::topRight, 1);
    }
    else
    {
        g.setColour(juce::Colours::white);
        g.setFont(15.0f);
        g.drawFittedText("Drag and Drop Audio Sample Here", getLocalBounds(), juce::Justification::centred, 1);
    }
    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void WaveThumbnail::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}


bool WaveThumbnail::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (auto file : files)
    {
        if (file.contains(".wav") || file.contains(".mp3") || file.contains(".aif"))
        {
            return true;
        }
    }
    return false;
}

void WaveThumbnail::filesDropped(const juce::StringArray& files, int x, int y)
{
    for (auto file : files)
    {
        if (isInterestedInFileDrag(file))
        {
            auto myFile = std::make_unique<juce::File>(file);
            mFileName = myFile->getFileNameWithoutExtension();

            audioProcessor.loadFile(file);
        }
    }

    repaint();
}
