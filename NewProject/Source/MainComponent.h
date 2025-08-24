#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "PlaylistComponent.h"

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
    
    // crossfader mixing logic
    void updateCrossfaderMix();

    // WaveForm display
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache{100};

    // Audio players and mixers
    DJAudioPlayer player1{formatManager};
    DJAudioPlayer player2{formatManager};
    juce::MixerAudioSource mixerSource;
    
    // crossfader gains for mixer
    double crossfaderLeftGain = 0.707; 
    double crossfaderRightGain = 0.707;
    
    // sample rate for audio processing
    double sampleRate = 44100.0;
    
    // GUI components - one for each deck
    DeckGUI deckGUI1{&player1, formatManager, thumbnailCache};
    DeckGUI deckGUI2{&player2, formatManager, thumbnailCache};

    PlaylistComponent playlistComponent;
    
    // crossfader for blending between decks
    juce::Slider crossfader;
    juce::Label crossfaderLabel;
    
    // master volume slider
    juce::Slider masterVolume;
    juce::Label masterVolumeLabel;
    
    // master filter slider
    juce::Slider masterFilter;
    juce::Label masterFilterLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};