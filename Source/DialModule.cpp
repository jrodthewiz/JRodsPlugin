#include "DialModule.h"


DialModule::DialModule(MyKnobLF& lookAndFeel, NewProjectAudioProcessor& processor, std::string moduleLabel, int numKnobs)
    : lf(lookAndFeel), audioProcessor(processor) // Initialize the lf member with the passed lookAndFeel reference
{
    
    mainLabel = std::make_unique<juce::Label>("MainLabel", "Dial Module");
    
    // Set the text justification to centered
    mainLabel->setJustificationType(juce::Justification::centred);
    
    // Set the text color of the main label to white
    mainLabel->setColour(juce::Label::textColourId, juce::Colours::white);
    
    addAndMakeVisible(mainLabel.get());

    // Loop to create and set up knobs and labels equal to the number specified by numKnobs
    for (int i = 0; i < numKnobs; ++i)
    {
        auto knob = std::make_unique<juce::Slider>("Knob " + juce::String(i));
        auto label = std::make_unique<juce::Label>("Label " + juce::String(i), "Dial " + juce::String(i + 1));

        // Set the look and feel of the knob
        knob->setLookAndFeel(&lf);
        // Set the style of the knob to Rotary
        knob->setSliderStyle(juce::Slider::Rotary);
        // Set the range of the knob values
        knob->setRange(-50.001, 50.001, 0.00000001);
        // Set the default value of the knob
        knob->setValue(0.0);
        // Enable a popup display for the knob
        knob->setPopupDisplayEnabled(true, true, knob->getParentComponent(), 1);
        // Set the style of the textbox associated with the knob
        knob->setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

        // Set the text justification of the label to centered
        label->setJustificationType(juce::Justification::centred);
        // Set the text color of the label with modified alpha and brightness
        label->setColour(juce::Label::textColourId, juce::Colours::white.withMultipliedAlpha(0.8).withMultipliedBrightness(2.0));

        // Make the knob visible and add it to the component
        addAndMakeVisible(knob.get());
        // Make the label visible and add it to the component
        addAndMakeVisible(label.get());

        // Add the knob to the vector of knobs
        knobs.push_back(std::move(knob));
        // Add the label to the vector of labels
        labels.push_back(std::move(label));


        // Register parameter with the audio processor
        std::string paramId = "Dial" + std::to_string(i);
        audioProcessor.registerParameter(moduleLabel, paramId, "Parameter for Dial module", knobs[i]->getValue());

        knobs[i]->onValueChange = [this, paramId, i, moduleLabel]() {
            float knobValue = knobs[i]->getValue();
            auto param = audioProcessor.getParameter(moduleLabel, paramId);
            if (param) {
                param->value = knobValue;  // Update the parameter value
            }
        };

    }
}

// Overridden method to handle resizing of the component
void DialModule::resized()
{
    // Get the local bounds of the component
    auto bounds = getLocalBounds();
    // Set the bounds of the main label to occupy the top 20 pixels
    mainLabel->setBounds(bounds.removeFromTop(20));

    // Calculate the number of columns and rows needed to display the knobs
    const int numCols = 3; // Fixed number of columns
    const int numRows = (knobs.size() + numCols - 1) / numCols; // Calculate rows based on the number of knobs
    const int rowHeight = bounds.getHeight() / numRows; // Height of each row
    const int knobWidth = bounds.getWidth() / numCols; // Width of each knob

    // Loop to set the bounds for each knob and its corresponding label
    for (int i = 0; i < knobs.size(); ++i)
    {
        int row = i / numCols; // Calculate the row number
        int col = i % numCols; // Calculate the column number

        // Calculate the area for each knob
        juce::Rectangle<int> knobArea = bounds.withTrimmedTop(rowHeight * row)
            .withTrimmedBottom(rowHeight * (numRows - row - 1))
            .withTrimmedLeft(knobWidth * col)
            .withTrimmedRight(knobWidth * (numCols - col - 1));

        // Set the bounds for the knob and its label
        knobs[i]->setBounds(knobArea.reduced(5).removeFromTop(knobArea.getHeight() - 20));
        labels[i]->setBounds(knobArea.removeFromBottom(20));
    }
}

// Overridden method to handle painting of the component
void DialModule::paint(juce::Graphics& g)
{
    juce::ColourGradient gradient = juce::ColourGradient::vertical(juce::Colour(45, 45, 48), 0, juce::Colour(60, 60, 65), getHeight());

    g.setGradientFill(gradient);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.0f); 

    g.drawRoundedRectangle(getLocalBounds().toFloat(), 10.0f, 2.0f);

    g.fillRect(mainLabel->getBounds());
    g.drawRect(getLocalBounds(), 1);

    juce::Path shadowPath;
    shadowPath.addRoundedRectangle(getLocalBounds().toFloat().reduced(1, 1).translated(0, 2), 10.0f);
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillPath(shadowPath);
    
    auto graphArea = getLocalBounds().reduced(15).removeFromTop(getLocalBounds().getHeight() / 3);

    juce::ColourGradient graphGradient = juce::ColourGradient::vertical(juce::Colour(70, 70, 75), graphArea.getY(), juce::Colour(50, 50, 55), graphArea.getBottom());
    g.setGradientFill(graphGradient);
    g.setColour(juce::Colours::white.withAlpha(0.6f));

}