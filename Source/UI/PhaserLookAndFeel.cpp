#include "PhaserLookAndFeel.h"

PhaserLookAndFeel::PhaserLookAndFeel()
{
    setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff101914));
    setColour (juce::Label::textColourId, juce::Colour (0xffd8ffe2));
}

void PhaserLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h,
    float pos, float start, float end, juce::Slider&)
{
    auto bounds = juce::Rectangle<float> ((float)x, (float)y, (float)w, (float)h).reduced (5.0f);
    const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.43f;
    const auto c = bounds.getCentre();
    const float angle = start + pos * (end - start);
    g.setColour (juce::Colour (0xff07100b));
    g.fillEllipse (c.x-radius, c.y-radius, radius*2, radius*2);
    g.setColour (juce::Colour (0xff244b32));
    g.drawEllipse (c.x-radius, c.y-radius, radius*2, radius*2, 4.0f);
    juce::Path arc;
    arc.addCentredArc (c.x, c.y, radius+2, radius+2, 0, start, angle, true);
    g.setColour (juce::Colour (0xff35ff70));
    g.strokePath (arc, juce::PathStrokeType (4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    g.setColour (juce::Colour (0xff18231c));
    g.fillEllipse (c.x-radius*0.72f, c.y-radius*0.72f, radius*1.44f, radius*1.44f);
    juce::Path pointer;
    pointer.addRoundedRectangle (-1.5f, -radius*0.65f, 3.0f, radius*0.28f, 1.5f);
    g.setColour (juce::Colour (0xffbaffcb));
    g.fillPath (pointer, juce::AffineTransform::rotation (angle).translated (c.x, c.y));
}

void PhaserLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& b,
    const juce::Colour&, bool over, bool down)
{
    auto r = b.getLocalBounds().toFloat().reduced (1.0f);
    auto col = b.getToggleState() ? juce::Colour (0xff123b20) : juce::Colour (0xff0b120e);
    if (over) col = col.brighter (0.25f);
    if (down) col = col.brighter (0.4f);
    g.setColour (col); g.fillRoundedRectangle (r, 7.0f);
    g.setColour (b.getToggleState() ? juce::Colour (0xff4dff83) : juce::Colour (0xff287443));
    g.drawRoundedRectangle (r, 7.0f, 1.4f);
}
