#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
    
    // Add deck 1 buttons
    addAndMakeVisible(&playButton);
    addAndMakeVisible(&stopButton);
    addAndMakeVisible(&loadButton);
    
    // Set up deck 1 button listeners
    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);
    
    // Add deck 1 sliders
    addAndMakeVisible(&volSlider);
    addAndMakeVisible(&speedSlider);
    addAndMakeVisible(&posSlider);
    
    // Set up deck 1 slider listeners
    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);
    
    // Set deck 1 slider ranges
    volSlider.setRange(0.0, 1.0);
    volSlider.setValue(0.5);
    speedSlider.setRange(0.5, 2.0);
    speedSlider.setValue(1.0);
    posSlider.setRange(0.0, 1.0);
    posSlider.setValue(0.0);
    
    // Add deck 1 labels
    addAndMakeVisible(&volLabel);
    addAndMakeVisible(&speedLabel);
    addAndMakeVisible(&posLabel);
    
    volLabel.setText("Volume", juce::dontSendNotification);
    volLabel.setJustificationType(juce::Justification::centredLeft);
    speedLabel.setText("Speed", juce::dontSendNotification);
    speedLabel.setJustificationType(juce::Justification::centredLeft);
    posLabel.setText("Position", juce::dontSendNotification);
    posLabel.setJustificationType(juce::Justification::centredLeft);
    
    // Add deck 2 buttons
    addAndMakeVisible(&playButton2);
    addAndMakeVisible(&stopButton2);
    addAndMakeVisible(&loadButton2);
    
    // Set up deck 2 button listeners
    playButton2.addListener(this);
    stopButton2.addListener(this);
    loadButton2.addListener(this);
    
    // Add deck 2 sliders
    addAndMakeVisible(&volSlider2);
    addAndMakeVisible(&speedSlider2);
    addAndMakeVisible(&posSlider2);
    
    // Set up deck 2 slider listeners
    volSlider2.addListener(this);
    speedSlider2.addListener(this);
    posSlider2.addListener(this);
    
    // Set deck 2 slider ranges
    volSlider2.setRange(0.0, 1.0);
    volSlider2.setValue(0.5);
    speedSlider2.setRange(0.5, 2.0);
    speedSlider2.setValue(1.0);
    posSlider2.setRange(0.0, 1.0);
    posSlider2.setValue(0.0);
    
    // Add deck 2 labels
    addAndMakeVisible(&volLabel2);
    addAndMakeVisible(&speedLabel2);
    addAndMakeVisible(&posLabel2);
    
    volLabel2.setText("Volume", juce::dontSendNotification);
    volLabel2.setJustificationType(juce::Justification::centredLeft);
    speedLabel2.setText("Speed", juce::dontSendNotification);
    speedLabel2.setJustificationType(juce::Justification::centredLeft);
    posLabel2.setText("Position", juce::dontSendNotification);
    posLabel2.setJustificationType(juce::Justification::centredLeft);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Clear the buffer first
    bufferToFill.clearActiveBufferRegion();
    
    // Get audio from player1 directly into the main buffer
    player1.getNextAudioBlock(bufferToFill);
    
    // Create a temporary buffer for player2
    juce::AudioBuffer<float> tempBuffer(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
    juce::AudioSourceChannelInfo tempInfo(&tempBuffer, 0, bufferToFill.numSamples);
    
    // Get audio from player2 into temporary buffer
    player2.getNextAudioBlock(tempInfo);
    
    // Mix player2's audio into the main buffer
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        bufferToFill.buffer->addFrom(channel, bufferToFill.startSample,
                                   tempBuffer, channel, 0,
                                   bufferToFill.numSamples);
    }
}

void MainComponent::releaseResources()
{
    player1.releaseResources();
    player2.releaseResources();
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    const int buttonHeight = 40;
    const int sliderHeight = 30;
    const int labelHeight = 20;
    const int margin = 10;
    
    // Split the window in half vertically
    const int halfWidth = getWidth() / 2;
    const int leftSideWidth = halfWidth - margin;
    const int rightSideWidth = halfWidth - margin;
    
    int y = margin;
    
    // Left side: Deck 1 controls
    // Row 1: Buttons
    playButton.setBounds(margin, y, 100, buttonHeight);
    stopButton.setBounds(120, y, 100, buttonHeight);
    loadButton.setBounds(230, y, 100, buttonHeight);
    
    y += buttonHeight + margin;
    
    // Row 2: Volume
    volLabel.setBounds(margin, y, 100, labelHeight);
    y += labelHeight;
    volSlider.setBounds(margin, y, leftSideWidth, sliderHeight);
    
    y += sliderHeight + margin;
    
    // Row 3: Speed
    speedLabel.setBounds(margin, y, 100, labelHeight);
    y += labelHeight;
    speedSlider.setBounds(margin, y, leftSideWidth, sliderHeight);
    
    y += sliderHeight + margin;
    
    // Row 4: Position
    posLabel.setBounds(margin, y, 100, labelHeight);
    y += labelHeight;
    posSlider.setBounds(margin, y, leftSideWidth, sliderHeight);
    
    // Right side: Deck 2 controls
    const int rightStartX = halfWidth + margin;
    y = margin; // Reset y position for right side
    
    // Row 1: Buttons
    playButton2.setBounds(rightStartX, y, 100, buttonHeight);
    stopButton2.setBounds(rightStartX + 110, y, 100, buttonHeight);
    loadButton2.setBounds(rightStartX + 220, y, 100, buttonHeight);
    
    y += buttonHeight + margin;
    
    // Row 2: Volume
    volLabel2.setBounds(rightStartX, y, 100, labelHeight);
    y += labelHeight;
    volSlider2.setBounds(rightStartX, y, rightSideWidth, sliderHeight);
    
    y += sliderHeight + margin;
    
    // Row 3: Speed
    speedLabel2.setBounds(rightStartX, y, 100, labelHeight);
    y += labelHeight;
    speedSlider2.setBounds(rightStartX, y, rightSideWidth, sliderHeight);
    
    y += sliderHeight + margin;
    
    // Row 4: Position
    posLabel2.setBounds(rightStartX, y, 100, labelHeight);
    y += labelHeight;
    posSlider2.setBounds(rightStartX, y, rightSideWidth, sliderHeight);
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &playButton)
    {
        player1.start();
    }
    else if (button == &stopButton)
    {
        player1.stop();
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
                                        std::cout << "Loading file to deck 1: " << file.getFullPathName().toStdString() << std::endl;
                                        player1.loadURL(juce::URL{file});
                                    }
                                });
    }
    else if (button == &playButton2)
    {
        player2.start();
    }
    else if (button == &stopButton2)
    {
        player2.stop();
    }
    else if (button == &loadButton2)
    {
        fileChooser2 = std::make_unique<juce::FileChooser>("Select an audio file to play...",
                                                           juce::File::getSpecialLocation(juce::File::userHomeDirectory),
                                                           "*.wav;*.mp3;*.m4a;*.aac;*.flac;*.ogg;*.aif;*.aiff");
        
        fileChooser2->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                                 [this](const juce::FileChooser& fc)
                                 {
                                     auto file = fc.getResult();
                                     if (file.existsAsFile())
                                     {
                                         std::cout << "Loading file to deck 2: " << file.getFullPathName().toStdString() << std::endl;
                                         player2.loadURL(juce::URL{file});
                                     }
                                 });
    }
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volSlider)
    {
        player1.setGain(volSlider.getValue());
    }
    else if (slider == &speedSlider)
    {
        player1.setSpeed(speedSlider.getValue());
    }
    else if (slider == &posSlider)
    {
        player1.setPositionRelative(posSlider.getValue());
    }
    else if (slider == &volSlider2)
    {
        player2.setGain(volSlider2.getValue());
    }
    else if (slider == &speedSlider2)
    {
        player2.setSpeed(speedSlider2.getValue());
    }
    else if (slider == &posSlider2)
    {
        player2.setPositionRelative(posSlider2.getValue());
    }
}