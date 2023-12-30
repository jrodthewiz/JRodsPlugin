/*
  ==============================================================================

    SynthSound.h
    Created: 6 Dec 2023 3:44:17pm
    Author:  wolfk

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class SynthSound : public juce::SynthesiserSound {
public:
    bool appliesToNote(int /*midiNoteNumber*/) override;
    bool appliesToChannel(int /*midiChannel*/) override;
};
