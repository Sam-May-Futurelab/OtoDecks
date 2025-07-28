#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "DeckGUI.h"

class MainComponent  : public juce::AudioAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:

    // WaveForm display
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache{100};

    // Audio players and mixers
    DJAudioPlayer player1{formatManager};
    DJAudioPlayer player2{formatManager};
    juce::MixerAudioSource mixerSource;
    
    // GUI components - one for each deck
    DeckGUI deckGUI1{&player1, formatManager, thumbnailCache};
    DeckGUI deckGUI2{&player2, formatManager, thumbnailCache};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};