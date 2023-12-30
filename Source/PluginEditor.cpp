/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), midiKeyboard(p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{

    dial1Module = std::make_unique<DialModule>(lf, audioProcessor, "DialModule_1");
    /*dial2Module = std::make_unique<DialModule>(lf, audioProcessor, "DialModule_2");
    dial3Module = std::make_unique<DialModule>(lf, audioProcessor, "DialModule_3");*/

    envelopeModule = std::make_unique<EnvelopeModule>(lf, audioProcessor, "EnvelopeModule");
    waveTableModule = std::make_unique<WaveTableModule>(lf, audioProcessor, "WaveTableModule");

    juce::Component::addAndMakeVisible(dial1Module.get());
    /*juce::Component::addAndMakeVisible(dial2Module.get());
    juce::Component::addAndMakeVisible(dial3Module.get());*/
    juce::Component::addAndMakeVisible(envelopeModule.get());
    juce::Component::addAndMakeVisible(waveTableModule.get());
    
    juce::Component::addAndMakeVisible(midiKeyboard);


    const int midiKeyboardHeight = midiKeyboard.getHeight();
    juce::Component::setSize(800 * 1.3, 600 * 1.3 + midiKeyboardHeight);



    //juce::Component::setSize(800 * 1.3, 600 * 1.3);

}

void NewProjectAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    audioProcessor.widthGL = openGLComponent.getWidth();
    audioProcessor.heightGL = openGLComponent.getHeight();
    openGLComponent.DialValue = 1; 
    openGLComponent.DialValue2 = 1;
    openGLComponent.DialValue3 = 1;

}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
}
//==============================================================================
void NewProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    // fill the whole window white
     // Vertical gradient background
    juce::ColourGradient gradient(juce::Colour(30, 30, 30), 0, 0, juce::Colour(60, 60, 60), 0, static_cast<float>(juce::Component::getHeight()), false);
    g.setGradientFill(gradient);
    g.fillAll();


    // set the current drawing colour to black
    g.setColour(juce::Colours::white);

    // set the font size and draw text to the screen
    g.setFont(15.0f);

    g.setColour(juce::Colours::grey);
    g.fillRect(0, 0, juce::Component::getWidth(), 1); // Top border
    g.fillRect(0, Component::getWidth() - 1, Component::getWidth(), 1); // Bottom border

    g.drawFittedText("JRod's Melody Synth", 0, 0, juce::Component::getWidth(), 30, juce::Justification::centred, 1);
}

void NewProjectAudioProcessorEditor::resized()
{
    // Define heights for various components
    const int bottomAreaHeight = 150; // Height for dials
    const int envelopeModuleHeight = 200; // Height for the envelope module
    const int waveTableModuleHeight = 200; // Height for the WaveTableModule
    const int topAreaHeight = 10; // Height for the OpenGL component area
    const int padding = 10; // Uniform padding between modules

    // Get the total bounds of the component
    auto totalBounds = juce::Component::getLocalBounds();

    // Allocate space for the top area (OpenGL component)
    auto topArea = totalBounds.removeFromTop(topAreaHeight);

    // Calculate MIDI keyboard height and width dynamically, with fallback values
    int midiKeyboardHeight = midiKeyboard.getHeight() > 0 ? midiKeyboard.getHeight() : 80;
    int midiKeyboardWidth = juce::Component::getLocalBounds().getWidth() > 0 ? juce::Component::getLocalBounds().getWidth() : 1040;

    // Allocate space for the MIDI keyboard at the bottom
    auto keyboardArea = totalBounds.removeFromBottom(midiKeyboardHeight);
    totalBounds.removeFromBottom(padding); // Add padding above the MIDI keyboard

    // Allocate space for the DialModule
    auto dialArea = totalBounds.removeFromBottom(bottomAreaHeight);
    totalBounds.removeFromBottom(padding); // Add padding above the DialModule

    // Allocate space for the EnvelopeModule with padding
    auto envelopeArea = totalBounds.removeFromBottom(envelopeModuleHeight);
    totalBounds.removeFromBottom(padding); // Add padding above the EnvelopeModule

    // Allocate space for the WaveTableModule with padding
    auto waveTableArea = totalBounds.removeFromBottom(waveTableModuleHeight);

    // Set bounds for each module with appropriate width
    const int waveTableModuleWidth = waveTableArea.getWidth() / 2; // Half the width for WaveTableModule
    const int envelopeModuleWidth = envelopeArea.getWidth() / 2; // Half the width for EnvelopeModule
    const int dialModuleWidth = dialArea.getWidth() / 2; // Half the width for DialModule

    // Position the WaveTableModule, EnvelopeModule, and DialModule
    int waveTableModuleX = waveTableArea.getX() + (waveTableArea.getWidth() - waveTableModuleWidth) / 2;
    int envelopeModuleX = envelopeArea.getX() + (envelopeArea.getWidth() - envelopeModuleWidth) / 2;
    int dialModuleX = dialArea.getX() + (dialArea.getWidth() - dialModuleWidth) / 2;

    waveTableModule->setBounds(waveTableModuleX, waveTableArea.getY(), waveTableModuleWidth, waveTableModuleHeight);
    envelopeModule->setBounds(envelopeModuleX, envelopeArea.getY(), envelopeModuleWidth, envelopeModuleHeight);
    dial1Module->setBounds(dialModuleX, dialArea.getY(), dialModuleWidth, bottomAreaHeight);

    // Set the bounds of the MIDI keyboard
    midiKeyboard.setBounds(0, keyboardArea.getY(), midiKeyboardWidth, midiKeyboardHeight);

    // Ensure MIDI keyboard visibility and bring it to front
    midiKeyboard.toFront(false);
    midiKeyboard.setVisible(true);

    // Layout for dials
    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    flexBox.alignItems = juce::FlexBox::AlignItems::center;
    flexBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    flexBox.items.clear();

    flexBox.performLayout(dialArea);
}












