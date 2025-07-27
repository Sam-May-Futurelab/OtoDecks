#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"

// Main
class MainComponent  : public juce::AudioAppComponent,
                       public juce::Button::Listener,
                       public juce::Slider::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    // Implement Button::Listener
    void buttonClicked(juce::Button* button) override;

    // Implement Slider::Listener
    void sliderValueChanged(juce::Slider* slider) override;

private:
    juce::Random rand;

    // Deck 1 components
    juce::TextButton playButton{"PLAY"};
    juce::TextButton stopButton{"STOP"};
    juce::TextButton loadButton{"LOAD"};
    
    juce::Slider volSlider;
    juce::Slider speedSlider;
    juce::Slider posSlider;
    
    juce::Label volLabel;
    juce::Label speedLabel;
    juce::Label posLabel;

    // Deck 2 components
    juce::TextButton playButton2{"PLAY"};
    juce::TextButton stopButton2{"STOP"};
    juce::TextButton loadButton2{"LOAD"};
    
    juce::Slider volSlider2;
    juce::Slider speedSlider2;
    juce::Slider posSlider2;
    
    juce::Label volLabel2;
    juce::Label speedLabel2;
    juce::Label posLabel2;

    DJAudioPlayer player1;
    DJAudioPlayer player2;
    
    std::unique_ptr<juce::FileChooser> fileChooser;
    std::unique_ptr<juce::FileChooser> fileChooser2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
