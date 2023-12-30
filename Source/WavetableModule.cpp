/*
  ==============================================================================

    WavetableModule.cpp
    Created: 5 Dec 2023 12:45:43am
    Author:  wolfk

  ==============================================================================
*/

#include "WavetableModule.h"



WaveTableModule::WaveTableModule(MyKnobLF& lookAndFeel, NewProjectAudioProcessor& processor, std::string moduleLabel) : lf(lookAndFeel), audioProcessor(processor)
{
    waveSelectorKnob = std::make_unique<juce::Slider>("WaveSelector");
    waveSelectorLabel = std::make_unique<juce::Label>("", "Wave");

    waveSelectorKnob->setLookAndFeel(&lf);
    waveSelectorKnob->setSliderStyle(juce::Slider::Rotary);
    waveSelectorKnob->setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 20);
    addAndMakeVisible(waveSelectorKnob.get());

    // Set the range for the knob
    waveSelectorKnob->setRange(0.0, 1.0, 0.01); // Min value, Max value, Step
    waveSelectorKnob->setValue(0.0); // Default value


    //auto param = audioProcessor.getParameter(moduleLabel, "Wave");
    //if (param) {
    //    param->value = 0.0;  // Update the parameter value
    //}

    //audioProcessor.wavetable = getCurrentWaveform();

    waveSelectorKnob->onValueChange = [this, moduleLabel]() {
        // Retrieve the current value of the knob
        float knobValue = waveSelectorKnob->getValue();

        // Update the waveform based on the knob value
        updateWaveform(knobValue);

        // Update the parameter in the AudioProcessor
        auto param = audioProcessor.getParameter(moduleLabel, "Wave");
        if (param) {
            param->value = knobValue;  // Update the parameter value
        }

        audioProcessor.wavetable = getCurrentWaveform();
    };

    waveSelectorLabel->setJustificationType(juce::Justification::centred);
    waveSelectorLabel->attachToComponent(waveSelectorKnob.get(), false);
    addAndMakeVisible(waveSelectorLabel.get());
    
    audioProcessor.registerParameter(moduleLabel, "Wave", "Change waves for WaveTableModule", waveSelectorKnob->getValue());

    initializeDummyWaveform();
}

void WaveTableModule::updateWaveform(float knobValue)
{
    // Example: Map knob value to different waveforms
    // 0-0.25: Sine, 0.25-0.5: Square, 0.5-0.75: Triangle, 0.75-1: Sawtooth
    if (knobValue < 0.25f) generateSineWave();
    else if (knobValue < 0.5f) generateSquareWave();
    else if (knobValue < 0.75f) generateTriangleWave();
    else generateSawtoothWave();

    repaint();
}

void WaveTableModule::generateSineWave() {
    for (int i = 0; i < wavetableSize; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(wavetableSize);
        wavetable[i] = std::sin(t * 2.0f * juce::MathConstants<float>::pi);
    }
}

void WaveTableModule::generateSquareWave() {
    for (int i = 0; i < wavetableSize; ++i) {
        wavetable[i] = (i < wavetableSize / 2) ? 1.0f : -1.0f;
    }
}

void WaveTableModule::generateTriangleWave() {
    for (int i = 0; i < wavetableSize; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(wavetableSize);
        wavetable[i] = 1.0f - 4.0f * std::abs(std::round(t - 0.25f) - (t - 0.25f));
    }
}

void WaveTableModule::generateSawtoothWave() {
    for (int i = 0; i < wavetableSize; ++i) {
        wavetable[i] = 2.0f * (static_cast<float>(i) / static_cast<float>(wavetableSize)) - 1.0f;
    }
}

std::vector<float> WaveTableModule::getCurrentWaveform() const
{
    return wavetable; // Return the current wavetable
}


void WaveTableModule::initializeDummyWaveform()
{
    wavetable.resize(wavetableSize);
    generateSineWave(); // Initialize with a sine wave
    audioProcessor.wavetable = getCurrentWaveform();
}

void WaveTableModule::paint(juce::Graphics& g)
{
    // Gradient Background
    juce::ColourGradient gradient = juce::ColourGradient::vertical(juce::Colour(45, 45, 48), 0, juce::Colour(60, 60, 65), getHeight());

    g.setGradientFill(gradient);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.0f); // Rounded corners

    // Enhanced Module Border
    //g.setColour(juce::Colours::white.withAlpha(0.5f)); // Semi-transparent white
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 10.0f, 2.0f); // Rounded border

    // Define the waveform display area as the upper part of the module
    auto waveformArea = getLocalBounds().reduced(15).removeFromTop(getHeight() / 2);

    // Waveform Area Background
    juce::ColourGradient waveformGradient = juce::ColourGradient::vertical(juce::Colours::grey, waveformArea.getY(), juce::Colours::grey.darker(), waveformArea.getBottom());
    g.setGradientFill(waveformGradient);
    g.fillRoundedRectangle(waveformArea.toFloat(), 8.0f); // Rounded corners for waveform area

    // Waveform Area Border
    g.setColour(juce::Colours::white.withAlpha(0.8f)); // Semi-transparent white for border
    g.drawRoundedRectangle(waveformArea.toFloat(), 8.0f, 1.0f);


    juce::Path shadowPath;
    shadowPath.addRoundedRectangle(getLocalBounds().toFloat().reduced(1, 1).translated(0, 2), 10.0f);
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillPath(shadowPath);

    // Draw the waveform in the defined area
    drawWaveform(g, waveformArea);

}


void WaveTableModule::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    // Height for the waveform area
    auto waveformAreaHeight = bounds.getHeight() / 2;
    auto waveformArea = bounds.removeFromTop(waveformAreaHeight);

    // Space between waveform area and knob
    int spaceBetween = 20;
    bounds.removeFromTop(spaceBetween);

    // Define the knob area
    auto knobArea = bounds;

    // Knob size
    int minimumKnobSize = 60;
    int knobDiameter = juce::jmax(juce::jmin(knobArea.getWidth(), knobArea.getHeight()) - 20, minimumKnobSize);

    // Knob position
    int knobX = knobArea.getX() + (knobArea.getWidth() - knobDiameter) / 2;
    int knobY = knobArea.getY() + (knobArea.getHeight() - knobDiameter) / 2;
    waveSelectorKnob->setBounds(knobX, knobY, knobDiameter, knobDiameter);

    // Label position and properties
    waveSelectorLabel->setJustificationType(juce::Justification::centred);
    waveSelectorLabel->setColour(juce::Label::textColourId, juce::Colours::white); // Set a visible text color
    int labelHeight = 20; // Adjust as needed
    waveSelectorLabel->setBounds(knobX, knobY + knobDiameter - 5, knobDiameter, labelHeight);
    waveSelectorLabel->setVisible(true); // Ensure the label is visible
}







void WaveTableModule::drawWaveform(juce::Graphics& g, const juce::Rectangle<int>& area)
{
    // Visualization of the waveform
    juce::Path waveformPath;
    for (size_t i = 0; i < wavetable.size(); ++i)
    {
        // Map the wavetable index to the horizontal position within the area
        float x = static_cast<float>(i) / static_cast<float>(wavetable.size() - 1) * area.getWidth() + area.getX();

        // Map the wavetable value to the vertical position within the area
        // Ensure the waveform is centered vertically and scaled to fit within the area
        float waveHeight = (wavetable[i] * 0.5f + 0.5f) * area.getHeight();
        float y = area.getY() + area.getHeight() - waveHeight;

        if (i == 0)
            waveformPath.startNewSubPath(x, y);
        else
            waveformPath.lineTo(x, y);
    }

    g.setColour(juce::Colours::white);
    g.strokePath(waveformPath, juce::PathStrokeType(2.0f));
}
