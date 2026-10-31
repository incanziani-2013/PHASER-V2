#include "PluginProcessor.h"
#include "PluginEditor.h"

PhaserAudioProcessor::PhaserAudioProcessor()
    : AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PHASER_STATE", createParameterLayout())
{}

juce::AudioProcessorValueTreeState::ParameterLayout PhaserAudioProcessor::createParameterLayout()
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout l;
    auto add = [&l] (const char* id, const char* name, float min, float max, float def, float step = 0.0f)
    {
        l.add (std::make_unique<AudioParameterFloat> (ParameterID { id, 1 }, name,
            NormalisableRange<float> (min, max, step), def));
    };
    auto toggle = [&l] (const char* id, const char* name, bool def)
    { l.add (std::make_unique<AudioParameterBool> (ParameterID { id, 1 }, name, def)); };

    add ("MIX", "Master Mix", 0, 100, 100);
    add ("DRIVE", "Drive", 0, 20, 5);
    add ("TONE", "Tone", 0, 100, 70);
    add ("SAT", "Saturation", 0, 100, 60);
    add ("LOW", "Low", 0, 100, 45);
    add ("MID", "Mid", 0, 100, 65);
    add ("HIGH", "High", 0, 100, 70);
    add ("RES", "Resonance", 0, 10, 4);
    add ("WIDTH", "Width", 0, 150, 75);
    add ("RATE", "Rate", 0.05f, 8.0f, 0.8f);
    add ("DEPTH", "Depth", 0, 100, 35);
    add ("CRUSH", "Crush", 0, 100, 35);
    add ("REDUX", "Redux", 0, 100, 20);
    add ("GLITCHMIX", "Glitch Mix", 0, 100, 50);
    add ("INPUT", "Input", -24, 24, 0);
    add ("OUTPUT", "Output", -24, 12, 0);
    toggle ("BYPASS", "Bypass", false);
    toggle ("FILTERON", "Spectrum Enabled", true);
    toggle ("CHORUSON", "Motion Enabled", true);
    toggle ("GLITCHON", "Glitch Enabled", false);
    return l;
}

void PhaserAudioProcessor::prepareToPlay (double sr, int block)
{
    dsp.prepare (sr, block, getTotalNumOutputChannels());
}
void PhaserAudioProcessor::releaseResources() { dsp.reset(); }

bool PhaserAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto out = layouts.getMainOutputChannelSet();
    return out == AudioChannelSet::mono() || out == AudioChannelSet::stereo();
}

void PhaserAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    dsp.process (buffer, parameters);
}

juce::AudioProcessorEditor* PhaserAudioProcessor::createEditor()
{
    return new PhaserAudioProcessorEditor (*this);
}

void PhaserAudioProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    // Always serialize a valid APVTS tree. Hosts may request state very early.
    const auto state = parameters.copyState();
    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, dest);
    else
        dest.reset();
}

void PhaserAudioProcessor::setStateInformation (const void* data, int size)
{
    // Hosts can pass empty, stale, or malformed state chunks. Never replace the
    // live parameter tree unless the complete XML is valid and has our root tag.
    if (data == nullptr || size <= 0)
        return;

    if (auto xml = getXmlFromBinary (data, size))
    {
        if (! xml->hasTagName (parameters.state.getType().toString()))
            return;

        auto restored = juce::ValueTree::fromXml (*xml);
        if (restored.isValid() && restored.getType() == parameters.state.getType())
            parameters.replaceState (restored);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PhaserAudioProcessor();
}
