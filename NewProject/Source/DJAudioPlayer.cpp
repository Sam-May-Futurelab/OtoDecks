/*
  Author: Sam May
  Note: Entire file written by me as part of CM2005 coursework
*/

#include "DJAudioPlayer.h"

DJAudioPlayer::DJAudioPlayer(juce::AudioFormatManager& _formatManager) : 
    formatManager(_formatManager),
    resamplingSource(&transportSource, false, 2)
{
    bpmAnalyser = std::make_unique<BPMAnalyser>();
}

DJAudioPlayer::~DJAudioPlayer()
{

}

void DJAudioPlayer::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resamplingSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void DJAudioPlayer::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    resamplingSource.getNextAudioBlock(bufferToFill);
    
    // gain application
    if (gain != 1.0)
    {
        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
        {
            bufferToFill.buffer->applyGain(channel, bufferToFill.startSample, bufferToFill.numSamples, gain);
        }
    }
}

void DJAudioPlayer::releaseResources()
{
    transportSource.releaseResources();
    resamplingSource.releaseResources();
}

void DJAudioPlayer::loadURL(juce::URL audioURL)
{
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(juce::URL::InputStreamOptions
        (juce::URL::ParameterHandling::inAddress)));
    if (reader != nullptr)
    {
        transportSource.setSource(nullptr);
        readerSource.reset();
        
        std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader, true));
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
        
        // Stores the audio file and start BPM analysis
        currentAudioFile = audioURL.getLocalFile();
        bpmAnalysisComplete = false;
        currentTrackBPM = 0.0;
        currentSpeedRatio = 1.0; // Resets the speed ratio for any new track
        
        // Performs the  BPM analysis in background
        if (currentAudioFile.exists())
        {
        // Starts the  BPM analysis
        currentTrackBPM = bpmAnalyser->analyseBPM(currentAudioFile);
        bpmAnalysisComplete = (currentTrackBPM > 0.0);
        }
    }
}

void DJAudioPlayer::setGain(double gain)
{
    if (gain >= 0.0 && gain <= 1.0)
    {
        this->gain = gain;
    }
}

void DJAudioPlayer::setSpeed(double ratio)
{
    if (ratio > 0.0 && ratio <= 4.0)
    {
        resamplingSource.setResamplingRatio(ratio);
        // Tracks the current speed for the BPM calculations
        currentSpeedRatio = ratio;
    }
}

void DJAudioPlayer::setPosition(double posInSecs)
{
    if (posInSecs >= 0.0)
    {
        transportSource.setPosition(posInSecs);
    }
}

void DJAudioPlayer::setPositionRelative(double pos)
{
    if (transportSource.getLengthInSeconds() > 0)
    {
        double posInSecs = pos * transportSource.getLengthInSeconds();
        setPosition(posInSecs);
    }
}

double DJAudioPlayer::getLengthInSeconds()
{
    return transportSource.getLengthInSeconds();
}

void DJAudioPlayer::start()
{
    transportSource.start();
}

void DJAudioPlayer::stop()
{
    transportSource.stop();
}

double DJAudioPlayer::getPositionRelative()
{
        return transportSource.getCurrentPosition() / transportSource.getLengthInSeconds();
}

double DJAudioPlayer::getBPM() const
{
    // Returns the BPM adjusted for current speed ratio
    return currentTrackBPM * currentSpeedRatio;
}

double DJAudioPlayer::getOriginalBPM() const
{
    // Returns the the original BPM before the speed adjustment
    return currentTrackBPM;
}

double DJAudioPlayer::getCurrentSpeed() const
{
    return currentSpeedRatio;
}

bool DJAudioPlayer::isBPMAnalysisComplete() const
{
    return bpmAnalysisComplete;
}