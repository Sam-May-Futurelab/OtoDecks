#include <JuceHeader.h>
#include "DeckGUI.h"
#include "PlaylistComponent.h"

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
    addAndMakeVisible(speedLabel);
    addAndMakeVisible(posSlider);   
    addAndMakeVisible(posLabel);
    // Adds track name display
    addAndMakeVisible(trackNameLabel);
     // Adds BPM display
    addAndMakeVisible(bpmLabel); 
    addAndMakeVisible(waveformDisplay);
    
    // Sets up waveform interaction callback
    waveformDisplay.onPositionChange = [this](double position) {
        if (player != nullptr) {
            player->setPositionRelative(position);
        }
    };

    volLabel.setText("Volume", juce::dontSendNotification);
    volLabel.setJustificationType(juce::Justification::centredLeft);
    
    speedLabel.setText("Speed", juce::dontSendNotification);
    speedLabel.setJustificationType(juce::Justification::centredLeft);
    
posLabel.setText("Position", juce::dontSendNotification);
    posLabel.setJustificationType(juce::Justification::centredLeft);
    
    // Sets up track name label - shows currently loaded track
    trackNameLabel.setText("No Track Loaded", juce::dontSendNotification);
    trackNameLabel.setJustificationType(juce::Justification::centred);
    trackNameLabel.setColour(juce::Label::backgroundColourId, juce::Colour::fromRGB(25, 25, 30));
    trackNameLabel.setColour(juce::Label::outlineColourId, juce::Colour::fromRGB(64, 224, 208).withAlpha(0.6f));
    
    // set up the BPM label & shows bpm of current track
    bpmLabel.setText("BPM: ---", juce::dontSendNotification);
    bpmLabel.setJustificationType(juce::Justification::centred);
    bpmLabel.setColour(juce::Label::backgroundColourId, juce::Colour::fromRGB(25, 25, 30));
    bpmLabel.setColour(juce::Label::outlineColourId, juce::Colour::fromRGB(255, 159, 67).withAlpha(0.6f)); // Orange border
    bpmLabel.setColour(juce::Label::textColourId, juce::Colour::fromRGB(255, 159, 67)); // orange text
    
    // set up volume slider
    volSlider.setRange(0.0, 1.0);
    volSlider.setValue(0.5);
    volSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    volSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    
    // set up speed slider 
    speedSlider.setRange(0.25, 4.0);
    speedSlider.setValue(1.0);
    speedSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    
    // set up position slider 
    posSlider.setRange(0.0, 1.0);
    posSlider.setValue(0.0);
    posSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    posSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    
    // add listeners
    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);
    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);
    
    // style the buttons with colors
    styleButton(playButton, juce::Colour::fromRGB(46, 213, 115)); // Green
    styleButton(stopButton, juce::Colour::fromRGB(255, 107, 107)); // coral
    styleButton(loadButton, juce::Colour::fromRGB(116, 185, 255)); // Blue
    
    // Style the sliders with coordinated colors
    styleSlider(volSlider, juce::Colour::fromRGB(116, 185, 255));  // Blue
    styleSlider(speedSlider, juce::Colour::fromRGB(255, 159, 67)); // Orange
    styleSlider(posSlider, juce::Colour::fromRGB(85, 239, 196));   // Mint
    
    // Style labels with softer white
    volLabel.setColour(juce::Label::textColourId, juce::Colour::fromRGB(220, 220, 225));
    volLabel.setFont(juce::Font(11.0f, juce::Font::bold));
    speedLabel.setColour(juce::Label::textColourId, juce::Colour::fromRGB(220, 220, 225));
    speedLabel.setFont(juce::Font(11.0f, juce::Font::bold));
    posLabel.setColour(juce::Label::textColourId, juce::Colour::fromRGB(220, 220, 225));
    posLabel.setFont(juce::Font(11.0f, juce::Font::bold));
    
    // Style track name label
    trackNameLabel.setColour(juce::Label::textColourId, juce::Colour::fromRGB(64, 224, 208));
    trackNameLabel.setFont(juce::Font(13.0f, juce::Font::bold));
    
    // styles the BPM label
    bpmLabel.setFont(juce::Font(12.0f, juce::Font::bold));

    startTimer(50);
}

DeckGUI::~DeckGUI()
{
    stopTimer();
}

void DeckGUI::paint (juce::Graphics& g)
{
    // Deck panel with the gradient background
    juce::ColourGradient deckGradient(juce::Colour::fromRGB(45, 45, 50), 0, 0,
                                      juce::Colour::fromRGB(35, 35, 40), 0, getHeight(), false);
    g.setGradientFill(deckGradient);
    g.fillAll();
    
    // Accent border
    g.setColour(juce::Colour::fromRGB(64, 224, 208).withAlpha(0.4f)); // Teal colour
    g.drawRect(getLocalBounds(), 1);
    
    // Inner highlight for depth
    g.setColour(juce::Colour::fromRGB(55, 55, 60).withAlpha(0.8f));
    g.drawRect(getLocalBounds().reduced(1), 1);
    
    // Deck label
    g.setColour(juce::Colour::fromRGB(220, 220, 225));
    g.setFont(juce::Font(12.0f, juce::Font::bold));
    g.drawText("DECK", getLocalBounds().removeFromTop(22).reduced(8, 3), 
               juce::Justification::topLeft, true);
}

void DeckGUI::resized()
{
    // Layout with inline labels for space efficiency
    auto area = getLocalBounds().reduced(5); 
    
    // Top row - buttons (fixed height for consistency)
    auto buttonArea = area.removeFromTop(50);
    int buttonWidth = (buttonArea.getWidth() - 20) / 3; 
    playButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
    buttonArea.removeFromLeft(10);
    stopButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
    buttonArea.removeFromLeft(10);
    loadButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
    
    area.removeFromTop(8); // spacing after buttons

    // Track name display - (shows what's currently loaded)
    auto trackNameArea = area.removeFromTop(30);
    trackNameLabel.setBounds(trackNameArea.reduced(5, 2));
    
    area.removeFromTop(3); // spacing after the played track name

    // BPM display - (shows the BPM of current track)
    auto bpmArea = area.removeFromTop(25);
    bpmLabel.setBounds(bpmArea.reduced(5, 2));
    
    area.removeFromTop(5); // spacing after BPM
    
    // Waveform
    auto waveformArea = area.removeFromBottom(120); 
    waveformDisplay.setBounds(waveformArea);
    
    area.removeFromBottom(8); // gap between controls and waveform
    
    // Control section - inline labels with sliders
    int controlHeight = area.getHeight() / 3; 
    
    // Volume control row
    auto volArea = area.removeFromTop(controlHeight);
    volLabel.setBounds(volArea.removeFromLeft(60)); 
    volSlider.setBounds(volArea.reduced(5, 8)); 
    
    // Speed control row  
    auto speedArea = area.removeFromTop(controlHeight);
    speedLabel.setBounds(speedArea.removeFromLeft(60));
    speedSlider.setBounds(speedArea.reduced(5, 8)); 
    
    // Position control row
    auto posArea = area.removeFromTop(controlHeight);
    posLabel.setBounds(posArea.removeFromLeft(60)); 
    posSlider.setBounds(posArea.reduced(5, 8)); 
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
                                        // Updates the track name display
                                        trackNameLabel.setText(file.getFileNameWithoutExtension(), juce::dontSendNotification);
                                        // Resets the BPM display
                                        bpmLabel.setText("BPM: Analysing...", juce::dontSendNotification);
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
    return true; // We are interested in file drag events
  }
  void DeckGUI::filesDropped (const juce::StringArray& files, int x, int y)
  {
        if (files.size() == 1)
        {
          // Load the track from either file browser or playlist drag
          juce::File audioFile(files[0]);
          if (audioFile.existsAsFile())
          {
              player->loadURL(juce::URL{audioFile});
              waveformDisplay.loadURL(juce::URL{audioFile});
              // Update track name display
              trackNameLabel.setText(audioFile.getFileNameWithoutExtension(), juce::dontSendNotification);
              // Reset BPM display
              bpmLabel.setText("BPM: Analysing...", juce::dontSendNotification);
          }
        }
  }

  void DeckGUI::timerCallback()
  {
    waveformDisplay.setPositionRelative(player->getPositionRelative());
    
    // Update BPM display
    if (player->isBPMAnalysisComplete())
    {
        double currentBPM = player->getBPM();
        double speedRatio = player->getCurrentSpeed();
        
        if (currentBPM > 0.0)
        {
            // Show current BPM, and indicate if speed has been adjusted
            if (std::abs(speedRatio - 1.0) > 0.01) // Speed is significantly different from 1.0
            {
                // Show both current BPM and speed percentage
                juce::String speedPercent = juce::String((speedRatio - 1.0) * 100.0, 0);
                if (speedRatio > 1.0)
                    speedPercent = "+" + speedPercent;
                
                bpmLabel.setText("BPM: " + juce::String(currentBPM, 1) + " (" + speedPercent + "%)", 
                                juce::dontSendNotification);
            }
            else
            {
                // Normal speed, just show BPM
                bpmLabel.setText("BPM: " + juce::String(currentBPM, 1), juce::dontSendNotification);
            }
        }
        else
        {
            bpmLabel.setText("BPM: ---", juce::dontSendNotification);
        }
    }
    else
    {
        bpmLabel.setText("BPM: Analysing...", juce::dontSendNotification);
    }
  }

void DeckGUI::styleButton(juce::TextButton& button, juce::Colour accentColor)
{
    // Button styling
    button.setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(55, 55, 65));
    button.setColour(juce::TextButton::buttonOnColourId, accentColor.withAlpha(0.9f));
    button.setColour(juce::TextButton::textColourOffId, juce::Colour::fromRGB(200, 200, 210));
    button.setColour(juce::TextButton::textColourOnId, juce::Colour::fromRGB(25, 25, 30));
}

void DeckGUI::styleSlider(juce::Slider& slider, juce::Colour accentColor)
{
    // Slider styling
    slider.setColour(juce::Slider::trackColourId, juce::Colour::fromRGB(65, 65, 75));
    slider.setColour(juce::Slider::thumbColourId, accentColor);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour::fromRGB(220, 220, 225));
    slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour::fromRGB(40, 40, 50));
    slider.setColour(juce::Slider::textBoxOutlineColourId, accentColor.withAlpha(0.4f));
}

void DeckGUI::loadTrack(const juce::File& audioFile)
{
    if (audioFile.existsAsFile())
    {
        player->loadURL(juce::URL{audioFile});
        waveformDisplay.loadURL(juce::URL{audioFile});
        
        // Update track name display
        trackNameLabel.setText(audioFile.getFileNameWithoutExtension(), juce::dontSendNotification);
        
        // Reset BPM display
        bpmLabel.setText("BPM: Analysing...", juce::dontSendNotification);
    }
}