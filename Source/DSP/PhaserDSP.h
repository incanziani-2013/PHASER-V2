#pragma once
#include <JuceHeader.h>

class PhaserDSP
{
public:
    void prepare (double sampleRate, int maxBlock, int channels);
    void reset();
    void process (juce::AudioBuffer<float>& buffer,
                  juce::AudioProcessorValueTreeState& state);

private:
    juce::dsp::StateVariableTPTFilter<float> lowPass, highPass;
    juce::dsp::Chorus<float> chorus;
    juce::AudioBuffer<float> dryBuffer;
    juce::Random random;
    double sr = 44100.0;
    float lfoPhase = 0.0f;
    float crushHold[2] = { 0.0f, 0.0f };
    int crushCounter = 0;
};
