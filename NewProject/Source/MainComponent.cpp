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
    
    // Add buttons
    addAndMakeVisible(&playButton);
    addAndMakeVisible(&stopButton);
    addAndMakeVisible(&loadButton);
    
    // Set up button listeners
    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);
    
    // Add sliders
    addAndMakeVisible(&volSlider);
    addAndMakeVisible(&speedSlider);
    addAndMakeVisible(&posSlider);
    
    // Set up slider listeners
    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);
    
    // Set slider ranges
    volSlider.setRange(0.0, 1.0);
    volSlider.setValue(0.5);
    speedSlider.setRange(0.5, 2.0);
    speedSlider.setValue(1.0);
    posSlider.setRange(0.0, 1.0);
    posSlider.setValue(0.0);
    
    // Add labels
    addAndMakeVisible(&volLabel);
    addAndMakeVisible(&speedLabel);
    addAndMakeVisible(&posLabel);
    
    volLabel.setText("Volume", juce::dontSendNotification);
    volLabel.setJustificationType(juce::Justification::centredLeft);
    speedLabel.setText("Speed", juce::dontSendNotification);
    speedLabel.setJustificationType(juce::Justification::centredLeft);
    posLabel.setText("Position", juce::dontSendNotification);
    posLabel.setJustificationType(juce::Justification::centredLeft);
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
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Get audio from player1 (which now handles its own volume and speed)
    player1.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    player1.releaseResources();
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
    
    int y = margin;
    
    // Row 1: Buttons
    playButton.setBounds(margin, y, 100, buttonHeight);
    stopButton.setBounds(120, y, 100, buttonHeight);
    loadButton.setBounds(230, y, 100, buttonHeight);
    
    y += buttonHeight + margin;
    
    // Row 2: Volume
    volLabel.setBounds(margin, y, 100, labelHeight);
    y += labelHeight;
    volSlider.setBounds(margin, y, getWidth() - 2 * margin, sliderHeight);
    
    y += sliderHeight + margin;
    
    // Row 3: Speed
    speedLabel.setBounds(margin, y, 100, labelHeight);
    y += labelHeight;
    speedSlider.setBounds(margin, y, getWidth() - 2 * margin, sliderHeight);
    
    y += sliderHeight + margin;
    
    // Row 4: Position
    posLabel.setBounds(margin, y, 100, labelHeight);
    y += labelHeight;
    posSlider.setBounds(margin, y, getWidth() - 2 * margin, sliderHeight);
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
        auto fileChooser = std::make_unique<juce::FileChooser>("Select a Wave file to play...",
                                                               juce::File{},
                                                               "*.wav");
        auto fileChooserFlags = juce::FileBrowserComponent::canSelectFiles;
        
        fileChooser->launchAsync(fileChooserFlags,
                                [this](const juce::FileChooser& fc)
                                {
                                    auto file = fc.getResult();
                                    if (file != juce::File{})
                                    {
                                        player1.loadURL(juce::URL{file});
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
}