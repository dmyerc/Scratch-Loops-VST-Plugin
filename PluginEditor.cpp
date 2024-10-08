/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginTestAudioProcessorEditor::PluginTestAudioProcessorEditor (PluginTestAudioProcessor& p)
    : AudioProcessorEditor (&p), mWaveThumbnail (p), mADSR (p), mNote1(p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    gainSlider.setRange(-48.0f, 0.0f, 0.01f);
    gainSlider.setValue(-1.0);
    gainSlider.addListener(this);

    volumeLabel.setText("Gain", juce::dontSendNotification);
    volumeLabel.attachToComponent(&gainSlider, false);
    volumeLabel.setJustificationType(juce::Justification::centredBottom);

    addAndMakeVisible(gainSlider);
    addAndMakeVisible(volumeLabel);

    /*arp1Button.setButtonText("Arp1");
    arp1Button.setClickingTogglesState(true);
    arp1Button.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::whitesmoke);
    addAndMakeVisible(arp1Button);*/

    arp1Button.addListener(this);

    //loadButton.onClick = [&]() { audioProcessor.loadFile(); };
    //addAndMakeVisible(loadButton);

    addAndMakeVisible(mNote1);
    addAndMakeVisible(mWaveThumbnail);
    addAndMakeVisible(mADSR);
    setSize(750, 450);

}

PluginTestAudioProcessorEditor::~PluginTestAudioProcessorEditor()
{
}

//==============================================================================
void PluginTestAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PluginTestAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    const auto edgeMargin = 10;
    const auto smallMargin = 8;
    const auto logoHeight = 75;
    const auto logoWidth = 200;
    const auto waveformHeight = 150;
    const auto waveformWidth = 300;
    const auto adsrWidth = 200;
    const auto adsrHeight = 75;
    const auto noteControlWidth = 400;
    const auto noteControlHeight = 200;

    //.setBounds(# of pixels from left edge, # of pixels from top edge, width in pixels, height in pixels)
    mWaveThumbnail.setBounds(edgeMargin, edgeMargin + logoHeight + smallMargin, waveformWidth, waveformHeight);
    mADSR.setBounds(edgeMargin, edgeMargin + logoHeight + smallMargin *2 + waveformHeight, adsrWidth, adsrHeight);
    mNote1.setBounds(waveformWidth + edgeMargin + smallMargin, edgeMargin, noteControlWidth, noteControlHeight);
    //gainSlider.setBoundsRelative(0.0f, 0.15f, sliderWidth, 0.8f);
    //arp1Button.setBounds(gainSlider.getX() + gainSlider.getWidth() + leftMargin, topMargin, buttonWidth, buttonHeight);

}

void PluginTestAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
  {
    if (slider == &gainSlider)
    {
        audioProcessor.rawVolume = pow(10, gainSlider.getValue() / 20);
    }
}

void PluginTestAudioProcessorEditor::buttonStateChanged(juce::Button* textButton)
{

    if (textButton == &arp1Button)
    {
        audioProcessor.arp1Active = arp1Button.getState();
    }
}

void PluginTestAudioProcessorEditor::buttonClicked(juce::Button* textButton)
{

}
