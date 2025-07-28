#include <JuceHeader.h>
#include "DeckGUI.h"

//==============================================================================
DeckGUI::DeckGUI(DJAudioPlayer* _player, 
                  juce::AudioFormatManager & formatManagerToUse,
                  juce::AudioThumbnailCache & thumbnailCacheToUse)
                  : player(_player), waveformDisplay(formatManagerToUse, thumbnailCacheToUse)
{
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(volSlider);
    addAndMakeVisible(volLabel);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);   
    addAndMakeVisible(posLabel);
    addAndMakeVisible(waveformDisplay);

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
    
    // Add listeners
    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);
    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);
}

DeckGUI::~DeckGUI()
{
}

void DeckGUI::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);
    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));
    g.drawText ("DeckGUI", getLocalBounds(),
                juce::Justification::centred, true);
}

void DeckGUI::resized()
{
    double rowH = getHeight() / 8.0; 
    
    // Top row - buttons side by side
    playButton.setBounds(10, 10, getWidth() / 4, rowH);
    stopButton.setBounds(getWidth() / 4 + 20, 10, getWidth() / 4, rowH);
    loadButton.setBounds(getWidth() / 2 + 30, 10, getWidth() / 4, rowH);
    
    // Sliders - full width of section
    volLabel.setBounds(10, rowH + 20, getWidth() - 20, 20);
    volSlider.setBounds(10, rowH + 40, getWidth() - 20, rowH);
    
    speedSlider.setBounds(10, rowH * 2 + 60, getWidth() - 20, rowH);
    
    posLabel.setBounds(10, rowH * 3 + 80, getWidth() - 20, 20);
    posSlider.setBounds(10, rowH * 3 + 100, getWidth() - 20, rowH);
    
    // Waveform - bottom half
    waveformDisplay.setBounds(10, rowH * 5, getWidth() - 20, getHeight() - rowH * 5 - 10);
}

void DeckGUI::buttonClicked(juce::Button* button)
{
    if (button == &playButton)
    {
        player->start();
    }
    else if (button == &stopButton)
    {
        player->stop();
    }
    else if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>("Select an audio file to play...",
                                                          juce::File::getSpecialLocation(juce::File::userHomeDirectory),
                                                          "*.wav;*.mp3;*.m4a;*.aac;*.flac;*.ogg;*.aif;*.aiff");
        
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                                [this](const juce::FileChooser& fc)
                                {
                                    auto file = fc.getResult();
                                    if (file.existsAsFile())
                                    {
                                        player->loadURL(juce::URL{file});
                                        waveformDisplay.loadURL(juce::URL{file});
                                    }
                                });
    }
}

void DeckGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volSlider)
    {
        player->setGain(slider->getValue());
    }
    else if (slider == &speedSlider)
    {
        player->setSpeed(slider->getValue());
    }
    else if (slider == &posSlider)
    {
        player->setPositionRelative(slider->getValue());
    }
}

  bool DeckGUI::isInterestedInFileDrag (const juce::StringArray& files)
  {
    std::cout << "DeckGUI:isInterestedInFileDrag" << std::endl;
    return true; // We are interested in file drag events
  }
  void DeckGUI::filesDropped (const juce::StringArray& files, int x, int y)
  {
        std::cout << "DeckGUI:filesDropped" << std::endl;
        if (files.size() == 1)
        {
          player->loadURL(juce::URL{juce::File{files[0]}});
        }
  }  