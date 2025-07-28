#include <JuceHeader.h>
#include "WaveformDisplay.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(juce::AudioFormatManager & formatManagerToUse,
                                  juce::AudioThumbnailCache & thumbnailCacheToUse)
    : formatManager(formatManagerToUse), 
      thumbnailCache(thumbnailCacheToUse),
      audioThumbnail(1000, formatManager, thumbnailCache)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (20.0f));
    g.drawText ("File not loaded...", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void WaveformDisplay::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void WaveformDisplay::loadURL(juce::URL audioURL)
{
    std::cout << "wfd: load url" << std::endl;
    audioThumbnail.clear();
    bool loaded =audioThumbnail.setSource(new juce::URLInputSource(audioURL));
    if (loaded)
    {
        std::cout << "WaveformDisplay: Audio loaded successfully." << std::endl;
    }
    else
    {
        std::cout << "WaveformDisplay: Failed to load audio." << std::endl;
    }
}
