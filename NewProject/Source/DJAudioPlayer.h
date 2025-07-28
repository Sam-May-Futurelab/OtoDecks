#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class DJAudioPlayer : public juce::AudioSource
{
  public:

    DJAudioPlayer(juce::AudioFormatManager& _formatManager);
    ~DJAudioPlayer();
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void loadURL(juce::URL audioURL);
    void setGain(double gain);
    void setSpeed(double ratio);
    void setPosition(double posInSecs);
    void setPositionRelative(double pos);
    
    double getLengthInSeconds();

    void start();
    void stop();

    // Get the current position of the track in seconds
    double getPositionRelative();


  private:
    juce::AudioFormatManager& formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::ResamplingAudioSource resamplingSource{&transportSource, false, 2};
    double gain{1.0};
};
