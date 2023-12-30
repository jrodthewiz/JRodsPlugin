/*
  ==============================================================================

    DialModule.h
    Created: 4 Dec 2023 10:01:07pm
    Author:  wolfk

  ==============================================================================
*/
#pragma once
#include <JuceHeader.h>
#include "MyKnobLF.h"
#include "PluginProcessor.h"

class DialModule : public juce::Component
{
public:
    DialModule(MyKnobLF& lookAndFeel, NewProjectAudioProcessor& processor, std::string moduleLabel, int numKnobs = 6);

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    MyKnobLF& lf;
    std::unique_ptr<juce::Label> mainLabel;
    std::vector<std::unique_ptr<juce::Slider>> knobs;
    std::vector<std::unique_ptr<juce::Label>> labels;
    NewProjectAudioProcessor& audioProcessor;
};