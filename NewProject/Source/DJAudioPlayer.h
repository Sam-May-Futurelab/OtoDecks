/*
  Author: Sam May
  Note: Entire file written by me as part of CM2005 coursework
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "BPMAnalyser.h"

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

    double getPositionRelative();
    double getBPM() const;
    double getOriginalBPM() const;
    double getCurrentSpeed() const;
    bool isBPMAnalysisComplete() const;


  private:
    juce::AudioFormatManager& formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::ResamplingAudioSource resamplingSource{&transportSource, false, 2};
    double gain{1.0};
    double currentSpeedRatio{1.0}; // Track current speed for BPM calculation
    
    // BPM Analysis
    std::unique_ptr<BPMAnalyser> bpmAnalyser;
    double currentTrackBPM{0.0};
    bool bpmAnalysisComplete{false};
    juce::File currentAudioFile;
};
