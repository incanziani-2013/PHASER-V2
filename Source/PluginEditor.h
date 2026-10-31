#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/PhaserLookAndFeel.h"

class PhaserAudioProcessorEditor : public juce::AudioProcessorEditor,
                                   private juce::Timer
{
public:
    explicit PhaserAudioProcessorEditor (PhaserAudioProcessor&);
    ~PhaserAudioProcessorEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void addKnob (const juce::String&, const juce::String&, const juce::String&);
    void showModule (int);
    PhaserAudioProcessor& audioProcessor;
    PhaserLookAndFeel look;
    juce::ComboBox presetBox;
    juce::TextButton bypassButton { "BYPASS" };
    juce::Label brand, subtitle, visualizerLabel;
    juce::Slider mix;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    struct Control { juce::String id, name; std::unique_ptr<juce::Slider> slider; std::unique_ptr<juce::Label> label; std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment; };
    std::vector<Control> controls;
    std::vector<std::unique_ptr<juce::TextButton>> tabs;
    int activeModule = 0;
    float visualPhase = 0.0f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhaserAudioProcessorEditor)
};
