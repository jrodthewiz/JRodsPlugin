#pragma once

#include <JuceHeader.h>
#include "MyKnobLF.h"
#include "PluginProcessor.h"

class WaveTableModule : public juce::Component
{
public:
    WaveTableModule(MyKnobLF& lookAndFeel, NewProjectAudioProcessor& processor, std::string moduleLabel);
    void updateWaveform(float knobValue);
    std::vector<float> getCurrentWaveform() const;
    void generateSineWave();
    void generateSquareWave();
    void generateTriangleWave();
    void generateSawtoothWave();

    void paint(juce::Graphics&) override;
    void resized() override;


    const int wavetableSize = 512;


private:
    MyKnobLF& lf;
    NewProjectAudioProcessor& audioProcessor;
    std::unique_ptr<juce::Slider> waveSelectorKnob;
    std::unique_ptr<juce::Label> waveSelectorLabel;
    std::vector<float> wavetable;
    void initializeDummyWaveform();
    void drawWaveform(juce::Graphics& g, const juce::Rectangle<int>& area);
};
