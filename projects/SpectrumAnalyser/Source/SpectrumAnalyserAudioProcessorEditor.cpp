#include "SpectrumAnalyserAudioProcessor.h"
#include "SpectrumAnalyserAudioProcessorEditor.h"

//==============================================================================
SpectrumAnalyserAudioProcessorEditor::SpectrumAnalyserAudioProcessorEditor (SpectrumAnalyserAudioProcessor* ownerFilter,
                                                                            Value& repaintSpectrumViewer,
                                                                            drow::Buffer& spectrumMagnitudeBuffer,
                                                                            Value& detectedFrequency)
    : AudioProcessorEditor (ownerFilter),
      spectrumViewer (repaintSpectrumViewer, spectrumMagnitudeBuffer, detectedFrequency)
{
    sampleRateValue.addListener (this);
    // The sampleRate has already been set in the SpectrumAnalyserAudioProcessor
    // before the creation of the SpectrumAnalyserAudioProcessorEditor.
    // Because of this it is important to have the referTo call after the
    // addListener. Only in this order the valueChanged member function
    // will be called implicitly.
    sampleRateValue.referTo (getProcessor()->sampleRate);
    
    header.setText("High Resolution Spectrum Analysis Meter", dontSendNotification);
    Font headerFont = Font (18.0f);
    header.setFont (headerFont);
    header.setJustificationType(Justification::centred);
    header.setColour (Label::textColourId, Colours::lightgoldenrodyellow);
    header.setColour (Label::backgroundColourId, Colours::black);
    
    hoverValueDisplay.referToFrequencyValue (spectrumViewer.getFrequencyToDisplay());
    
    addAndMakeVisible (&header);
    addAndMakeVisible (&spectrumViewer);
    addAndMakeVisible (&hoverValueDisplay);
    
    // Add the triangular resizer component for the bottom-right of the UI.
    resizeLimits.setMinimumSize(360, 320);
    resizer = std::make_unique<ResizableCornerComponent>(this, &resizeLimits);
    addAndMakeVisible (resizer.get());
    
    // The plugin's initial editor size.
    setSize (1000, 500);
}

SpectrumAnalyserAudioProcessorEditor::~SpectrumAnalyserAudioProcessorEditor()
{
}

//==============================================================================
void SpectrumAnalyserAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::black);
}

void SpectrumAnalyserAudioProcessorEditor::resized()
{
    header.setBounds(0, 0, getWidth(), 24);
    
    spectrumViewer.setBounds (0, header.getHeight(), getWidth(), getHeight() - header.getHeight());

    const int widthForHoverValueDisplay = 190;
    const int maxHeight = getHeight() - header.getHeight();
    const int height = jmin (140, maxHeight);
    hoverValueDisplay.setBounds(getWidth() - widthForHoverValueDisplay, 10, widthForHoverValueDisplay, height);
    
    resizer->setBounds (getWidth() - 16, getHeight() - 16, 16, 16);
}

void SpectrumAnalyserAudioProcessorEditor::valueChanged (Value & value)
{
    if (value.refersToSameSourceAs (sampleRateValue))
    {
        spectrumViewer.setSampleRate (sampleRateValue.getValue());
    }
}

SpectrumAnalyserAudioProcessor* SpectrumAnalyserAudioProcessorEditor::getProcessor() const
{
    return static_cast <SpectrumAnalyserAudioProcessor*> (getAudioProcessor());
}
