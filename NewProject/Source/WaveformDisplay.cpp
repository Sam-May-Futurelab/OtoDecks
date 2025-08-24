#include <JuceHeader.h>
#include "WaveformDisplay.h"

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
    // Dark waveform background
    g.fillAll(juce::Colour::fromRGB(25, 25, 30));

    // Border
    g.setColour(juce::Colour::fromRGB(55, 55, 60));
    g.drawRect(getLocalBounds(), 1);

    if(fileLoaded)
    {
        // blue waveform with transparency
        g.setColour(juce::Colour::fromRGB(116, 185, 255).withAlpha(0.8f));
        audioThumbnail.drawChannel(g,
                              getLocalBounds().reduced(2),
                            0,
                              audioThumbnail.getTotalLength(),
                              0, 1.0f);
        
        // Green position indicator
        g.setColour(juce::Colour::fromRGB(85, 239, 196));
        int posX = position * (getWidth() - 4) + 2;
        g.drawLine(posX, 2, posX, getHeight() - 2, 2.0f);
        
        // Glow effect
      g.setColour(juce::Colour::fromRGB(85, 239, 196).withAlpha(0.3f));
      g.drawLine(posX - 1, 2, posX - 1, getHeight() - 2, 1.0f);
      g.drawLine(posX + 1, 2, posX + 1, getHeight() - 2, 1.0f);
    }
    else
    {
        g.setColour(juce::Colour::fromRGB(180, 180, 185));
        g.setFont(juce::Font(12.0f, juce::Font::italic));
        g.drawText("No audio file loaded...", getLocalBounds(),
                   juce::Justification::centred, true);
    }
}

void WaveformDisplay::resized()
{


}

void WaveformDisplay::loadURL(juce::URL audioURL)
{
    audioThumbnail.clear();
    fileLoaded = audioThumbnail.setSource(new juce::URLInputSource(audioURL));
    if (fileLoaded)
    {
        repaint();
    }
}

void WaveformDisplay::mouseDown(const juce::MouseEvent& event)
{
    if (fileLoaded && audioThumbnail.getTotalLength() > 0)
    {
        // Calculates the relative position (0.0 to 1.0) from the DJ's mouse click
        double clickPosition = (double)(event.x - 2) / (double)(getWidth() - 4);
        clickPosition = juce::jlimit(0.0, 1.0, clickPosition);
        
        // Updates the position and notify parent component
        position = clickPosition;
        repaint();
        
        if (onPositionChange)
            onPositionChange(clickPosition);
    }
}

void WaveformDisplay::mouseDrag(const juce::MouseEvent& event)
{
    mouseDown(event);
}

void WaveformDisplay::changeListenerCallback (juce::ChangeBroadcaster *source)
{

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
  