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
    
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(volSlider);
    addAndMakeVisible(volLabel);
    addAndMakeVisible(speedSlider);

    volLabel.setText("Volume", juce::dontSendNotification);
    volLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(trackSelector);

    trackSelector.addItem("Track 1", 1);
    trackSelector.addItem("Track 2", 2);
    trackSelector.addItem("Track 3", 3);
    trackSelector.setSelectedId(1);

    addAndMakeVisible(trackLabel);
    trackLabel.setText("Track", juce::dontSendNotification);
    trackLabel.setJustificationType(juce::Justification::centred);
    trackSelector.onChange = [this]() {
        // Track selection changed - could add different functionality here
    };
    
    // Set up volume slider
    volSlider.setRange(0.0, 1.0);
    volSlider.setValue(0.5);
    volSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    volSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    
    // Set up speed slider (resampling ratio)
    speedSlider.setRange(0.25, 4.0); // Quarter speed to 4x speed
    speedSlider.setValue(1.0); // Normal speed
    speedSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);

    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);
    volSlider.addListener(this);
    speedSlider.addListener(this);
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
    int rowH = getHeight() / 8; // Divide window into 8 rows
    
    // Simple row-based layout
    playButton.setBounds(10, 10, getWidth() / 4, rowH);
    stopButton.setBounds(10, rowH + 15, getWidth() / 4, rowH);
    loadButton.setBounds(10, rowH * 2 + 20, getWidth() / 4, rowH);
    
    volLabel.setBounds(getWidth() / 3, 0, getWidth() / 3, rowH);
    volSlider.setBounds(getWidth() / 3, rowH, getWidth() / 3, rowH);
    speedSlider.setBounds(getWidth() / 3, rowH * 2, getWidth() / 3, rowH);
    
    trackLabel.setBounds(getWidth() - getWidth() / 4 - 10, 10, getWidth() / 4, rowH);
    trackSelector.setBounds(getWidth() - getWidth() / 4 - 10, rowH + 15, getWidth() / 4, rowH);
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
        // Use async file chooser as required in JUCE 8
        auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
        
        auto* chooser = new juce::FileChooser("Select an audio file...", 
                                            juce::File::getSpecialLocation(juce::File::userMusicDirectory), 
                                            "*.mp3;*.wav;*.m4a;*.aiff");
        
        chooser->launchAsync(chooserFlags, [this, chooser](const juce::FileChooser&)
        {
            auto file = chooser->getResult();
            if (file.existsAsFile())
            {
                player1.loadURL(juce::URL{file});
            }
            delete chooser;
        });
    }
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volSlider)
    {
        player1.setGain(slider->getValue());
    }
    else if (slider == &speedSlider)
    {
        player1.setSpeed(slider->getValue());
    }
}