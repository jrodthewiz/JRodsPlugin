/*
  ==============================================================================

    MyKnobLF.cpp
    Created: 4 Dec 2023 10:03:35pm
    Author:  wolfk

  ==============================================================================
*/

#include "MyKnobLF.h"


MyKnobLF::MyKnobLF()
{
    setColour(juce::Slider::thumbColourId, juce::Colours::red);
}

//void MyKnobLF::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&)
//{
//    const float radius = juce::jmin(width / 2, height / 2) - 3.0f;
//    const float centreX = x + width * 0.5f;
//    const float centreY = y + height * 0.5f;
//    const float rx = centreX - radius;
//    const float ry = centreY - radius;
//    const float rw = radius * 2.0f;
//    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
//
//    // Improved calculation for glow effect intensity
//    const float percentage = std::pow((angle - rotaryStartAngle) / (rotaryEndAngle - rotaryStartAngle), 2.0f) ; // Square the percentage for non-linear mapping
//
//    // Directly fill the ellipse with a purple color based on the percentage
//    juce::Colour glowColour = juce::Colour::fromFloatRGBA(percentage, 0.0f, percentage, 0.3f); // Purple glow
//    g.setColour(glowColour);
//    g.fillEllipse(rx, ry, rw, rw);
//
//    // Outline of the knob
//    g.setColour(juce::Colours::purple);
//    g.drawEllipse(rx, ry, rw, rw, 1.0f);
//
//    // Pointer
//    juce::Path p;
//    const float pointerLength = radius * 0.33f;
//    const float pointerThickness = 4.0f;
//    p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
//    p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
//
//    g.setColour(juce::Colours::white);
//    g.fillPath(p);
//
//    // Optional shadow for the knob
//    g.setColour(juce::Colours::black.withAlpha(0.5f));
//    g.drawEllipse(rx + 2, ry + 2, rw, rw, 1.0f);
//}




void MyKnobLF::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&)
{
     
    const float radius = juce::jmin(width / 2, height / 2) - 4.0f; // slightly more inset
    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float rx = centreX - radius;
    const float ry = centreY - radius;
    const float rw = radius * 2.0f;
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Vintage knob background
    juce::Colour baseColour(0, 0, 0); // Dark, muted brown
    juce::Colour highlightColour(255, 255, 255); // Light, creamy brown
    juce::ColourGradient gradient(highlightColour.brighter(), centreX, centreY - radius * 0.6f, baseColour.darker(), centreX, centreY + radius, false);
    g.setGradientFill(gradient);
    g.fillEllipse(rx, ry, rw, rw);

    g.fillEllipse(rx, ry, rw, rw);

    juce::Path p;
    const float pointerLength = radius * 0.33f;
    const float pointerThickness = 6.0f; // Slightly thicker pointer for vintage style
    p.addRectangle(-pointerThickness * 0.5f, -radius * 0.6f, pointerThickness, pointerLength); // Start the pointer a bit inside
    p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

    g.setColour(highlightColour.withAlpha(0.85f));
    g.fillPath(p);

    // Shadow for depth, less pronounced than modern styles
    g.setColour(juce::Colours::black.withAlpha(0.2f));
    g.drawEllipse(rx + 1, ry + 1, rw, rw, 1.0f);

    // Knob center cap, reminiscent of vintage gear
    float capRadius = radius * 0.15f;
    g.setColour(baseColour.contrasting(0.5f));
    g.fillEllipse(centreX - capRadius, centreY - capRadius, capRadius * 2.0f, capRadius * 2.0f);
    g.setColour(baseColour.brighter());
    g.drawEllipse(centreX - capRadius, centreY - capRadius, capRadius * 2.0f, capRadius * 2.0f, 1.0f);








}