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
    addAndMakeVisible(volSlider);
    addAndMakeVisible(volLabel);
    volLabel.setText("Volume", juce::dontSendNotification);
    volLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(trackSelector);
    trackSelector.addItem("Track 1", 1);
    trackSelector.addItem("Track 2", 2);
    trackSelector.addItem("Track 3", 3);
    trackSelector.setSelectedId(1); // Default to "Track 1"
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
    
    // Initialize string displacements
    stringDisplacements.resize(6, 0.0f);
    stringsPluckedInDrag.resize(6, false);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // Create 6 strings with different frequencies (like a guitar)
    strings.clear();
    strings.push_back(std::make_unique<StringSynthesiser>(sampleRate, 82.4)); // E2
    strings.push_back(std::make_unique<StringSynthesiser>(sampleRate, 110.0)); // A2
    strings.push_back(std::make_unique<StringSynthesiser>(sampleRate, 146.8)); // D3
    strings.push_back(std::make_unique<StringSynthesiser>(sampleRate, 196.0)); // G3
    strings.push_back(std::make_unique<StringSynthesiser>(sampleRate, 246.9)); // B3
    strings.push_back(std::make_unique<StringSynthesiser>(sampleRate, 329.6)); // E4
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    
    // Generate string sounds
    for (auto& string : strings)
    {
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            float stringOutput = string->getNextSample();
            
            for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
            {
                bufferToFill.buffer->addSample(channel, bufferToFill.startSample + sample, 
                                              stringOutput * (float)volSlider.getValue() * 0.3f);
            }
        }
    }
}

void MainComponent::releaseResources()
{
    strings.clear();
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    // Draw strings
    for (int i = 0; i < stringAreas.size(); ++i)
    {
        auto& area = stringAreas[i];
        
        // Different colors for each string
        juce::Colour stringColor = juce::Colour::fromHSV(i / 6.0f, 0.7f, 0.9f, 1.0f);
        g.setColour(stringColor);
        
        // Draw string with displacement (like being pulled)
        auto centerY = area.getCentreY();
        auto displacement = stringDisplacements[i];
        
        // Draw curved string when displaced
        if (std::abs(displacement) > 1.0f)
        {
            juce::Path stringPath;
            stringPath.startNewSubPath(area.getX(), centerY);
            
            // Create curve with displacement at the center
            auto midX = area.getCentreX();
            stringPath.quadraticTo(midX, centerY + displacement, area.getRight(), centerY);
            
            g.strokePath(stringPath, juce::PathStrokeType(3.0f));
        }
        else
        {
            // Draw straight string
            g.drawLine(area.getX(), centerY, area.getRight(), centerY, 3.0f);
        }
    }
    
    // Draw instruction text at better position
    g.setColour(juce::Colours::white);
    g.setFont(16.0f);
    g.drawText("Drag across the strings like a guitar and release!", 
               0, 105, getWidth(), 30, juce::Justification::centred);
}

void MainComponent::resized()
{
    // Improved layout with better spacing
    int topMargin = 20;
    int controlHeight = 40;
    int labelHeight = 20;
    int buttonWidth = 100;
    int sliderWidth = 200;
    int dropdownWidth = 120;
    
    // Play button on the left
    playButton.setBounds(20, topMargin + labelHeight + 5, buttonWidth, controlHeight);
    
    // Volume section centered
    int centerX = getWidth() / 2;
    volLabel.setBounds(centerX - sliderWidth/2, topMargin, sliderWidth, labelHeight);
    volSlider.setBounds(centerX - sliderWidth/2, topMargin + labelHeight + 5, sliderWidth, controlHeight);
    
    // Track section on the right
    int rightX = getWidth() - dropdownWidth - 20;
    trackLabel.setBounds(rightX, topMargin, dropdownWidth, labelHeight);
    trackSelector.setBounds(rightX, topMargin + labelHeight + 5, dropdownWidth, controlHeight);
    
    // More space between controls and strings
    int stringStartY = topMargin + labelHeight + controlHeight + 90; // Extra space for instructions
    int stringHeight = 25;
    int stringSpacing = 55; // More space between strings
    int stringWidth = getWidth() - 100; // Margins on both sides
    
    // Layout string areas with better spacing
    stringAreas.clear();
    for (int i = 0; i < 6; ++i)
    {
        stringAreas.push_back(juce::Rectangle<int>(50, stringStartY + i * stringSpacing, stringWidth, stringHeight));
    }
    
    // Ensure our tracking vectors match the number of strings
    stringsPluckedInDrag.resize(stringAreas.size(), false);
}

void MainComponent::mouseDown (const juce::MouseEvent& e)
{
    isDragging = true;
    // Reset all strings plucked tracking
    std::fill(stringsPluckedInDrag.begin(), stringsPluckedInDrag.end(), false);
    
    // Check if we started on any string and pluck it immediately
    for (int i = 0; i < stringAreas.size(); ++i)
    {
        if (stringAreas[i].contains(e.getPosition()))
        {
            if (!stringsPluckedInDrag[i]) // Only pluck if not already plucked
            {
                strings[i]->pluck();
                stringsPluckedInDrag[i] = true;
            }
            break;
        }
    }
}

void MainComponent::mouseDrag (const juce::MouseEvent& e)
{
    if (!isDragging) return;
    
    // Check which string the mouse is currently over and pluck it if not already plucked
    for (int i = 0; i < stringAreas.size(); ++i)
    {
        if (stringAreas[i].contains(e.getPosition()))
        {
            if (!stringsPluckedInDrag[i]) // Only pluck if not already plucked in this drag
            {
                strings[i]->pluck();
                stringsPluckedInDrag[i] = true;
                
                // Add some visual feedback - briefly show displacement
                stringDisplacements[i] = 20.0f;
                repaint();
                
                // Start a timer to reset the visual displacement
                juce::Timer::callAfterDelay(100, [this, i]() {
                    if (i < stringDisplacements.size())
                    {
                        stringDisplacements[i] = 0.0f;
                        repaint();
                    }
                });
            }
        }
    }
}

void MainComponent::mouseUp (const juce::MouseEvent& e)
{
    isDragging = false;
    // Reset all displacements
    std::fill(stringDisplacements.begin(), stringDisplacements.end(), 0.0f);
    repaint();
}