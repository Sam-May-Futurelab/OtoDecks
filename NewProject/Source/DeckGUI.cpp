/*
  ==============================================================================

    DeckGUI.cpp
    Created: 27 Jul 2025 2:52:05pm
    Author:  MacBook

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DeckGUI.h"

//==============================================================================
DeckGUI::DeckGUI()
{
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(volSlider);
    addAndMakeVisible(volLabel);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);   
    addAndMakeVisible(posLabel);

    volLabel.setText("Volume", juce::dontSendNotification);
    volLabel.setJustificationType(juce::Justification::centred);
    
    posLabel.setText("Position", juce::dontSendNotification);
    posLabel.setJustificationType(juce::Justification::centred);
    
    // Set up volume slider
    volSlider.setRange(0.0, 1.0);
    volSlider.setValue(0.5);
    volSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    volSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    
    // Set up speed slider
    speedSlider.setRange(0.25, 4.0);
    speedSlider.setValue(1.0);
    speedSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    
    // Set up position slider
    posSlider.setRange(0.0, 1.0);
    posSlider.setValue(0.0);
    posSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    posSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
}

DeckGUI::~DeckGUI()
{
}

void DeckGUI::paint (juce::Graphics& g)
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
    g.setFont (juce::FontOptions (14.0f));
    g.drawText ("DeckGUI", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void DeckGUI::resized()
{
    double rowH = getHeight() / 8.0; 

    playButton.setBounds(10, 10, getWidth() / 4, rowH);
    stopButton.setBounds(10, rowH + 15, getWidth() / 4, rowH);
    loadButton.setBounds(10, rowH * 2 + 20, getWidth() / 4, rowH);
    
    volLabel.setBounds(getWidth() / 3, 0, getWidth() / 3, rowH);
    volSlider.setBounds(getWidth() / 3, rowH, getWidth() / 3, rowH);
    speedSlider.setBounds(getWidth() / 3, rowH * 2, getWidth() / 3, rowH);
    posLabel.setBounds(getWidth() / 3, rowH * 3, getWidth() / 3, rowH);
    posSlider.setBounds(getWidth() / 3, rowH * 4, getWidth() / 3, rowH);
}
