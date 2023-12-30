#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "OpenGLComponent.h"

#include "DialModule.h"
#include "EnvelopModule.h"
#include "WavetableModule.h"


class NewProjectAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                        private juce::Slider::Listener, public OpenGLComponent// [2]
{
public:
    NewProjectAudioProcessorEditor (NewProjectAudioProcessor&);
    ~NewProjectAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;


private:
    void sliderValueChanged(juce::Slider* slider) override; // [3]
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NewProjectAudioProcessor& audioProcessor;

    OpenGLComponent openGLComponent;
    juce::FlexBox flexBox;


    std::unique_ptr<DialModule> dial1Module;
    std::unique_ptr<DialModule> dial2Module;
    std::unique_ptr<DialModule> dial3Module;
    std::unique_ptr<EnvelopeModule> envelopeModule;
    std::unique_ptr<WaveTableModule> waveTableModule;

    juce::MidiKeyboardComponent midiKeyboard;

    MyKnobLF lf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};
