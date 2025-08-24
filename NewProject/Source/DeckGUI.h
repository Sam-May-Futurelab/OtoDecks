/*
  Author: Sam May
  Note: Entire file written by me as part of CM2005 coursework
*/

#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"

class DeckGUI  : public juce::Component,
                public juce::Button::Listener,
                public juce::Slider::Listener,
                public juce::FileDragAndDropTarget,
                public juce::Timer
{
public:
    DeckGUI(DJAudioPlayer* player, juce::AudioFormatManager& formatManagerToUse, juce::AudioThumbnailCache& thumbnailCacheToUse);
    ~DeckGUI() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    // Implement Button::Listener
    void buttonClicked(juce::Button* button) override;
    
    // Implement Slider::Listener
    void sliderValueChanged(juce::Slider* slider) override;

    // Implement FileDragAndDropTarget
    bool isInterestedInFileDrag (const juce::StringArray& files) override;
    void filesDropped (const juce::StringArray& files, int x, int y) override;

    void timerCallback() override;
    
    // Method to load track from external source (like playlist)
    void loadTrack(const juce::File& audioFile);

private:

    // Helper functions for styling
    void styleButton(juce::TextButton& button, juce::Colour accentColor);
    void styleSlider(juce::Slider& slider, juce::Colour accentColor);

    juce::Slider volSlider;
    juce::Slider speedSlider;
    juce::Slider posSlider;
    juce::Label volLabel;
    juce::Label speedLabel;
    juce::Label posLabel;
    // Shows the currently loaded track
    juce::Label trackNameLabel;  
    // Shows the  BPM of current track
    juce::Label bpmLabel; 

    juce::TextButton playButton{"PLAY"};
    juce::TextButton stopButton{"STOP"};
    juce::TextButton loadButton{"LOAD"};
    
    DJAudioPlayer* player;
    std::unique_ptr<juce::FileChooser> fileChooser;

    WaveformDisplay waveformDisplay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};