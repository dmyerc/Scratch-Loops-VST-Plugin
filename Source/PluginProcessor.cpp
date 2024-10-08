/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "StandalonePlayHead.h"
#include <cmath>

//==============================================================================
PluginTestAudioProcessor::PluginTestAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ), mAPVTS(*this, nullptr, "PARAMETERS", createParameters())
#endif
{
    mFormatManager.registerBasicFormats();
    mAPVTS.state.addListener (this);

    for (int i = 0; i < mNumVoices; i++)
    {
        msampler.addVoice(new juce::SamplerVoice());
    }
}

PluginTestAudioProcessor::~PluginTestAudioProcessor()
{
    mFormatReader = nullptr;
}

//==============================================================================
const juce::String PluginTestAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginTestAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginTestAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginTestAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginTestAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginTestAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginTestAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PluginTestAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PluginTestAudioProcessor::getProgramName (int index)
{
    return {};
}

void PluginTestAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PluginTestAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    msampler.setCurrentPlaybackSampleRate(sampleRate);
    updateADSR();
    standalonePlayHead.prepare(getSampleRate());
}

void PluginTestAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PluginTestAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif
void PluginTestAudioProcessor::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
}

void PluginTestAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if (mShouldUpdate)
    {
        updateADSR();
    }

    // Extract values for note rhythm 
    int metronomeBPM;
    double currentPpq;
    if (!juce::JUCEApplication::isStandaloneApp()) {
        auto tmp_bpm = getPlayHead()->getPosition()->getBpm();
        auto tmp_ppq = getPlayHead()->getPosition()->getPpqPosition();
        // Set your module level variable to the retrieved value, for use elsewhere
        metronomeBPM = (int)*tmp_bpm;
        currentPpq = (double)*tmp_ppq;
        if (!getPlayHead()->getPosition()->getIsPlaying())
        {
            currentPpq = -1.0;
        }
    }
    else {
        // Not supported in host environment?
        standalonePlayHead(numSamples);
        metronomeBPM = (int)standalonePlayHead.bpm;
        currentPpq = standalonePlayHead.getPpq();
    }

    //check if note should play
    if (mAPVTS.getRawParameterValue("RHYTHMON")->load())
    {
        auto rhythmIndex = (int)mAPVTS.getRawParameterValue("RHYTHM")->load();
            auto rhythmRate = 1 / rhythmValues[rhythmIndex];
            //if (lastPpq != (int)currentPpq && (int)currentPpq % rhythmRate == 0) // 4 is example but should be timeSig.denominator * (1/x) wher 1/x is your rate
            auto remainder = fmod((int)currentPpq, rhythmRate);
            if (lastPpq != (int)currentPpq && (remainder < 0.001 || remainder > -0.001))
            {
                //Play note for X amount of time before next measure
                DBG("Note on");
                midiMessages.addEvent(juce::MidiMessage::noteOn(1, 60 + mAPVTS.getRawParameterValue("PITCH")->load(), 1.0f), 0.0f);
                samplesToStopEvent = getSampleRate() * 1.0 / (1 / rhythmRate);
                DBG(samplesToStopEvent);
                noteStopped = false;
            }
            else if (!noteStopped)
            {
                stopNote(numSamples, totalNumInputChannels, midiMessages);
            }
            lastPpq = (int)currentPpq;
    }
    else
    {
        playLoopNote(numSamples, totalNumInputChannels, midiMessages);
    }
    
    msampler.renderNextBlock(buffer, midiMessages, 0, numSamples);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] = buffer.getSample(channel, sample) * rawVolume;
        }
    }
}

void PluginTestAudioProcessor::stopNote(int numSamples, int totalNumInputChannels, juce::MidiBuffer& midiMessages)
{
    int noteNumber = 60 + mAPVTS.getRawParameterValue("PITCH")->load();        //play middle c
    float velocity = 1.0f;  //Eventually change these non-static values to APVST values that are connected to sliders in notePlayer class
    if (samplesToStopEvent < numSamples)  //time to play the event yet?
    {
        juce::MidiMessage msg;
        msg = juce::MidiMessage::noteOff(1, noteNumber, 0.0f);
        DBG("Note off");
        //here samplesToNextMidiEvent is between 0 and numSamples-1 and controls when
        //the midi event will play: 0 at start of the processblock, numSamples-1 at the end of processBlock
        midiMessages.addEvent(msg, samplesToStopEvent);
        noteStopped = true;
    }

    samplesToStopEvent -= numSamples; //count down with the number of samples in every processblock
}

void PluginTestAudioProcessor::playLoopNote(int numSamples, int totalNumInputChannels, juce::MidiBuffer& midiMessages)
{

    int noteNumber = 60 + mAPVTS.getRawParameterValue("PITCH")->load();        //play middle c
    float velocity = 1.0f;  //Eventually change these non-static values to APVST values that are connected to sliders in notePlayer class

    if (samplesToNextMidiEvent < numSamples)  //time to play the event yet?
    {
        juce::MidiMessage msg;

        if (noteOn)
        {
            msg = juce::MidiMessage::noteOn(1, noteNumber, velocity);
            DBG("Note on");
        }
        else
        {
            msg = juce::MidiMessage::noteOff(1, noteNumber, 0.0f);
            DBG("Note off");
        }

        //here samplesToNextMidiEvent is between 0 and numSamples-1 and controls when
        //the midi event will play: 0 at start of the processblock, numSamples-1 at the end of processBlock
        midiMessages.addEvent(msg, samplesToNextMidiEvent);

        //the next midi event will play in exactly 1 second (assuming a samplerate of 44100)
        //double freq = 1.0 / (bpm / 60.0);
        noteOn = !noteOn;
        samplesToNextMidiEvent += getSampleRate() * mAPVTS.getRawParameterValue("RATE")->load();
    }

    samplesToNextMidiEvent -= numSamples; //count down with the number of samples in every processblock
}

//==============================================================================
bool PluginTestAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginTestAudioProcessor::createEditor()
{
    return new PluginTestAudioProcessorEditor (*this);
}

//==============================================================================
void PluginTestAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PluginTestAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void PluginTestAudioProcessor::loadFile(const juce::String& path)
{
    msampler.clearSounds();

    auto file = juce::File(path);
    mFormatReader = mFormatManager.createReaderFor(file);

    std::unique_ptr<juce::AudioFormatReaderSource> tempSource(new juce::AudioFormatReaderSource(mFormatReader, true));

    auto sampleLength = static_cast<int>(mFormatReader->lengthInSamples);
    mWaveForm.setSize(1, sampleLength);
    mFormatReader->read(&mWaveForm, 0, sampleLength, 0, true, false);

    juce::BigInteger range;
    range.setRange(0, 128, true);

    msampler.addSound(new juce::SamplerSound("Sample", *mFormatReader, range, 60, 0.1, 0.1, 10.0));

    updateADSR();
}

void PluginTestAudioProcessor::updateADSR()
{
    mADSR.attack = mAPVTS.getRawParameterValue ("ATTACK")->load();
    mADSR.decay = mAPVTS.getRawParameterValue ("DECAY")->load();
    mADSR.sustain = mAPVTS.getRawParameterValue ("SUSTAIN")->load();
    mADSR.release = mAPVTS.getRawParameterValue ("RELEASE")->load();

    for (int i = 0; i < msampler.getNumSounds(); ++i)
    {
        if (auto sound = dynamic_cast<juce::SamplerSound*>(msampler.getSound(i).get()))
        {
            sound->setEnvelopeParameters(mADSR);
        }
    }

    mShouldUpdate = false;
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginTestAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    parameters.push_back (std::make_unique<juce::AudioParameterFloat> ("ATTACK", "Attack", 0.0f, 5.0f, 0.0f));
    parameters.push_back (std::make_unique<juce::AudioParameterFloat> ("DECAY", "Decay", 0.0f, 3.0f, 2.0f));
    parameters.push_back (std::make_unique<juce::AudioParameterFloat> ("SUSTAIN", "Sustain", 0.0f, 1.0f, 1.0));
    parameters.push_back (std::make_unique<juce::AudioParameterFloat> ("RELEASE", "Release", 0.0f, 5.0f, 2.0f));
    parameters.push_back (std::make_unique<juce::AudioParameterFloat> ("RATE", "Rate", 0.01f, 2.0f, 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterInt>("PITCH", "Pitch", -24, 24, 0));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("RHYTHMON", "Ryhthm On", false));
    parameters.push_back(std::make_unique<juce::AudioParameterInt>("RHYTHM", "Rhythm Rate", 0, 11, 0));
    return {parameters.begin(), parameters.end() };
}

void PluginTestAudioProcessor::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    mShouldUpdate = true;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginTestAudioProcessor();
}
