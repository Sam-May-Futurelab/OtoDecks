#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class WaveformDisplay  : public juce::Component,
                         public juce::ChangeListener
{
public:
    WaveformDisplay(juce::AudioFormatManager & formatManagerToUse,
                    juce::AudioThumbnailCache & thumbnailCacheToUse);
    ~WaveformDisplay() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void changeListenerCallback (juce::ChangeBroadcaster *source) override;

    void loadURL(juce::URL audioURL);

    // sets the position of the waveform display relative to the audio file
    void setPositionRelative(double pos);

private:
    juce::AudioFormatManager & formatManager;
    juce::AudioThumbnailCache & thumbnailCache;
    juce::AudioThumbnail audioThumbnail;
    bool fileLoaded;
    double position;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
