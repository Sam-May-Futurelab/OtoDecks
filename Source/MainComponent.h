#pragma once

#include <JuceHeader.h>

// Plucked string synthesiser
class StringSynthesiser
{
public:
    StringSynthesiser (double sampleRate, double frequencyInHz)
    {
        auto delayLineLength = (size_t) juce::roundToInt (sampleRate / frequencyInHz);
        delayLine.resize (delayLineLength);
        std::fill (delayLine.begin(), delayLine.end(), 0.0f);
        pos = 0;
    }

    void pluck()
    {
        // Fill delay line with noise
        for (auto& sample : delayLine)
            sample = (juce::Random::getSystemRandom().nextFloat() * 2.0f) - 1.0f;
    }

    float getNextSample()
    {
        auto nextPos = (pos + 1) % delayLine.size();
        delayLine[nextPos] = (delayLine[pos] + delayLine[nextPos]) * 0.499f; // Filter
        auto output = delayLine[pos];
        pos = nextPos;
        return output;
    }

private:
    std::vector<float> delayLine;
    size_t pos = 0;
};

// Main
class MainComponent  : public juce::AudioAppComponent,
                       public juce::Button::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;

    // Implement Button::Listener
    void buttonClicked(juce::Button* button) override;

private:
    juce::Slider volSlider;
    juce::Label volLabel;
    juce::ComboBox trackSelector;
    juce::Label trackLabel;
    juce::TextButton playButton{"PLAY"};
    juce::TextButton stopButton{"STOP"};

    // Plucked strings
    std::vector<std::unique_ptr<StringSynthesiser>> strings;
    std::vector<juce::Rectangle<int>> stringAreas;
    std::vector<float> stringDisplacements; // How much each string is pulled
    std::vector<bool> stringsPluckedInDrag; // Which strings have been plucked 
    bool isDragging = false; 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
