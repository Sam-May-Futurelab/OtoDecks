/*
  ==============================================================================

    DJAudioPlayer.cpp
    Created: 27 Jul 2025 10:55:32am
    Author:  MacBook

  ==============================================================================
*/

#include "DJAudioPlayer.h"

DJAudioPlayer::DJAudioPlayer()
{
    formatManager.registerBasicFormats();
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
    
    // Simple gain application
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
    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset();
    
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)));
    if (reader != nullptr)
    {
        std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader, true));
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
    }
}

void DJAudioPlayer::setGain(double gain)
{
    if (gain < 0.0 || gain > 1.0)
    {
        std::cout << "Gain should be between 0.0 and 1.0" << std::endl;
    }
    else
    {
        this->gain = gain;
    }
}

void DJAudioPlayer::setSpeed(double ratio)
{
    if (ratio <= 0.0 || ratio > 4.0)
    {
        std::cout << "Speed ratio should be between 0.0 and 4.0" << std::endl;
    }
    else
    {
        resamplingSource.setResamplingRatio(ratio);
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