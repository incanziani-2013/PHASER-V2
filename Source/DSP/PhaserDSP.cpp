#include "PhaserDSP.h"

static float p (juce::AudioProcessorValueTreeState& s, const char* id)
{
    if (auto* v = s.getRawParameterValue (id)) return v->load();
    return 0.0f;
}

void PhaserDSP::prepare (double sampleRate, int maxBlock, int channels)
{
    sr = sampleRate;
    juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32) maxBlock, (juce::uint32) channels };
    lowPass.prepare (spec); highPass.prepare (spec); chorus.prepare (spec);
    lowPass.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
    highPass.setType (juce::dsp::StateVariableTPTFilterType::highpass);
    chorus.setRate (0.25f); chorus.setDepth (0.25f); chorus.setCentreDelay (10.0f);
    chorus.setFeedback (0.08f); chorus.setMix (1.0f);
    dryBuffer.setSize (channels, maxBlock);
    reset();
}

void PhaserDSP::reset()
{
    lowPass.reset(); highPass.reset(); chorus.reset();
    dryBuffer.clear(); lfoPhase = 0.0f; crushCounter = 0;
    crushHold[0] = crushHold[1] = 0.0f;
}

void PhaserDSP::process (juce::AudioBuffer<float>& buffer,
                         juce::AudioProcessorValueTreeState& state)
{
    const int channels = buffer.getNumChannels(), samples = buffer.getNumSamples();
    if (channels == 0 || samples == 0) return;
    dryBuffer.setSize (channels, samples, false, false, true);
    dryBuffer.makeCopyOf (buffer, true);

    const bool bypass = p (state, "BYPASS") > 0.5f;
    const float mix = p (state, "MIX") * 0.01f;
    const float drive = p (state, "DRIVE");
    const float tone = p (state, "TONE");
    const float saturation = p (state, "SAT") * 0.01f;
    const float low = p (state, "LOW");
    const float mid = p (state, "MID");
    const float high = p (state, "HIGH");
    const float resonance = p (state, "RES");
    const float width = p (state, "WIDTH") * 0.01f;
    const float rate = p (state, "RATE");
    const float depth = p (state, "DEPTH") * 0.01f;
    const float crush = p (state, "CRUSH") * 0.01f;
    const float redux = p (state, "REDUX");
    const float glitchMix = p (state, "GLITCHMIX") * 0.01f;
    const float output = juce::Decibels::decibelsToGain (p (state, "OUTPUT"));
    const float input = juce::Decibels::decibelsToGain (p (state, "INPUT"));

    lowPass.setCutoffFrequency (juce::jlimit (300.0f, 18000.0f, 18000.0f - tone * 120.0f));
    lowPass.setResonance (0.5f + resonance * 0.075f);
    highPass.setCutoffFrequency (juce::jlimit (20.0f, 1500.0f, 20.0f + (100.0f - low) * 8.0f));
    highPass.setResonance (0.7f);

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    if (p (state, "FILTERON") > 0.5f)
    {
        highPass.process (context);
        lowPass.process (context);
    }
    if (p (state, "CHORUSON") > 0.5f)
    {
        chorus.setRate (juce::jlimit (0.05f, 8.0f, rate));
        chorus.setDepth (depth);
        chorus.setCentreDelay (5.0f + width * 15.0f);
        chorus.process (context);
    }

    const float preGain = input * (1.0f + drive * 0.055f);
    const float satAmount = saturation * 5.0f;
    const int bits = juce::jlimit (4, 16, (int) std::round (16.0f - redux * 0.12f));
    const float levels = std::pow (2.0f, (float) bits);

    for (int ch = 0; ch < channels; ++ch)
    {
        auto* data = buffer.getWritePointer (ch);
        const auto* dry = dryBuffer.getReadPointer (ch);
        const int c = juce::jmin (ch, 1);
        for (int i = 0; i < samples; ++i)
        {
            float x = data[i] * preGain;
            // COLOR: smooth, bounded tanh waveshaping.
            x = std::tanh (x * (1.0f + satAmount)) / std::tanh (1.0f + satAmount);
            // GLITCH: quantisation with sample-and-hold reduction.
            if (p (state, "GLITCHON") > 0.5f && crush > 0.001f)
            {
                const int holdEvery = juce::jmax (1, (int) (1.0f + crush * 24.0f));
                if ((crushCounter++ % holdEvery) == 0)
                    crushHold[c] = std::round (x * levels) / levels;
                x = x * (1.0f - glitchMix) + crushHold[c] * glitchMix;
            }
            // WIDTH: mid/side width for stereo; mono remains unchanged.
            if (channels >= 2 && ch < 2)
            {
                // Applied as a bounded side gain using the opposite channel's dry signal.
                const float other = dryBuffer.getReadPointer (1 - ch)[i];
                const float midSignal = 0.5f * (dry[i] + other);
                const float sideSignal = 0.5f * (dry[i] - other) * (width * 2.0f);
                const float widened = (ch == 0 ? midSignal + sideSignal : midSignal - sideSignal);
                x = x * 0.75f + widened * 0.25f;
            }
            x *= output;
            const float wet = x;
            data[i] = bypass ? dry[i] : dry[i] * (1.0f - mix) + wet * mix;
        }
    }
}
