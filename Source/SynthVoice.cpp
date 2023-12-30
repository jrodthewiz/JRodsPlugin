/*
  ==============================================================================

    SynthVoice.cpp
    Created: 6 Dec 2023 3:43:43pm
    Author:  wolfk

  ==============================================================================
*/

#include "SynthVoice.h"


SynthVoice::SynthVoice(NewProjectAudioProcessor& processor)
    : audioProcessor(processor), isNotePlaying(false), currentLevel(0), currentPhase(0), attackTime(0), decayTime(0), sustainLevel(0), releaseTime(0), noteOnTime(0), noteOffTime(0), isReleasing(false) {
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound) {
    return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) {
    currentFrequency = midiNoteToFrequency(midiNoteNumber);
    currentLevel = velocity;
    currentPhase = 0.0;
    noteOnTime = juce::Time::getMillisecondCounterHiRes();
    isNotePlaying = true;
    isReleasing = false;
    updateADSR();
}


void SynthVoice::stopNote(float /*velocity*/, bool allowTailOff) {
    if (allowTailOff && isNotePlaying && !isReleasing) {
        // Begin the release phase
        isReleasing = true;
        noteOffTime = juce::Time::getMillisecondCounterHiRes();
    }
    else {
        // Immediately stop the note if tail off is not allowed
        isNotePlaying = false;
        currentLevel = 0;
        clearCurrentNote();
    }
}



void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) {
    if (!isNotePlaying && currentLevel == 0 && !isReleasing) {
        clearCurrentNote();
        return;
    }

    const auto& wavetable = audioProcessor.wavetable;
    auto wavetableSize = wavetable.size();
    if (wavetableSize == 0) return;

    double wavetableIncrement = wavetableSize * currentFrequency / getSampleRate();

    for (int sampleIndex = 0; sampleIndex < numSamples; ++sampleIndex) {

        if (!isNotePlaying) {
            break;
        }


        double currentTime = juce::Time::getMillisecondCounterHiRes() / 1000.0;
        double noteElapsedTime = currentTime - (noteOnTime / 1000.0);
        double releaseElapsedTime = isReleasing ? currentTime - (noteOffTime / 1000.0) : 0.0;

        float level = applyADSR(noteElapsedTime, releaseElapsedTime);

        auto waveSample = wavetable[static_cast<unsigned int>(currentPhase) % wavetableSize];
        currentPhase += wavetableIncrement;

        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
            outputBuffer.addSample(channel, startSample + sampleIndex, waveSample * level);
        }

        // Check if release phase is completed
        if (isReleasing && releaseElapsedTime >= releaseTime) {
            isNotePlaying = false;
            clearCurrentNote();
            break;
        }
    }
}

float SynthVoice::applyADSR(double noteElapsedTime, double releaseElapsedTime) {
    if (!isReleasing) {
        if (noteElapsedTime < attackTime) {
            // Apply attack phase
            return static_cast<float>(noteElapsedTime / attackTime) * currentLevel;
        }
        else if (noteElapsedTime - attackTime < decayTime) {
            // Apply decay phase
            double decayProgress = (noteElapsedTime - attackTime) / decayTime;
            return static_cast<float>((1.0 - decayProgress) * (1.0 - sustainLevel) + sustainLevel) * currentLevel;
        }
        else {
            // Apply sustain level
            return sustainLevel * currentLevel;
        }
    }
    else { // Release phase
        if (releaseElapsedTime < releaseTime) {
            // Smoothly interpolate the level to zero
            return static_cast<float>((1.0 - releaseElapsedTime / releaseTime) * sustainLevel) * currentLevel;
        }
        else {
            // Ensure a smooth transition to complete silence
            isNotePlaying = false; // Mark the note as no longer playing
            return 0.0f;
        }
    }
}

void SynthVoice::updateADSR() {
    auto attackParam = audioProcessor.getParameter("EnvelopeModule", "Attack");
    auto decayParam = audioProcessor.getParameter("EnvelopeModule", "Decay");
    auto sustainParam = audioProcessor.getParameter("EnvelopeModule", "Sustain");
    auto releaseParam = audioProcessor.getParameter("EnvelopeModule", "Release");

    // Convert time parameters from milliseconds to seconds
    if (attackParam) attackTime = attackParam->value / 1000.0;  // Convert from ms to seconds
    if (decayParam) decayTime = decayParam->value / 1000.0;    // Convert from ms to seconds
    if (sustainParam) sustainLevel = sustainParam->value;      // Sustain is unitless, no conversion
    if (releaseParam) releaseTime = releaseParam->value / 1000.0; // Convert from ms to seconds
}


void SynthVoice::pitchWheelMoved(int /*newValue*/) {}

void SynthVoice::controllerMoved(int /*controllerNumber*/, int /*newValue*/) {}

float SynthVoice::midiNoteToFrequency(int midiNote) {
    return 440.0 * pow(2.0, (midiNote - 69) / 12.0);
}
