/*
  ==============================================================================

    SynthSound.cpp
    Created: 6 Dec 2023 3:44:17pm
    Author:  wolfk

  ==============================================================================
*/

#include "SynthSound.h"


bool SynthSound::appliesToNote(int /*midiNoteNumber*/) {
    return true;
}

bool SynthSound::appliesToChannel(int /*midiChannel*/) {
    return true;
}
