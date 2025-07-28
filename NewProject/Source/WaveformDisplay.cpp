#include <JuceHeader.h>
#include "WaveformDisplay.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(juce::AudioFormatManager & formatManagerToUse,
                                  juce::AudioThumbnailCache & thumbnailCacheToUse)
    : formatManager(formatManagerToUse), 
      thumbnailCache(thumbnailCacheToUse),
      audioThumbnail(1000, formatManager, thumbnailCache),
      fileLoaded(false),
      position(0.0)
{
    audioThumbnail.addChangeListener(this);
}

WaveformDisplay::~WaveformDisplay()
{

}

void WaveformDisplay::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    if(fileLoaded)
    {
        audioThumbnail.drawChannel(g,
                                  getLocalBounds(),
                                  0,
                                  audioThumbnail.getTotalLength(),
                                  0, 1.0f);
        
        g.setColour(juce::Colours::green);
        g.drawRect(position * getWidth(), 0, 10, getHeight());
    }
    else
      {
        g.setFont (juce::FontOptions (20.0f));
        g.drawText ("File not loaded...", getLocalBounds(),
        juce::Justification::centred, true);   // draw some placeholder text
      }
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
    fileLoaded = audioThumbnail.setSource(new juce::URLInputSource(audioURL));
    if (fileLoaded)
    {
        std::cout << "WaveformDisplay: Audio loaded successfully." << std::endl;
        repaint();
    }
    else
    {
        std::cout << "WaveformDisplay: Failed to load audio." << std::endl;
    }
}

void WaveformDisplay::changeListenerCallback (juce::ChangeBroadcaster *source)
{
        std::cout << "WaveformDisplay: Change listener callback triggered." << std::endl;

        repaint();

}

void WaveformDisplay::setPositionRelative(double pos)
{
  if (pos != position)
  {
    position = pos;
    repaint();
  }
}
  