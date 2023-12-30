#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <complex>
#include "SynthVoice.h"
#include "SynthSound.h"

NewProjectAudioProcessor::NewProjectAudioProcessor(): forwardFFT(fftOrder)
{
}

NewProjectAudioProcessor::~NewProjectAudioProcessor()
{
}

const juce::String NewProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NewProjectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NewProjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NewProjectAudioProcessor::getNumPrograms()
{
    return 1;   
}

int NewProjectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NewProjectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String NewProjectAudioProcessor::getProgramName (int index)
{
    return {};
}

void NewProjectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}


// Register a parameter
void NewProjectAudioProcessor::registerParameter(const std::string& moduleName, const std::string& paramName, const std::string& label, float initialValue) {
    std::string key = moduleName + "." + paramName;
    parameterMap[key] = ParameterInfo(moduleName, label, initialValue);
}

// Get a parameter
ParameterInfo* NewProjectAudioProcessor::getParameter(const std::string& moduleName, const std::string& paramName) {
    std::string key = moduleName + "." + paramName;
    auto it = parameterMap.find(key);
    if (it != parameterMap.end()) {
        return &(it->second);
    }
    return nullptr;
}


void NewProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Initialize the synthesizer
    synth.setCurrentPlaybackSampleRate(sampleRate);
    synth.clearVoices();
    for (int i = 0; i < 4; ++i)  
        synth.addVoice(new SynthVoice(*this));

    synth.clearSounds();
    synth.addSound(new SynthSound());
}

void NewProjectAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NewProjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif
    return true;
  #endif
}
#endif

void NewProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void NewProjectAudioProcessor::setOpenGLComponent(OpenGLComponent* component) 
{ 
    openGL = component; 
}

bool NewProjectAudioProcessor::hasEditor() const
{
    return true; 
}

juce::AudioProcessorEditor* NewProjectAudioProcessor::createEditor()
{
    return new NewProjectAudioProcessorEditor (*this);
}

void NewProjectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void NewProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NewProjectAudioProcessor();
}
