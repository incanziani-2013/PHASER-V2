#include "PluginEditor.h"

PhaserAudioProcessorEditor::PhaserAudioProcessorEditor (PhaserAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Keep editor construction deterministic for VST3 hosts. Set the size
    // before adding child components and only attach our LAF to this editor.
    setSize (1100, 720);
    setLookAndFeel (&look);
    brand.setText ("PHASER", juce::dontSendNotification);
    brand.setFont (juce::Font (juce::FontOptions (36.0f, juce::Font::bold)));
    brand.setColour (juce::Label::textColourId, juce::Colour (0xffeffff2));
    addAndMakeVisible (brand);
    subtitle.setText ("ADVANCED COLOR PROCESSOR", juce::dontSendNotification);
    subtitle.setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)));
    subtitle.setColour (juce::Label::textColourId, juce::Colour (0xff38ff79));
    addAndMakeVisible (subtitle);

    presetBox.addItem ("01 - CHROMATIC VOID", 1);
    presetBox.addItem ("02 - NEON DISTORTION", 2);
    presetBox.addItem ("03 - DARK ATMOSPHERE", 3);
    presetBox.addItem ("04 - DIGITAL COLLAPSE", 4);
    presetBox.addItem ("05 - CRYSTAL MOTION", 5);
    presetBox.setSelectedId (1, juce::dontSendNotification);
    addAndMakeVisible (presetBox);

    mix.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    mix.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 62, 18);
    mix.setRange (0, 100, 0.1); mix.setValue (100);
    addAndMakeVisible (mix);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (p.parameters, "MIX", mix);

    bypassButton.setClickingTogglesState (true);
    bypassButton.onClick = [this] { audioProcessor.parameters.getParameter ("BYPASS")->setValueNotifyingHost (bypassButton.getToggleState() ? 1.0f : 0.0f); };
    addAndMakeVisible (bypassButton);

    const std::array<std::pair<const char*, const char*>, 4> moduleNames {{
        {"COLOR", "COLOR"}, {"SPECTRUM", "SPECTRUM"}, {"MOTION", "MOTION"}, {"GLITCH", "GLITCH"}
    }};
    for (int i = 0; i < 4; ++i)
    {
        auto b = std::make_unique<juce::TextButton> (moduleNames[(size_t)i].first);
        b->onClick = [this, i] { showModule (i); };
        addAndMakeVisible (*b); tabs.push_back (std::move (b));
    }
    addKnob ("DRIVE", "DRIVE", "COLOR"); addKnob ("TONE", "TONE", "COLOR");
    addKnob ("SAT", "SATURATION", "COLOR");
    addKnob ("LOW", "LOW", "SPECTRUM"); addKnob ("MID", "MID", "SPECTRUM");
    addKnob ("HIGH", "HIGH", "SPECTRUM"); addKnob ("RES", "RESONANCE", "SPECTRUM");
    addKnob ("WIDTH", "WIDTH", "MOTION"); addKnob ("RATE", "RATE", "MOTION");
    addKnob ("DEPTH", "DEPTH", "MOTION");
    addKnob ("CRUSH", "CRUSH", "GLITCH"); addKnob ("REDUX", "REDUX", "GLITCH");
    addKnob ("GLITCHMIX", "GLITCH MIX", "GLITCH");
    addKnob ("INPUT", "INPUT", "MASTER"); addKnob ("OUTPUT", "OUTPUT", "MASTER");
    showModule (0);
    startTimerHz (30);
}

void PhaserAudioProcessorEditor::addKnob (const juce::String& id, const juce::String& name, const juce::String& module)
{
    Control c; c.id = id; c.name = name;
    c.slider = std::make_unique<juce::Slider>();
    c.slider->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    c.slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 65, 18);
    if (id == "INPUT" || id == "OUTPUT") c.slider->setTextValueSuffix (" dB");
    c.label = std::make_unique<juce::Label>();
    c.label->setText (name, juce::dontSendNotification);
    c.label->setJustificationType (juce::Justification::centred);
    c.label->setFont (juce::Font (juce::FontOptions (10.0f, juce::Font::bold)));
    c.label->setColour (juce::Label::textColourId, juce::Colour (0xffb9ffd0));
    auto* raw = c.slider.get();
    addAndMakeVisible (*c.slider); addAndMakeVisible (*c.label);
    c.attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.parameters, id, *raw);
    controls.push_back (std::move(c));
}

void PhaserAudioProcessorEditor::showModule (int m)
{
    activeModule = m;
    const juce::String names[] = { "COLOR", "SPECTRUM", "MOTION", "GLITCH" };
    for (size_t i = 0; i < tabs.size(); ++i) tabs[i]->setToggleState ((int)i == m, juce::dontSendNotification);
    for (auto& c : controls)
    {
        const bool visible = (m == 0 && (c.id == "DRIVE" || c.id == "TONE" || c.id == "SAT")) ||
            (m == 1 && (c.id == "LOW" || c.id == "MID" || c.id == "HIGH" || c.id == "RES")) ||
            (m == 2 && (c.id == "WIDTH" || c.id == "RATE" || c.id == "DEPTH")) ||
            (m == 3 && (c.id == "CRUSH" || c.id == "REDUX" || c.id == "GLITCHMIX"));
        c.slider->setVisible (visible); c.label->setVisible (visible);
    }
    resized(); repaint();
}

PhaserAudioProcessorEditor::~PhaserAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void PhaserAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff050907));
    auto area = getLocalBounds().toFloat();
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff102218), 0, 0, juce::Colour (0xff050907), 0, (float)getHeight(), false));
    g.fillRect (area);
    g.setColour (juce::Colour (0xff1b6535)); g.drawRoundedRectangle (12, 112, 1076, 520, 18, 1.5f);
    g.setColour (juce::Colour (0xff0d2918)); g.fillRoundedRectangle (20, 122, 1060, 500, 16);
    g.setColour (juce::Colour (0xff39ff75).withAlpha (0.45f));
    g.drawRoundedRectangle (26, 130, 1048, 480, 14, 1.0f);

    // Central energy orb: decorative/reactive visualization.
    const float cx = 550.0f, cy = 365.0f, r = 145.0f + 3.0f * std::sin (visualPhase);
    for (int i = 0; i < 5; ++i)
    {
        g.setColour (juce::Colour (0xff22ff73).withAlpha (0.08f + 0.025f * (float)i));
        g.drawEllipse (cx-r-i*9, cy-r-i*9, (r+i*9)*2, (r+i*9)*2, 2.0f);
    }
    juce::ColourGradient orb (juce::Colour (0xff0dff79).withAlpha (0.35f), cx-60, cy-80,
                              juce::Colour (0xff00bfc9).withAlpha (0.05f), cx+100, cy+120, true);
    g.setGradientFill (orb); g.fillEllipse (cx-r, cy-r, r*2, r*2);
    g.setColour (juce::Colour (0xff49ffc2)); g.drawEllipse (cx-r, cy-r, r*2, r*2, 3.0f);
    juce::Path wave;
    for (int i = 0; i <= 120; ++i)
    {
        const float x = cx-r+10 + (2*r-20) * i / 120.0f;
        const float y = cy + std::sin (i*0.13f + visualPhase) * (18.0f + 22.0f * std::sin (i*0.035f+visualPhase)) +
                        std::cos (i*0.07f-visualPhase) * 18.0f;
        if (i == 0) wave.startNewSubPath (x,y); else wave.lineTo (x,y);
    }
    g.setColour (juce::Colour (0xff39ff8c).withAlpha (0.85f));
    g.strokePath (wave, juce::PathStrokeType (2.2f));
    g.setColour (juce::Colour (0xff00dce8).withAlpha (0.7f));
    g.drawText (juce::String ("PHASER  //  ") + (activeModule == 0 ? "COLOR" : activeModule == 1 ? "SPECTRUM" : activeModule == 2 ? "MOTION" : "GLITCH"),
                390, 535, 320, 24, juce::Justification::centred);
    g.setColour (juce::Colour (0xff39ff75));
    g.drawText ("INPUT", 45, 655, 100, 18, juce::Justification::centredLeft);
    g.drawText ("OUTPUT", 955, 655, 100, 18, juce::Justification::centredRight);
}

void PhaserAudioProcessorEditor::resized()
{
    brand.setBounds (28, 18, 310, 52);
    subtitle.setBounds (31, 68, 300, 18);
    presetBox.setBounds (400, 30, 300, 32);
    mix.setBounds (865, 8, 95, 100);
    bypassButton.setBounds (980, 35, 100, 36);
    for (int i = 0; i < 4; ++i) tabs[(size_t)i]->setBounds (125 + i*215, 590, 195, 38);

    // Controls sit in the side banks; only the active module is visible.
    int visibleIndex = 0;
    for (auto& c : controls)
    {
        if (!c.slider->isVisible()) continue;
        const int col = visibleIndex % 2;
        const int row = visibleIndex / 2;
        const int x = col == 0 ? 45 : 850;
        const int y = 190 + row * 105;
        c.label->setBounds (x, y, 110, 18);
        c.slider->setBounds (x, y+16, 100, 82);
        ++visibleIndex;
    }
}

void PhaserAudioProcessorEditor::timerCallback()
{
    visualPhase += 0.045f;
    if (visualPhase > juce::MathConstants<float>::twoPi) visualPhase = 0.0f;
    repaint (390, 200, 320, 330);
}
