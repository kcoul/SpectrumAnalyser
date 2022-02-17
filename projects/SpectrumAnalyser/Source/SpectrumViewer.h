#pragma once
#include "SpectrumAnalyserHeader.h"

//#if JUCE_MAC || JUCE_IOS || DROWAUDIO_USE_FFTREAL

//==============================================================================
/** Creates a standard Spectroscope.
    This will display the amplitude of each frequency bin from an FFT in a
    continuous line which will decay with time.
    This is very simple to use, it is a GraphicalComponent so just register one
    with a TimeSliceThread, make sure its running and then continually call the
    copySamples() method. The FFT itself will be performed on a background thread.
 */
class SpectrumViewer :  public Component,
                        public Value::Listener,
                        public Timer
{
public:
    //==============================================================================
    /** Provide it with the parameters from the SpectrumViewer.
     */
	SpectrumViewer (Value& repaintViewerValue,
                    drow::Buffer& magnitudeBuffer,
                    Value& detectedFrequencyValue);
	
    /** Destructor. */
	~SpectrumViewer();
	
    /** @internal */
	void resized() override;
	
    /** @internal */
	void paint (Graphics &g) override;

    //==============================================================================
    void setSampleRate (double newSampleRate);
    
    int getHeightOfFrequencyCaption();
    
    Value & getFrequencyToDisplay();

	void timerCallback() override;
    
    void valueChanged (Value &value) override;
    
    void mouseEnter (const MouseEvent &event) override;
    
    void mouseMove (const MouseEvent &event) override;
    
    void mouseExit (const MouseEvent &event) override;
	

private:
    //==============================================================================
    void createGradientImage();
        
    static const int frequenciesToPlot[];
    static const int numberOfFrequenciesToPlot;
    
    double sampleRate;
    Value repaintViewer;
    drow::Buffer& fftMagnitudeBuffer;
    
    Value detectedFrequency;
    Value frequencyToDisplay;
    bool mouseMode;
    int mouseXPosition;
    
    //==============================================================================
    /*  The numbers below the graph, indication the frequency.
     */
    class FrequencyCaption    : public Component
    {
    public:
        FrequencyCaption();
        ~FrequencyCaption();
        
        void paint (Graphics&);
        void resized();
        
        void setSampleRate (double newSampleRate);
        
    private:
        OwnedArray<Label> frequencyLabels;
        
        double sampleRate;
    };
    //==============================================================================
    
    FrequencyCaption frequencyCaption;
    int heightForFrequencyCaption;
	
    Image gradientImage;
        
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumViewer);
};

//#endif  // #if JUCE_MAC || JUCE_IOS || DROWAUDIO_USE_FFTREAL

