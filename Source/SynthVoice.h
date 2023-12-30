/*
  ==============================================================================

    SynthVoice.h
    Created: 6 Dec 2023 3:43:43pm
    Author:  wolfk

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class SynthVoice : public juce::SynthesiserVoice {
public:
    SynthVoice(NewProjectAudioProcessor& processor);

    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override;
    void stopNote(float /*velocity*/, bool allowTailOff) override;
    void pitchWheelMoved(int /*newValue*/) override;
    void controllerMoved(int /*controllerNumber*/, int /*newValue*/) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    float applyADSR(double noteElapsedTime, double releaseElapsedTime);

private:
    float currentFrequency = 0.0f;
    float currentLevel = 0.0f;
    double currentPhase = 0.0;
    bool isNotePlaying = false;

    // ADSR envelope parameters
    double attackTime;
    double decayTime;
    double sustainLevel;
    double releaseTime;

    // Timing for ADSR calculation
    double noteOnTime;
    double noteOffTime;
    bool isReleasing;

    void updateADSR();

    float midiNoteToFrequency(int midiNote);
    NewProjectAudioProcessor& audioProcessor;
};

