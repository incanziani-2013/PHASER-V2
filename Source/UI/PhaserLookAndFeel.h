#pragma once
#include <JuceHeader.h>

class PhaserLookAndFeel : public juce::LookAndFeel_V4
{
public:
    PhaserLookAndFeel();
    void drawRotarySlider (juce::Graphics&, int, int, int, int, float, float, float,
                           juce::Slider&) override;
    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&,
                               bool, bool) override;
};
