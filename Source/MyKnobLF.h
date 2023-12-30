/*
  ==============================================================================

    MyKnobLF.h
    Created: 4 Dec 2023 10:03:35pm
    Author:  wolfk

  ==============================================================================
*/
#pragma once
#include <JuceHeader.h>

class MyKnobLF : public juce::LookAndFeel_V4
{
public:
    MyKnobLF();

    void mouseDown(const juce::MouseEvent& e);

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, const float rotaryStartAngle,
        const float rotaryEndAngle, juce::Slider&) override;
};