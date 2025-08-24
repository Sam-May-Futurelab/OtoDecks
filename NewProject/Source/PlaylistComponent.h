/*
  Author: Sam May
  Note: Entire file written by me as part of CM2005 coursework
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>
#include <functional>

class PlaylistComponent  : public juce::Component,
                           public juce::TableListBoxModel,
                           public juce::Button::Listener

{
public:
    PlaylistComponent();
    ~PlaylistComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    int getNumRows() override;
    void paintRowBackground(juce::Graphics& g, 
                            int rowNumber, 
                            int width, 
                            int height, 
                            bool rowIsSelected) override;

    void paintCell(juce::Graphics& g,
                  int rowNumber,
                  int columnId,
                  int width,
                  int height,
                  bool rowIsSelected) override;

    juce::Component * refreshComponentForCell(int rowNumber, 
                                    int columnId, 
                                    bool isRowSelected, 
                                    juce::Component *existingComponentToUpdate) override;

    void buttonClicked(juce::Button* button) override;
    
    // simple access to track files for external loading
    juce::File getTrackFile(int index) const;
    
    // Callback for when the tracks should be loaded to the decks
    std::function<void(int, const juce::File&)> onTrackLoadRequest;
    
    // State persistence methods
    void savePlaylistState();
    void loadPlaylistState();
    
    // button component for the load buttons
    class LoadButton : public juce::TextButton
    {
    public:
        LoadButton(int trackIndex, int deckIndex, PlaylistComponent* parent)
        : trackIndex(trackIndex), deckIndex(deckIndex), parent(parent)
        {
          setButtonText(deckIndex == 0 ? "Deck 1" : "Deck 2");
        }
        
        int trackIndex;
        int deckIndex;
        PlaylistComponent* parent;
    };

private:

    juce::TableListBox tableComponent;
    std::vector<juce::String> trackTitles;
    std::vector<juce::File> trackFiles;
    
    // state persistence for when using the PropertiesFile
    std::unique_ptr<juce::PropertiesFile> playlistProperties;
    juce::File getPlaylistFile();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};
