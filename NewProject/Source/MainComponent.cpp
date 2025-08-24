/*
  Author: Sam May
  Note: Entire file written by me as part of CM2005 coursework
*/

#include "MainComponent.h"

MainComponent::MainComponent()
{

    setSize (800, 600);
    
    // register audio formats for the formatManager
    formatManager.registerBasicFormats();

    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Input/output channels
        setAudioChannels (2, 2);
    }
    
    // adds the the two DeckGUI components
    addAndMakeVisible(deckGUI1);
    addAndMakeVisible(deckGUI2);

    addAndMakeVisible(playlistComponent);
    
    // Sets up crossfader for blending between decks
    crossfader.setRange(0.0, 1.0);
    crossfader.setValue(0.5); 
    crossfader.setSliderStyle(juce::Slider::LinearHorizontal);
    crossfader.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    crossfader.onValueChange = [this]() {
        updateCrossfaderMix();
    };
    
    // styles the crossfader
    crossfader.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(255, 107, 107));
    crossfader.setColour(juce::Slider::trackColourId, juce::Colour::fromRGB(60, 60, 70));
    crossfader.setColour(juce::Slider::backgroundColourId, juce::Colour::fromRGB(40, 40, 50));
    
    addAndMakeVisible(crossfader);
    
    // crossfader label
    crossfaderLabel.setText("CROSSFADER", juce::dontSendNotification);
    crossfaderLabel.setJustificationType(juce::Justification::centredRight);
    crossfaderLabel.setColour(juce::Label::textColourId, juce::Colour::fromRGB(200, 200, 210));
    crossfaderLabel.setFont(juce::Font(10.0f, juce::Font::bold));
    addAndMakeVisible(crossfaderLabel);
    
    // setup master volume slider
    masterVolume.setRange(0.0, 1.0);
    masterVolume.setValue(0.8); // default to 80%
    masterVolume.setSliderStyle(juce::Slider::LinearHorizontal);
    masterVolume.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    
    // Styles the master volume slider
    masterVolume.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(107, 255, 107));
    masterVolume.setColour(juce::Slider::trackColourId, juce::Colour::fromRGB(60, 70, 60));
    masterVolume.setColour(juce::Slider::backgroundColourId, juce::Colour::fromRGB(40, 50, 40));
    
    addAndMakeVisible(masterVolume);
    
    // master volume label
    masterVolumeLabel.setText("MASTER", juce::dontSendNotification);
    masterVolumeLabel.setJustificationType(juce::Justification::centredRight);
    masterVolumeLabel.setColour(juce::Label::textColourId, juce::Colour::fromRGB(200, 210, 200));
    masterVolumeLabel.setFont(juce::Font(10.0f, juce::Font::bold));
    addAndMakeVisible(masterVolumeLabel);
    
    // setup master filter slider 
    masterFilter.setRange(0.0, 1.0);
    masterFilter.setValue(0.5); 
    masterFilter.setSliderStyle(juce::Slider::LinearHorizontal);
    masterFilter.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    
    // style the master filter slider
    masterFilter.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(255, 159, 67)); // Orange
    masterFilter.setColour(juce::Slider::trackColourId, juce::Colour::fromRGB(70, 60, 60));
    masterFilter.setColour(juce::Slider::backgroundColourId, juce::Colour::fromRGB(50, 40, 40));
    
    addAndMakeVisible(masterFilter);
    
    // master filter label
    masterFilterLabel.setText("FILTER", juce::dontSendNotification);
    masterFilterLabel.setJustificationType(juce::Justification::centredRight);
    masterFilterLabel.setColour(juce::Label::textColourId, juce::Colour::fromRGB(255, 180, 120));
    masterFilterLabel.setFont(juce::Font(10.0f, juce::Font::bold));
    addAndMakeVisible(masterFilterLabel);

    // connects the playlist to decks for track loading
    playlistComponent.onTrackLoadRequest = [this](int deckIndex, const juce::File& audioFile)
    {
        if (deckIndex == 0)
            deckGUI1.loadTrack(audioFile);
        else if (deckIndex == 1)
            deckGUI2.loadTrack(audioFile);
    };
    
    // initialize crossfader mixing
    updateCrossfaderMix();
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    this->sampleRate = sampleRate;
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // creates temporary buffers for each deck
    juce::AudioBuffer<float> deck1Buffer(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
    juce::AudioBuffer<float> deck2Buffer(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
    
    // gets the audio from each of the decks
    juce::AudioSourceChannelInfo deck1Info(&deck1Buffer, 0, bufferToFill.numSamples);
    juce::AudioSourceChannelInfo deck2Info(&deck2Buffer, 0, bufferToFill.numSamples);
    
    player1.getNextAudioBlock(deck1Info);
    player2.getNextAudioBlock(deck2Info);
    
    // clears the output buffer
    bufferToFill.clearActiveBufferRegion();
    
    // mix with crossfader gains and apply master filter
    double filterValue = masterFilter.getValue();
    
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        auto* outputData = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
        const auto* deck1Data = deck1Buffer.getReadPointer(channel);
        const auto* deck2Data = deck2Buffer.getReadPointer(channel);
        
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            float mixedSample = (deck1Data[sample] * crossfaderLeftGain) + 
                               (deck2Data[sample] * crossfaderRightGain);
            
            // Apply master filter effect
            if (filterValue < 0.5)
            {
                // Low-pass filter (cut highs) - left side of slider
                float cutoffRatio = filterValue * 2.0f; 
                float filterFactor = 0.1f + (cutoffRatio * 0.9f); 
                mixedSample *= filterFactor;
            }
            else if (filterValue > 0.5)
            {
                // High-pass filter (cut lows) - right side of slider
                float cutoffRatio = (filterValue - 0.5f) * 2.0f; 
                float filterFactor = 1.0f - (cutoffRatio * 0.7f); 
                mixedSample = mixedSample * filterFactor + (mixedSample * 0.3f * cutoffRatio);
            }
            
            // Master volume
            outputData[sample] = mixedSample * static_cast<float>(masterVolume.getValue());
        }
    }
}

void MainComponent::releaseResources()
{
    player1.releaseResources();
    player2.releaseResources();
}

void MainComponent::paint (juce::Graphics& g)
{
    // dark gradient background
    juce::ColourGradient gradient(juce::Colour::fromRGB(18, 18, 18), 0, 0,
                                  juce::Colour::fromRGB(28, 28, 28), 0, getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    // draws the crossfader background panel
    auto bounds = getLocalBounds();
    bounds.reduce(8, 8);
    auto playlistHeight = getHeight() * 0.3;
    auto crossfaderY = bounds.getBottom() - playlistHeight - 43;
    
    juce::Rectangle<int> crossfaderPanel(bounds.getX() + (bounds.getWidth() - 720) / 2, 
                                        crossfaderY, 
                                        720, 
                                        35);
    
    g.setColour(juce::Colour::fromRGB(35, 35, 45).withAlpha(0.8f));
    g.fillRoundedRectangle(crossfaderPanel.toFloat(), 8.0f);
    
    g.setColour(juce::Colour::fromRGB(70, 70, 80).withAlpha(0.4f));
    g.drawRoundedRectangle(crossfaderPanel.toFloat(), 8.0f, 1.0f);
}

void MainComponent::resized()
{
    auto area = getLocalBounds();
    
    // add some padding around the entire layout
    area.reduce(8, 8);
    
    // Bottom area for playlist 
    auto playlistArea = area.removeFromBottom(getHeight() * 0.3);
    playlistComponent.setBounds(playlistArea);
    
    // Add small gap before crossfader
    area.removeFromBottom(8);
    
    // Crossfader area between decks and playlist
    auto crossfaderArea = area.removeFromBottom(35);
    auto crossfaderPanelArea = crossfaderArea.withSizeKeepingCentre(720, 25);
    
    // Crossfader label inside the panel on the left
    auto labelArea = crossfaderPanelArea.removeFromLeft(80);
    labelArea.removeFromTop(2); 
    crossfaderLabel.setBounds(labelArea);
    
    // small gap between label and slider
    crossfaderPanelArea.removeFromLeft(10);
    
    // crossfader slider 
    crossfader.setBounds(crossfaderPanelArea.removeFromLeft(160));
    
    // gap between sliders
    crossfaderPanelArea.removeFromLeft(15);
    
    // master volume label
    auto masterLabelArea = crossfaderPanelArea.removeFromLeft(60);
    masterLabelArea.removeFromTop(2);
    masterVolumeLabel.setBounds(masterLabelArea);
    
    // small gap
    crossfaderPanelArea.removeFromLeft(10);
    
    // master volume slider
    masterVolume.setBounds(crossfaderPanelArea.removeFromLeft(120));
    
    // gap between sliders
    crossfaderPanelArea.removeFromLeft(15);
    
    // master filter label
    auto filterLabelArea = crossfaderPanelArea.removeFromLeft(60);
    filterLabelArea.removeFromTop(2);
    masterFilterLabel.setBounds(filterLabelArea);
    
    // Small gap
    crossfaderPanelArea.removeFromLeft(10);
    
    // Master filter slider in remaining space
    masterFilter.setBounds(crossfaderPanelArea);
    
    // Add gap between crossfader and decks
    area.removeFromBottom(8);
    
    // splits the remaining area equally between the two decks
    auto deckWidth = (area.getWidth() - 10) / 2;
    auto leftDeckArea = area.removeFromLeft(deckWidth);
    area.removeFromLeft(10); // small gap between decks
    auto rightDeckArea = area;
    
    // set deck positions
    deckGUI1.setBounds(leftDeckArea);
    deckGUI2.setBounds(rightDeckArea);
}

void MainComponent::updateCrossfaderMix()
{
    // crossfader value: 0.0 = full left (deck1), 1.0 = full right (deck2)
    double crossfaderPos = crossfader.getValue();
    
    // calculate gain for each deck using crossfading curve
    crossfaderLeftGain = std::cos(crossfaderPos * juce::MathConstants<double>::halfPi);
    crossfaderRightGain = std::sin(crossfaderPos * juce::MathConstants<double>::halfPi);
}