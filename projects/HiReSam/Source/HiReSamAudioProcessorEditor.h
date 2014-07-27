/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "HiReSamHeader.h"
#include "HiReSamAudioProcessor.h"
#include "PitchDetectorComponent.h"
#include "SamWithBubble.h"
#include "SpectrumViewer.h"


//==============================================================================
/**
*/
class HiReSamAudioProcessorEditor  : public AudioProcessorEditor,
                                     public Value::Listener
{
public:
    HiReSamAudioProcessorEditor (HiReSamAudioProcessor* ownerFilter,
                                 Value& repaintSpectrumViewerValue,
                                 drow::Buffer& spectrumMagnitudeBuffer);
    ~HiReSamAudioProcessorEditor();

    //==============================================================================
    // This is just a standard Juce paint method...
    void paint (Graphics& g);
    
    void resized();
    
    /** The value listener method. */
    void valueChanged (Value & value);
    
    // Public because the HiReSamAudioProcessor needs to access them in
    // its processBlock to transfer the samples.
    PitchDetectorComponent pitchDetector;
    
private:
    SpectrumViewer spectrumViewer;
    Value sampleRate;
    Label header;
    SamWithBubble samWithBubble;
    
    HiReSamAudioProcessor* getProcessor() const;
};


#endif  // PLUGINEDITOR_H_INCLUDED
