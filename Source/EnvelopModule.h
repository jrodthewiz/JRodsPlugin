/*
  ==============================================================================

    EnvelopModule.h
    Created: 4 Dec 2023 10:36:16pm
    Author:  wolfk

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MyKnobLF.h"
#include "PluginProcessor.h"

class EnvelopeModule : public juce::Component
{
public:
    EnvelopeModule(MyKnobLF& lookAndFeel, NewProjectAudioProcessor& processor, std::string moduleLabel);

    void resized() override;
    void paint(juce::Graphics& g) override;

    void updateGraph();

private:
    MyKnobLF& lf;
    NewProjectAudioProcessor& audioProcessor;
    std::unique_ptr<juce::Label> mainLabel;
    std::unique_ptr<juce::Slider> attackKnob, decayKnob, sustainKnob, releaseKnob;
    std::unique_ptr<juce::Label> attackLabel, decayLabel, sustainLabel, releaseLabel;
    void drawEnvelopeGraph(juce::Graphics& g, const juce::Rectangle<int>& area);
};
