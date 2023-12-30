/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "OpenGLComponent.h"
//#include "./juce_cryptography/juce_cryptography.h"
//#include "./juce_dsp/juce_dsp.h"
//#include "./foleys_gui_magic/foleys_gui_magic.cpp"
//==============================================================================
/**
*/



struct ParameterInfo {
    float value;
    std::string label;
    std::string moduleName;

    // Default constructor
    ParameterInfo() : value(0.0f), label(""), moduleName("") {}

    // Constructor with arguments
    ParameterInfo(std::string moduleName, std::string label, float initialValue)
        : moduleName(moduleName), label(label), value(initialValue) {}
};



class NewProjectAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    NewProjectAudioProcessor();
    ~NewProjectAudioProcessor() override;



    std::unordered_map<std::string, ParameterInfo> parameterMap;

    // Method to register a parameter
    void registerParameter(const std::string& moduleName, const std::string& paramName, const std::string& label, float initialValue);

    // Method to get a parameter
    ParameterInfo* getParameter(const std::string& moduleName, const std::string& paramName);
    
    std::vector<float> wavetable;


    float dialValue1 = 100000;
    float dialValue2;
    float dialValue3;
    int widthGL;
    int heightGL;



    bool audio_reset = true;
    bool audio_pause = false;;
    double volume = 8000.0;
    double play_x = 0.0, play_y = 0.0;
    double play_cx = 0.0, play_cy = 0.0;
    double play_nx = 0.0, play_ny = 0.0;
    double play_px = 0.0, play_py = 0.0;
    float sample_rate = 44100;
    float max_freq = 4000;
    const int steps = sample_rate / max_freq;
    float escape_radius_sq = 1000.0;
    int m_audio_time;
    double jx = 1e8;
    double jy = 1e8;
    double mean_x = 0.0;
    double mean_y = 0.0;
    bool normalized = true;
    double dx;
    double dy;
    double dpx;
    double dpy;
    bool sustain = true;

    OpenGLComponent* openGL = nullptr;

    void setOpenGLComponent(OpenGLComponent* component);

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void pushNextSampleIntoFifo(float sample);

    static constexpr auto fftOrder = 10;                // [1]
    static constexpr auto fftSize = 1 << fftOrder;     // [2]
    
    juce::MidiKeyboardState keyboardState;

    void setMidiKeyboardState(juce::MidiKeyboardState* state);
    
    juce::Synthesiser synth;



private:
    juce::dsp::FFT forwardFFT;                          // [3]
    
    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessor)
};
