#include <JuceHeader.h>
#include "PlaylistComponent.h"

PlaylistComponent::PlaylistComponent()
{
    // load saved playlist state first
    loadPlaylistState();
    
    // if no saved state, load from tracks folder
    if (trackTitles.empty()) {
        juce::File tracksDir("/Users/MacBook/Desktop/Projects/Uni/OtoDecks/NewProject/tracks");
        juce::Array<juce::File> files;
        tracksDir.findChildFiles(files, juce::File::findFiles, false, "*.mp3;*.wav");
        
        for (auto file : files)
        {
            trackTitles.push_back(file.getFileNameWithoutExtension());
            trackFiles.push_back(file);
        }
        
        // save the loaded tracks
        savePlaylistState();
    }
    
    // styles the table headers with track details and load buttons for each deck
    tableComponent.getHeader().addColumn("Track Title", 1, 300);
    tableComponent.getHeader().addColumn("Size", 2, 80);
    tableComponent.getHeader().addColumn("Load Deck 1", 3, 100);
    tableComponent.getHeader().addColumn("Load Deck 2", 4, 100);
    tableComponent.getHeader().setStretchToFitActive(true);
    
    // Sets the header colors
    tableComponent.getHeader().setColour(juce::TableHeaderComponent::backgroundColourId, 
                                        juce::Colour::fromRGB(50, 50, 60));
    tableComponent.getHeader().setColour(juce::TableHeaderComponent::textColourId, 
                                        juce::Colour::fromRGB(220, 220, 225));
    tableComponent.getHeader().setColour(juce::TableHeaderComponent::outlineColourId, 
                                        juce::Colour::fromRGB(64, 224, 208).withAlpha(0.6f));
    
    // Sets the table colors
    tableComponent.setColour(juce::ListBox::backgroundColourId, 
                            juce::Colour::fromRGB(35, 35, 40));
    tableComponent.setColour(juce::ListBox::outlineColourId, 
                            juce::Colour::fromRGB(64, 224, 208).withAlpha(0.3f));

    tableComponent.setModel(this);
    addAndMakeVisible(tableComponent);
    
    // button loading system
    tableComponent.setMultipleSelectionEnabled(false);
}

PlaylistComponent::~PlaylistComponent()
{
}

void PlaylistComponent::paint (juce::Graphics& g)
{
    // Background gradient
    juce::ColourGradient playlistGradient(juce::Colour::fromRGB(40, 40, 45), 0, 0,
                                          juce::Colour::fromRGB(30, 30, 35), 0, getHeight(), false);
    g.setGradientFill(playlistGradient);
    g.fillAll();
    
    // Border
    g.setColour(juce::Colour::fromRGB(64, 224, 208).withAlpha(0.5f));
    g.drawRect(getLocalBounds(), 1);
    
    // Title
    g.setColour(juce::Colour::fromRGB(220, 220, 225));
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.drawText("MUSIC LIBRARY", getLocalBounds().removeFromTop(30).reduced(10, 5), 
               juce::Justification::topLeft, true);
}

void PlaylistComponent::resized()
{
    auto area = getLocalBounds();
    
    area.removeFromTop(35);
    
    // Padding around the table
    area.reduce(10, 5);
    
    tableComponent.setBounds(area);
}


int PlaylistComponent::getNumRows()
{
    return static_cast<int>(trackTitles.size());
}


void PlaylistComponent::paintRowBackground(juce::Graphics& g, 
                        int rowNumber, 
                        int width, 
                        int height, 
                        bool rowIsSelected)
  {
    if (rowIsSelected)
    {
        // Teal selection color
        g.fillAll(juce::Colour::fromRGB(64, 224, 208).withAlpha(0.3f));
    }
    else
    {
        // Row colors
        if (rowNumber % 2 == 0)
          g.fillAll(juce::Colour::fromRGB(38, 38, 42));
        else
            g.fillAll(juce::Colour::fromRGB(42, 42, 47));
    }
    
    // separator line
    g.setColour(juce::Colour::fromRGB(55, 55, 60));
    g.drawHorizontalLine(height - 1, 0, width);
  }

void PlaylistComponent::paintCell(juce::Graphics& g,
                int rowNumber,
                int columnId,
                int width,
                int height,
                bool rowIsSelected)
  {
    // Track titles in column 1
    if (columnId == 1)
    {
        // Sets the text color based on selection
        if (rowIsSelected)
          g.setColour(juce::Colour::fromRGB(25, 25, 30));
        else
            g.setColour(juce::Colour::fromRGB(220, 220, 225));
            
        g.setFont(juce::Font(12.0f, juce::Font::plain));
        
        g.drawText(trackTitles[rowNumber], 
                    10, 0,
                    width - 10, 
                    height,
                    juce::Justification::centredLeft, true);
    }
    // File size in column 2
    else if (columnId == 2)
    {
        if (rowIsSelected)
            g.setColour(juce::Colour::fromRGB(25, 25, 30));
        else
            g.setColour(juce::Colour::fromRGB(160, 160, 165));
            
        g.setFont(juce::Font(10.0f, juce::Font::plain));
        
        if (rowNumber < static_cast<int>(trackFiles.size()))
        {
            auto fileSize = trackFiles[rowNumber].getSize();
            juce::String sizeStr = juce::String(fileSize / (1024 * 1024)) + " MB";
            g.drawText(sizeStr, 
                        5, 0, width - 5, height,
                        juce::Justification::centred, true);
        }
    }
  }

  juce::Component * PlaylistComponent::refreshComponentForCell(int rowNumber, 
                                        int columnId, 
                                        bool isRowSelected, 
                                        juce::Component *existingComponentToUpdate)
  {
    // Column 3 = Deck 1, Column 4 = Deck 2
      if (columnId == 3 || columnId == 4)
      {
        if (existingComponentToUpdate == nullptr)
        {
          LoadButton* btn = new LoadButton(rowNumber, columnId - 3, this); // columnId-3 = deckIndex (0 or 1)
          btn->setColour(juce::TextButton::buttonColourId, 
                        columnId == 3 ? juce::Colour::fromRGB(70, 130, 180) : juce::Colour::fromRGB(220, 20, 60));
          btn->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
          btn->addListener(this);
          existingComponentToUpdate = btn;
        }
      }
      return existingComponentToUpdate;
    }

    void PlaylistComponent::buttonClicked(juce::Button* button)
    {
      LoadButton* loadBtn = dynamic_cast<LoadButton*>(button);
      if (loadBtn)
      {
          if (onTrackLoadRequest && loadBtn->trackIndex < static_cast<int>(trackFiles.size()))
          {
              onTrackLoadRequest(loadBtn->deckIndex, trackFiles[loadBtn->trackIndex]);
              
              savePlaylistState();
          }
      }
    }

juce::File PlaylistComponent::getTrackFile(int index) const
{
    if (index >= 0 && index < static_cast<int>(trackFiles.size()))
        return trackFiles[index];
    return juce::File();
}

// State persistence implementation

juce::File PlaylistComponent::getPlaylistFile()
{
    // Saves to DJ's Documents folder
    juce::File documentsDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    juce::File otoDecksDir = documentsDir.getChildFile("OtoDecks");
    otoDecksDir.createDirectory();
    return otoDecksDir.getChildFile("playlist.properties");
}

void PlaylistComponent::savePlaylistState()
{
    // Create PropertiesFile
    juce::PropertiesFile::Options options;
    options.applicationName = "OtoDecks";
    options.filenameSuffix = ".properties";
    options.folderName = "OtoDecks";
    
    playlistProperties.reset(new juce::PropertiesFile(getPlaylistFile(), options));
    
    // Saves the track count and paths
    playlistProperties->setValue("trackCount", static_cast<int>(trackFiles.size()));
    
    for (int i = 0; i < static_cast<int>(trackFiles.size()); ++i)
    {
        juce::String trackKey = "track_" + juce::String(i) + "_path";
        juce::String titleKey = "track_" + juce::String(i) + "_title";
        
        playlistProperties->setValue(trackKey, trackFiles[i].getFullPathName());
        playlistProperties->setValue(titleKey, trackTitles[i]);
    }
    
    playlistProperties->saveIfNeeded();
}

void PlaylistComponent::loadPlaylistState()
{
    // Loads the existing playlist state
    juce::PropertiesFile::Options options;
    options.applicationName = "OtoDecks";
    options.filenameSuffix = ".properties";
    options.folderName = "OtoDecks";
    
    juce::File playlistFile = getPlaylistFile();
    
    if (playlistFile.exists())
    {
        playlistProperties.reset(new juce::PropertiesFile(playlistFile, options));
        
        int savedTrackCount = playlistProperties->getIntValue("trackCount", 0);
        
        if (savedTrackCount > 0)
        {
            // clears the current vectors and loads saved tracks
            trackTitles.clear();
            trackFiles.clear();
            
            for (int i = 0; i < savedTrackCount; ++i)
            {
                juce::String trackKey = "track_" + juce::String(i) + "_path";
                juce::String titleKey = "track_" + juce::String(i) + "_title";
                
                juce::String savedPath = playlistProperties->getValue(trackKey);
                juce::String savedTitle = playlistProperties->getValue(titleKey);
                
                juce::File trackFile(savedPath);
                if (trackFile.exists())
                {
                    trackFiles.push_back(trackFile);
                    trackTitles.push_back(savedTitle);
                }
            }
        }
    }
}