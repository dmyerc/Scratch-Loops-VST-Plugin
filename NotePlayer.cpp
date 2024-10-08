/*
  ==============================================================================

    NotePlayer.cpp
    Created: 24 Aug 2024 7:36:14pm
    Author:  domin

  ==============================================================================
*/

#include <JuceHeader.h>
#include "NotePlayer.h"

//==============================================================================
NotePlayer::NotePlayer(PluginTestAudioProcessor& p) : audioProcessor(p), noteNumber(60), maxVelocity(1.0f)
{
    //Rate Button
    mRateButton.setClickingTogglesState(true);
    mRateButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::azure);
    mRateButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::darkgrey);
    mRateButton.onClick = [&]() { buttonStateChanged(); };
    mRhythmAttachment = std::make_unique <juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.getAPVTS(), "RHYTHMON", mRateButton);


    addAndMakeVisible(mRateButton);
    //Rate Drop Down
    mRateList.add("1/1");
    mRateList.add("1/2");
    mRateList.add("1/3");
    mRateList.add("1/4");
    mRateList.add("1/6");
    mRateList.add("1/8");
    mRateList.add("1/12");
    mRateList.add("1/16");
    mRateList.add("1/24");
    mRateList.add("1/32");
    mRateList.add("1/48");
    mRateList.add("1/64");
    mRateDropDown.addItemList(mRateList, 1);
    mRateDropDown.onChange = [&]() {dropDownStateChanged(); };
    mRateDropDown.setEnabled(false);
    addAndMakeVisible(mRateDropDown);
    mRhythmRateAttachment = std::make_unique <juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.getAPVTS(), "RHYTHM", mRateDropDown);

    //Rate Slider (ms)
    mRateLabel.setFont(10.0f);
    mRateLabel.setText("Period", juce::NotificationType::dontSendNotification);
    mRateLabel.setJustificationType(juce::Justification::centredTop);
    mRateLabel.attachToComponent(&mRateSlider, false);

    mRateSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mRateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    addAndMakeVisible(mRateSlider);
    mRateAttachment = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), "RATE", mRateSlider);

    //Pitch Slider
    mPitchLabel.setFont(10.0f);
    mPitchLabel.setText("Pitch", juce::NotificationType::dontSendNotification);
    mPitchLabel.setJustificationType(juce::Justification::centredTop);
    mPitchLabel.attachToComponent(&mPitchSlider, false);

    mPitchSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mPitchSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    addAndMakeVisible(mPitchSlider);

    mPitchAttachment = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), "PITCH", mPitchSlider);
}

NotePlayer::~NotePlayer()
{
}

void NotePlayer::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

}

void NotePlayer::resized()
{
    const auto buttonHeight = 0.1f;
    const auto menuWidth = 0.5f;
    const auto menuHeight = 0.4f;
    const auto dialWidth = 0.4f;
    const auto dialHeight = 0.4f;

    mRateButton.setBoundsRelative(0.0f, 0.0f, 0.3f, buttonHeight);
    mRateDropDown.setBoundsRelative(0.0f, buttonHeight, menuWidth, menuHeight);
    mRateSlider.setBoundsRelative(0.0f + menuWidth, buttonHeight, dialWidth, dialHeight);
    mPitchSlider.setBoundsRelative(0.25f, buttonHeight + menuWidth, dialWidth, dialHeight);
}

void NotePlayer::buttonStateChanged()
{
    if (mRateButton.getToggleState())
    {
        //Toggle to Beats Rate
        mRateDropDown.setEnabled(true);
        mRateSlider.setEnabled(false);
    }
    else
    {
        //Toggle to Seconds Rate
        mRateSlider.setEnabled(true);
        mRateDropDown.setEnabled(false);
    }
}

void NotePlayer::dropDownStateChanged()
{
    auto currRate = mRateDropDown.getText();

}

bool NotePlayer::inBpmMode()
{
    return mRateButton.getToggleState();
}