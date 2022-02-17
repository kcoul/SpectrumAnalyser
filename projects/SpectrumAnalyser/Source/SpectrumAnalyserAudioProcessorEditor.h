#pragma once

#include "SpectrumAnalyserHeader.h"
#include "SpectrumAnalyserAudioProcessor.h"
#include "HoverValueDisplay.h"
#include "SpectrumViewer.h"


//==============================================================================
/**
*/
class SpectrumAnalyserAudioProcessorEditor  : public AudioProcessorEditor,
                                              public Value::Listener
{
public:
    SpectrumAnalyserAudioProcessorEditor (SpectrumAnalyserAudioProcessor* ownerFilter,
                                          Value& repaintSpectrumViewer,
                                          drow::Buffer& spectrumMagnitudeBuffer,
                                          Value& detectedFrequency);
    ~SpectrumAnalyserAudioProcessorEditor();

    //==============================================================================
    // This is just a standard Juce paint method...
    void paint (Graphics& g);
    
    void resized();
    
    /** The value listener method. */
    void valueChanged (Value & value);
    
private:
    SpectrumAnalyserAudioProcessor* getProcessor() const;
    
    SpectrumViewer spectrumViewer;
    Value sampleRateValue;
    Label header;
    HoverValueDisplay hoverValueDisplay;
    
    std::unique_ptr<ResizableCornerComponent> resizer;
    /** Specifies the maximum size of the window. */
    ComponentBoundsConstrainer resizeLimits;
};
