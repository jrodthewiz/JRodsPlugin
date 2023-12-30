/*
  ==============================================================================

    EnvelopModule.cpp
    Created: 4 Dec 2023 10:36:16pm
    Author:  wolfk

  ==============================================================================
*/

#include "EnvelopModule.h"

EnvelopeModule::EnvelopeModule(MyKnobLF& lookAndFeel, NewProjectAudioProcessor& processor, std::string moduleLabel)
    : lf(lookAndFeel), audioProcessor(processor)
{
    // Initialize knobs and labels for ADSR parameters
    attackKnob = std::make_unique<juce::Slider>("Attack");
    decayKnob = std::make_unique<juce::Slider>("Decay");
    sustainKnob = std::make_unique<juce::Slider>("Sustain");
    releaseKnob = std::make_unique<juce::Slider>("Release");

    // Attack Knob - Range from 0 to 5000 milliseconds (0 to 5 seconds)
    attackKnob->setRange(0.0, 5000.0, 1.0);
    attackKnob->setValue(500.0); // Example default value: 500 ms

    // Decay Knob - Range from 0 to 5000 milliseconds (0 to 5 seconds)
    decayKnob->setRange(0.0, 5000.0, 1.0);
    decayKnob->setValue(500.0); // Example default value: 500 ms

    // Sustain Knob - Range from 0.0 (silence) to 1.0 (full level)
    sustainKnob->setRange(0.0, 1.0, 0.01);
    sustainKnob->setValue(0.7); // Example default value: 70% level

    // Release Knob - Range from 0 to 5000 milliseconds (0 to 5 seconds)
    releaseKnob->setRange(0.0, 5000.0, 1.0);
    releaseKnob->setValue(500.0); // Example default value: 500 ms




    attackLabel = std::make_unique<juce::Label>("", "Attack");
    decayLabel = std::make_unique<juce::Label>("", "Decay");
    sustainLabel = std::make_unique<juce::Label>("", "Sustain");
    releaseLabel = std::make_unique<juce::Label>("", "Release");


    /*attackKnob->onValueChange = [this] { this->updateGraph(); };
    decayKnob->onValueChange = [this] { this->updateGraph(); };
    sustainKnob->onValueChange = [this] { this->updateGraph(); };
    releaseKnob->onValueChange = [this] { this->updateGraph(); };*/

    // Set up knob and label properties using a lambda function
    auto setupKnob = [this, moduleLabel](std::unique_ptr<juce::Slider>& knob, std::unique_ptr<juce::Label>& label, const std::string& paramId) {
        knob->setLookAndFeel(&lf);
        knob->setSliderStyle(juce::Slider::Rotary);
        knob->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        addAndMakeVisible(knob.get());

        label->setJustificationType(juce::Justification::centred);
        label->attachToComponent(knob.get(), false);
        addAndMakeVisible(label.get());

        knob->onValueChange = [this, paramId, &knob, moduleLabel]() {

            this->updateGraph();

            // Retrieve the current value of the knob
            float knobValue = knob->getValue();

            // Update the parameter in the AudioProcessor
            auto param = audioProcessor.getParameter(moduleLabel, paramId);
            if (param) {
                param->value = knobValue;  // Update the parameter value
            }
        };

        // Register parameter with the audio processor
        audioProcessor.registerParameter(moduleLabel, paramId, "Change " + paramId, knob->getValue());
    };

    // Setup each knob and label
    setupKnob(attackKnob, attackLabel, "Attack");
    setupKnob(decayKnob, decayLabel, "Decay");
    setupKnob(sustainKnob, sustainLabel, "Sustain");
    setupKnob(releaseKnob, releaseLabel, "Release");
}

void EnvelopeModule::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    // Adjust the ratio between the knobs and graph areas
    auto graphHeight = bounds.getHeight() / 3; // One-third for the graph
    auto knobsHeight = bounds.getHeight() - graphHeight; // Remaining space for knobs
    auto knobsArea = bounds.removeFromBottom(knobsHeight);

    int knobWidth = knobsArea.getWidth() / 4;

    // Add some padding to the top of the knobs area to prevent overlapping with the graph
    int knobPadding = 25; // You can adjust this value as needed
    knobsArea.removeFromTop(knobPadding);

    // Position the knobs
    attackKnob->setBounds(knobsArea.removeFromLeft(knobWidth).reduced(10));
    decayKnob->setBounds(knobsArea.removeFromLeft(knobWidth).reduced(10));
    sustainKnob->setBounds(knobsArea.removeFromLeft(knobWidth).reduced(10));
    releaseKnob->setBounds(knobsArea.reduced(10));
}

void EnvelopeModule::updateGraph()
{
    repaint(); // This will trigger the paint method and redraw the graph
}

void EnvelopeModule::paint(juce::Graphics& g)
{
    // Gradient Background
    juce::ColourGradient gradient = juce::ColourGradient::vertical(juce::Colour(45, 45, 48), 0, juce::Colour(60, 60, 65), getHeight());
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.0f);
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 10.0f, 2.0f);
    // Manual Shadow Effect
    juce::Path shadowPath;
    shadowPath.addRoundedRectangle(getLocalBounds().toFloat().reduced(1, 1).translated(0, 2), 10.0f);
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillPath(shadowPath);
    // Graph Area Definition
    auto graphArea = getLocalBounds().reduced(15).removeFromTop(getLocalBounds().getHeight() / 3);

    // Modern Graph Area Background
    juce::ColourGradient graphGradient = juce::ColourGradient::vertical(juce::Colour(70, 70, 75), graphArea.getY(), juce::Colour(50, 50, 55), graphArea.getBottom());
    g.setGradientFill(graphGradient);
    g.fillRoundedRectangle(graphArea.toFloat(), 8.0f);

    // Graph Area Border with Enhanced Visibility
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.drawRoundedRectangle(graphArea.toFloat(), 8.0f, 1.0f);

    // Draw the Enhanced ADSR graph
    drawEnvelopeGraph(g, graphArea);
}


void EnvelopeModule::drawEnvelopeGraph(juce::Graphics& g, const juce::Rectangle<int>& area)
{
    // Get knob values
    float attackValue = attackKnob->getValue() / 5000.0f;  // Scaled from 0 to 1
    float decayValue = decayKnob->getValue() / 5000.0f;    // Scaled from 0 to 1
    float sustainValue = sustainKnob->getValue();          // Already in 0 to 1 range
    float releaseValue = releaseKnob->getValue() / 5000.0f; // Scaled from 0 to 1


    // Map the values to the pixel coordinates in the graph area
    int graphWidth = area.getWidth();
    int graphHeight = area.getHeight();

    int attackX = area.getX() + static_cast<int>(attackValue * graphWidth);
    int decayX = attackX + static_cast<int>(decayValue * graphWidth);
    int sustainY = area.getY() + static_cast<int>((1 - sustainValue) * graphHeight); // Invert sustain value
    int releaseX = decayX + static_cast<int>(releaseValue * graphWidth);

    // Draw the ADSR graph
    juce::Path envelopePath;
    envelopePath.startNewSubPath(area.getX(), area.getBottom());
    envelopePath.lineTo(attackX, area.getY()); // Attack phase
    envelopePath.lineTo(decayX, sustainY); // Decay to sustain level
    envelopePath.lineTo(releaseX, area.getBottom()); // Release phase

    // Customized graph style
    juce::ColourGradient envelopeGradient(juce::Colours::lightgreen, area.getX(), area.getY(),
        juce::Colours::orange, area.getRight(), area.getBottom(), false);
    g.setGradientFill(envelopeGradient);
    g.strokePath(envelopePath, juce::PathStrokeType(2.0f));

    // Additional graphical enhancements
    // Dashed lines for sustain level
    juce::Path dashedLine;
    dashedLine.startNewSubPath(decayX, sustainY);
    dashedLine.lineTo(area.getRight(), sustainY);
    juce::PathStrokeType dashed(1.0f, juce::PathStrokeType::beveled);
    g.setColour(juce::Colours::grey);
    g.strokePath(dashedLine, dashed);

    // Text labels for ADSR values (optional)
    g.setColour(juce::Colours::white);
}

