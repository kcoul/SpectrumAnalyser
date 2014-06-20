/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "HiReSamAudioProcessor.h"
#include "HiReSamAudioProcessorEditor.h"


//==============================================================================
HiReSamAudioProcessorEditor::HiReSamAudioProcessorEditor (HiReSamAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter),
      spectroscope(11), // FFT Size of 2^11 = 2048
      renderThread("FFT Render Thread")

{
    // This is where our plugin's editor size is set.
    setSize (900, 500);
    
    spectroscope.setLogFrequencyDisplay(true);
    addAndMakeVisible (&spectroscope);
    
    pitchDetector.setSampleRate(44100);
    pitchDetector.setLogFrequencyDisplay(true);
    addAndMakeVisible(&pitchDetector);
    
    renderThread.addTimeSliceClient (&spectroscope);
    renderThread.startThread (3);
}

HiReSamAudioProcessorEditor::~HiReSamAudioProcessorEditor()
{
    renderThread.removeTimeSliceClient (&spectroscope);
    renderThread.stopThread (500);
}

//==============================================================================
void HiReSamAudioProcessorEditor::paint (Graphics& g)
{
//    g.fillAll (Colours::white);
//    g.setColour (Colours::black);
//    g.setFont (15.0f);
//    g.drawFittedText ("Zviel Arbet? HiRe sam@klangfreund.com!",
//                      0, 0, getWidth(), getHeight(),
//                      Justification::centred, 1);
}

void HiReSamAudioProcessorEditor::resized()
{
    spectroscope.setBounds (0, 0, getWidth(), getHeight());
    pitchDetector.setBounds(spectroscope.getBounds());
}
