# OtoDecks - Guitar String Synthesizer

A JUCE-based audio application featuring a plucked string synthesizer using the Karplus-Strong algorithm. This application simulates guitar strings that can be "strummed" by dragging across them.

## Features

- **Karplus-Strong Synthesis**: Physics-based string synthesis algorithm
- **Interactive UI**: Click and drag across strings to pluck them individually or strum multiple strings
- **6 Guitar Strings**: Tuned to standard guitar notes (E2, A2, D3, G3, B3, E4)
- **Real-time Audio**: Low-latency audio processing with JUCE
- **Visual Feedback**: Strings show visual displacement when interacted with

## How to Use

1. Launch the application
2. Use the volume slider to adjust output level
3. Click and drag across the strings to play them
4. Drag across multiple strings to create strumming effects

## Building

This project requires:
- JUCE Framework
- Xcode (for macOS builds)
- macOS 10.13 or later

To build:
1. Open `Builds/MacOSX/NewProject.xcodeproj` in Xcode
2. Build and run the project

## Technical Details

- Built with JUCE 6.x+
- Uses Karplus-Strong algorithm for realistic string synthesis
- Supports simultaneous string playback
- Cross-platform audio device support via JUCE

## License

This project is part of the OtoDecks suite of audio applications.
