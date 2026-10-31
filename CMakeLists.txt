cmake_minimum_required(VERSION 3.22)
project(PHASER VERSION 1.0.0)

include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(PHASER
    COMPANY_NAME "PHASER Audio"
    BUNDLE_ID "com.phaseraudio.phaser"
    PLUGIN_MANUFACTURER_CODE Phsr
    PLUGIN_CODE Ph01
    FORMATS VST3 Standalone
    PRODUCT_NAME "PHASER"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)

juce_generate_juce_header(PHASER)
target_sources(PHASER PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/DSP/PhaserDSP.cpp
    Source/UI/PhaserLookAndFeel.cpp)
target_include_directories(PHASER PRIVATE Source Source/DSP Source/UI)
target_compile_features(PHASER PRIVATE cxx_std_17)
target_compile_definitions(PHASER PUBLIC
    JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(PHASER PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_gui_extra
    PUBLIC juce::juce_recommended_config_flags
           juce::juce_recommended_lto_flags
           juce::juce_recommended_warning_flags)
