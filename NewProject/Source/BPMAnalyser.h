#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

// Analyses the audio track to detect the BPM using beat detection
        class BPMAnalyser
{
public:
    BPMAnalyser(double sampleRate = 44100.0);
    ~BPMAnalyser();
    
    // Analyses an entire audio file and return its BPM
    double analyseBPM(const juce::File& audioFile);
    
    // Process audio in real-time chunks for live BPM detection
    void processAudioBuffer(const float* buffer, int numSamples);
    
    // Gets  the currently detected BPM
    double getCurrentBPM() const { return currentBPM; }
    
    // Resets the analysis state
    void reset();
    void setSampleRate(double newSampleRate);

private:
    double sampleRate;
 double currentBPM;
    
    // Audio data buffers
    juce::AudioBuffer<float> analysisBuffer;
    std::vector<double> energyHistory;
    std::vector<double> beatTimes;
    
        static constexpr int ANALYSIS_BUFFER_SIZE = 8192;
        static constexpr int ENERGY_HISTORY_SIZE = 200;
        static constexpr double MIN_BPM = 60.0;
        static constexpr double MAX_BPM = 200.0;
    
    // Beat detection
    double calculateEnergy(const float* buffer, int startSample, int numSamples);
    void detectBeat(double energy, int sampleIndex);
    double estimateBPMFromBeats();
    double smoothBPM(double newBPM);
    
    double previousEnergy;
    double energyThreshold;
    int samplesProcessed;
    double lastBeatTime;
    std::vector<double> recentBPMValues;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BPMAnalyser)
};
