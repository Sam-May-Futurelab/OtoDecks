#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class WaveformDisplay  : public juce::Component
{
public:
    WaveformDisplay(juce::AudioFormatManager & formatManagerToUse,
                    juce::AudioThumbnailCache & thumbnailCacheToUse);
    ~WaveformDisplay() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void loadURL(juce::URL audioURL);

private:
    juce::AudioFormatManager & formatManager;
    juce::AudioThumbnailCache & thumbnailCache;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
