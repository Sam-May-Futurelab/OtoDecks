#include "BPMAnalyser.h"

BPMAnalyser::BPMAnalyser(double sampleRate)
    : sampleRate(sampleRate), currentBPM(0.0), previousEnergy(0.0), 
      energyThreshold(0.0), samplesProcessed(0), lastBeatTime(0.0)
{
    // initialise analysis buffer
    analysisBuffer.setSize(1, ANALYSIS_BUFFER_SIZE);
    
    // reserve space for beat times
    energyHistory.reserve(ENERGY_HISTORY_SIZE);
    beatTimes.reserve(100);
    recentBPMValues.reserve(10);
    
    reset();
}

BPMAnalyser::~BPMAnalyser()
{
}

double BPMAnalyser::analyseBPM(const juce::File& audioFile)
{
    if (!audioFile.exists())
        return 0.0;
    
    // create audio format manager and reader
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(audioFile));
    
    if (reader == nullptr)
        return 0.0;
    
    // sets the sample rate from file
    setSampleRate(reader->sampleRate);
    
    // reset analyser state
    reset();
    
    // Read and analyse the audio in chunks
    const int bufferSize = 4096;
    juce::AudioBuffer<float> buffer(static_cast<int>(reader->numChannels), bufferSize);
    
    int samplesRead = 0;
    int totalSamples = static_cast<int>(reader->lengthInSamples);
    
    // analyse first 30 seconds or entire file (whichever is shorter)
    int samplesToAnalyse = juce::jmin(totalSamples, static_cast<int>(sampleRate * 30.0));
    
    while (samplesRead < samplesToAnalyse)
    {
    int samplesToRead = juce::jmin(bufferSize, samplesToAnalyse - samplesRead);
        
        if (!reader->read(&buffer, 0, samplesToRead, samplesRead, true, true))
            break;
        
        // converts to mono if stereo
        const float* channelData = buffer.getReadPointer(0);
        if (reader->numChannels > 1)
        {
            // Mixes down to mono
            const float* channel2Data = buffer.getReadPointer(1);
            for (int i = 0; i < samplesToRead; ++i)
            {
                float monoSample = (channelData[i] + channel2Data[i]) * 0.5f;
                buffer.setSample(0, i, monoSample);
            }
        }
        
        // Process the buffer
        processAudioBuffer(buffer.getReadPointer(0), samplesToRead);
        samplesRead += samplesToRead;
    }
    
    // Calculates the final BPM estimate
    return estimateBPMFromBeats();
}

void BPMAnalyser::processAudioBuffer(const float* buffer, int numSamples)
{
    //Processes audio in analysis frames
    const int frameSize = 512;  
    
    for (int i = 0; i < numSamples; i += frameSize)
    {
        int frameSamples = juce::jmin(frameSize, numSamples - i);
        
        //Calculates the energy of current frame
        double energy = calculateEnergy(buffer, i, frameSamples);
        
        // adds to energy history
        energyHistory.push_back(energy);
        if (energyHistory.size() > ENERGY_HISTORY_SIZE)
            energyHistory.erase(energyHistory.begin());
        
        // update energy threshold (adaptive)
        if (energyHistory.size() > 10)
        {
            double averageEnergy = 0.0;
            for (double e : energyHistory)
                averageEnergy += e;
            averageEnergy /= energyHistory.size();
            
            energyThreshold = averageEnergy * 1.3;
        }
        
        // Detects beats
        detectBeat(energy, samplesProcessed + i);
        
        previousEnergy = energy;
    }
    
    samplesProcessed += numSamples;
}

void BPMAnalyser::reset()
{
    currentBPM = 0.0;
    previousEnergy = 0.0;
    energyThreshold = 0.0;
    samplesProcessed = 0;
    lastBeatTime = 0.0;
    
    energyHistory.clear();
    beatTimes.clear();
    recentBPMValues.clear();
}

void BPMAnalyser::setSampleRate(double newSampleRate)
{
    sampleRate = newSampleRate;
}

double BPMAnalyser::calculateEnergy(const float* buffer, int startSample, int numSamples)
{
    double energy = 0.0;
    
    for (int i = 0; i < numSamples; ++i)
    {
        float sample = buffer[startSample + i];
        energy += sample * sample;
    }
    
    return energy / numSamples; // normalise by number of samples
}

void BPMAnalyser::detectBeat(double energy, int sampleIndex)
{
    // Simple onset detection: energy is significantly higher than previous energy

    bool isOnset = (energy > energyThreshold) && 
                   (energy > previousEnergy * 1.5) && 
                   (energyHistory.size() > 5);
    
    if (isOnset)
    {
        double currentTime = static_cast<double>(sampleIndex) / sampleRate;
        
        // Avoids detecting beats too close together (minimum 0.2 seconds apart)
        if (currentTime - lastBeatTime > 0.2)
        {
            beatTimes.push_back(currentTime);
            lastBeatTime = currentTime;
            
            // keeps only the recent beats (last 20 secs)
            while (!beatTimes.empty() && (currentTime - beatTimes.front()) > 20.0)
            {
                beatTimes.erase(beatTimes.begin());
            }
            

            // updates the  BPM estimate if we have enough beats
            if (beatTimes.size() >= 8)
            {
                double newBPM = estimateBPMFromBeats();
                if (newBPM > 0.0)
                {
                    currentBPM = smoothBPM(newBPM);
                }
            }
        }
    }
}

double BPMAnalyser::estimateBPMFromBeats()
{
    if (beatTimes.size() < 4)
        return 0.0;
    
    // calculates the intervals between consecutive beats
    std::vector<double> intervals;
    for (size_t i = 1; i < beatTimes.size(); ++i)
    {
        double interval = beatTimes[i] - beatTimes[i-1];
        
        // filters out unreasonable intervals
        if (interval > 0.25 && interval < 2.0) // (Between 30 and 240 BPM)
        {
            intervals.push_back(interval);
        }
    }
    
    if (intervals.empty())
        return 0.0;
    
    // find the most common interval
    std::map<int, int> intervalHistogram;
    
    for (double interval : intervals)
    {
        int bin = static_cast<int>(interval * 200); // 200 = 1000ms / 5ms
        intervalHistogram[bin]++;
    }
    
    // finds the most frequent interval
    int maxCount = 0;
    int bestBin = 0;
    for (const auto& pair : intervalHistogram)
    {
        if (pair.second > maxCount)
        {
            maxCount = pair.second;
            bestBin = pair.first;
        }
    }
    
    if (maxCount < 3) // Need at least 3 similar intervals
        return 0.0;
    
    // converts back to interval and then to BPM
    double bestInterval = static_cast<double>(bestBin) / 200.0;
    double bpm = 60.0 / bestInterval;
    
    // Validate BPM range
    if (bpm < MIN_BPM || bpm > MAX_BPM)
    {
        // Tries doubling or halving
        if (bpm < MIN_BPM)
            bpm *= 2.0;
        else if (bpm > MAX_BPM)
            bpm /= 2.0;
    }
    
    // Final range check
    if (bpm >= MIN_BPM && bpm <= MAX_BPM)
        return bpm;
    
    return 0.0;
}

double BPMAnalyser::smoothBPM(double newBPM)
{
    // Adds to recent values
    recentBPMValues.push_back(newBPM);
    
    // Keeps only last 5 values
    if (recentBPMValues.size() > 5)
        recentBPMValues.erase(recentBPMValues.begin());

    // Calculates weighted average (more weight to recent values)
    if (recentBPMValues.size() == 1)
        return newBPM;
    
    double weightedSum = 0.0;
    double totalWeight = 0.0;
    
    for (size_t i = 0; i < recentBPMValues.size(); ++i)
    {
        double weight = static_cast<double>(i + 1); 
        weightedSum += recentBPMValues[i] * weight;
        totalWeight += weight;
    }
    
    return weightedSum / totalWeight;
}
