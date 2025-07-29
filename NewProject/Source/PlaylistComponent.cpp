#include <JuceHeader.h>
#include "PlaylistComponent.h"

//==============================================================================
PlaylistComponent::PlaylistComponent()
{
    trackTitles.push_back("Track 1");
    trackTitles.push_back("Track 2");
    trackTitles.push_back("Track 3");
    trackTitles.push_back("Track 4");
    trackTitles.push_back("Track 5");
    trackTitles.push_back("Track 6");

    tableComponent.getHeader().addColumn("Track Title", 1, 400);
    tableComponent.getHeader().addColumn("Play", 2, 200);
    tableComponent.getHeader().setStretchToFitActive(true);

    tableComponent.setModel(this);
    addAndMakeVisible(tableComponent);

}

PlaylistComponent::~PlaylistComponent()
{
}

void PlaylistComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void PlaylistComponent::resized()
{
    tableComponent.setBounds(0, 0, getWidth(), getHeight());
}


int PlaylistComponent::getNumRows()
{
    return static_cast<int>(trackTitles.size());
}


void PlaylistComponent::paintRowBackground(juce::Graphics& g, 
                        int rowNumber, 
                        int width, 
                        int height, 
                        bool rowIsSelected)
  {
    if (rowIsSelected)
    {
        g.fillAll(juce::Colours::lightblue);
    }
    else
    {
        g.fillAll(juce::Colours::darkgrey);
    }
  }

void PlaylistComponent::paintCell(juce::Graphics& g,
                int rowNumber,
                int columnId,
                int width,
                int height,
                bool rowIsSelected)
  {
    if (columnId == 1) // Only draw track titles in column 1 (left)
    {
        g.drawText(trackTitles[rowNumber], 
                    2, 0, 
                    width, 
                    height,
                    juce::Justification::centredLeft, true);
    }
  }

  juce::Component * PlaylistComponent::refreshComponentForCell(int rowNumber, 
                                        int columnId, 
                                        bool isRowSelected, 
                                        juce::Component *existingComponentToUpdate)
  {
      if (columnId == 2) // Column 2 = right side for play buttons
      {
        if (existingComponentToUpdate == nullptr)
        {
          juce::TextButton* btn = new juce::TextButton{"Play"};
          btn->addListener(this);
          existingComponentToUpdate = btn; // Return the same button we added listener to
        }
      }
      return existingComponentToUpdate;
    }

    void PlaylistComponent::buttonClicked(juce::Button* button)
    {
      std::cout << "PlaylistComponent: Button clicked!" << std::endl;
    }