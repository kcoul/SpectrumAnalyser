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
    // The plugin's initial editor size.
    setSize (900, 500);
    
    sampleRate.addListener (this);
    // The sampleRate has already been set in the HiReSamAudioProcessor before
    // the creation of the HiReSamAudioProcessorEditor.
    // Because of this it is important to have the referTo call after the
    // addListener. Only in this order the valueChanged member function
    // will be called implicitly.
    sampleRate.referTo (getProcessor()->sampleRate);
    
    renderThread.addTimeSliceClient (&spectroscope);
    renderThread.startThread (3);
    
    header.setText("High Resolution Spectrum Analyse Meter", dontSendNotification);
    Font headerFont = Font (18.0f);
    header.setFont (headerFont);
    header.setJustificationType(Justification::centred);
    header.setColour (Label::textColourId, Colours::lightgoldenrodyellow);
    header.setColour (Label::backgroundColourId, Colours::black);
    
    addAndMakeVisible (&header);
    addAndMakeVisible (&spectroscope);
    addAndMakeVisible (&pitchDetector);
}

HiReSamAudioProcessorEditor::~HiReSamAudioProcessorEditor()
{
    renderThread.removeTimeSliceClient (&spectroscope);
    renderThread.stopThread (500);
}

//==============================================================================
void HiReSamAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::black);
    
    const int imageHeightPlusBorder = 150 + 10;
    g.drawImage(ImageCache::getFromMemory (BinaryData::Samuel_Gaehwiler_png, BinaryData::Samuel_Gaehwiler_pngSize), spectroscope.getWidth(), getHeight() - imageHeightPlusBorder, 150, 150, 0, 0, 300, 300);
//    g.setColour (Colours::black);
//    g.setFont (15.0f);
//    g.drawFittedText ("Zviel Arbet? HiRe sam@klangfreund.com!",
//                      0, 0, getWidth(), getHeight(),
//                      Justification::centred, 1);
}

void HiReSamAudioProcessorEditor::resized()
{
    header.setBounds(0, 0, getWidth(), 24);
    
    const int widthOfImageAndBubble = 160;
    spectroscope.setBounds (0, header.getHeight(), getWidth() - widthOfImageAndBubble, getHeight() - header.getHeight());
    pitchDetector.setBounds (0, header.getHeight(), spectroscope.getWidth(), getHeight() - header.getHeight() - spectroscope.getHeightOfFrequencyCaption());
}

void HiReSamAudioProcessorEditor::valueChanged (Value & value)
{
    if (value.refersToSameSourceAs (sampleRate))
    {
        spectroscope.setSampleRate (sampleRate.getValue());
        pitchDetector.setSampleRate (sampleRate.getValue());
    }
}

HiReSamAudioProcessor* HiReSamAudioProcessorEditor::getProcessor() const
{
    return static_cast <HiReSamAudioProcessor*> (getAudioProcessor());
}
