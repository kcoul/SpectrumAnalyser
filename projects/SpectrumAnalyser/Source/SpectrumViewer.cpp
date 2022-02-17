#include "SpectrumViewer.h"

//#if JUCE_MAC || JUCE_IOS || DROWAUDIO_USE_FFTREAL


// static member initialisation
// ============================

const int SpectrumViewer::frequenciesToPlot[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 20000};
const int SpectrumViewer::numberOfFrequenciesToPlot = 29;


//==============================================================================
SpectrumViewer::SpectrumViewer (Value& repaintViewerValue,
                                drow::Buffer& magnitudeBuffer,
                                Value& detectedFrequencyValue)
  : sampleRate                {44100.0},
    repaintViewer             (var(false)),
    fftMagnitudeBuffer        {magnitudeBuffer},
    detectedFrequency         {var(0)},
    frequencyToDisplay        {var(0)},
    mouseMode                 {false},
    mouseXPosition            {0},
    heightForFrequencyCaption {20},
    gradientImage             (Image::RGB, 100, 100, false)
{
	setOpaque (true);
    
    repaintViewer.referTo (repaintViewerValue);
    
    detectedFrequency.referTo (detectedFrequencyValue);
    detectedFrequency.addListener(this);

    gradientImage.clear (gradientImage.getBounds(), Colours::black);

    addAndMakeVisible (&frequencyCaption);
    
    startTimer (30);
}


SpectrumViewer::~SpectrumViewer()
{
}


void SpectrumViewer::resized()
{
    gradientImage = Image (Image::RGB, jmax (1, getWidth()), jmax (1, getHeight() - heightForFrequencyCaption), false);
    createGradientImage();

    frequencyCaption.setBounds (0, getHeight() - heightForFrequencyCaption,
                                 getWidth(), heightForFrequencyCaption);
}


void SpectrumViewer::paint(Graphics& g)
{
    const int w = getWidth();
    const int h = getHeight() - heightForFrequencyCaption;
    
    g.setColour (Colours::black);
    g.fillRect(0, 0, w, h);
    
    // Draw the vertical lines
    // -----------------------
    g.setColour (Colours::darkgreen.darker().darker());
    
    for (int i = 0; i < numberOfFrequenciesToPlot; ++i)
    {
        const double proportion = frequenciesToPlot[i] / (sampleRate * 0.5);
        int xPos = logTransformInRange0to1 (proportion) * getWidth();
        g.drawVerticalLine(xPos, 0.0f, getHeight());
    }
    
    
    // Draw the actual result from the FFT
    // -----------------------------------
    const int numBins = (const int)fftMagnitudeBuffer.getSize();
    const float* data = fftMagnitudeBuffer.getData();
    
    // NOTE TO DAVE96: The jlimit is not needed. Puts the load off the CPU by 1-2%.
    // Original line:      float y2, y1 = jlimit (0.0f, 1.0f, float (1 + (drow::toDecibels (data[0]) / 100.0f)));
    float x = 0;
    float y = 0;
    
    // The path which will be the border of the filled area.
    Path spectrumPath;
    // Add the top left point.
    // Only values > 0 are passed to drow::toDecibels().
    const float yInPercent = data[0]>0 ? float (1 + (drow::toDecibels (data[0]) / 100.0f)) : -0.01;
    y = h - h * yInPercent;
    // No coordinate should be NaN
    jassert (x == x && y == y);
    spectrumPath.startNewSubPath(x, y);
    
    for (int i = 1; i < numBins; ++i)
    {
        x = logTransformInRange0to1 ((float)i / numBins) * w;
        // Only values > 0 are passed to drow::toDecibels().
        const float yInPercent = data[i]>0 ? float (1 + (drow::toDecibels (data[i]) / 100.0f)) : -0.01;
        y = h - h * yInPercent;
        
        spectrumPath.lineTo(x, y);
    }
    
    g.setColour (Colours::green);
    float lineThickness = 0.75f;
    g.strokePath (spectrumPath, PathStrokeType(lineThickness));
    
    // Remark: Using g.drawVerticalLine - which is told to be much more efficient for the renderer -
    // on every bin position instead of using a Path improved the overall performance of this
    // spectrum analyser by only 5%. (I profiled it)
    
    // Bottom right point.
    spectrumPath.lineTo(x, getHeight());
    // Bottom left point.
    spectrumPath.lineTo(0, getHeight());
    spectrumPath.closeSubPath();
    
    // Fill the graph.
    const float opacity = 0.8f;
    g.setTiledImageFill(gradientImage, 0, 0, opacity);
    g.fillPath(spectrumPath);
    
    
    // Draw a vertical line at the mouse position
    // ------------------------------------------
    if (mouseMode)
    {
        g.setColour (Colours::lightgoldenrodyellow);
        g.drawVerticalLine(mouseXPosition -1, 0, getHeight());
        // Don't know why the -1 offset for the x position
        // is needed. (It is also needed if the drawing
        // would be placed directly on the component, i.e.
        // in the paint() function.)
        // Maybe because of rounding in the mouseMove function.
    }
    else
    {
        const double proportion = (double)detectedFrequency.getValue() / (sampleRate / 2.0);
        const int pitchXCoord = roundToInt (logTransformInRange0to1 (proportion) * getWidth());
        
        g.setColour (Colours::green);
        g.drawVerticalLine (pitchXCoord, 0.0f, (float) getHeight());
    }

}


//==============================================================================
void SpectrumViewer::setSampleRate (double newSampleRate)
{
    sampleRate = newSampleRate;
    frequencyCaption.setSampleRate (newSampleRate);
}


int SpectrumViewer::getHeightOfFrequencyCaption()
{
    return heightForFrequencyCaption;
}


Value & SpectrumViewer::getFrequencyToDisplay()
{
    return frequencyToDisplay;
}


void SpectrumViewer::timerCallback()
{
    // TODO: How compares the CPU usage if we listen to repaintViewer
    //   instead of doing it here in the timerCallback?
    if (repaintViewer == true)
	{
        repaint(0, 0, getWidth(), getHeight() - heightForFrequencyCaption);
            // - heightForFrequencyCaption: We don't need to repaint the frequency caption.
        
        repaintViewer = false;
    }

	// Make the FFT levels fall.
    const int magnitudeBufferSize = (const int)fftMagnitudeBuffer.getSize();
	float* magnitudeBuffer = fftMagnitudeBuffer.getData();
    
	for (int i = 0; i < magnitudeBufferSize; i++)
		magnitudeBuffer[i] *= JUCE_LIVE_CONSTANT (0.707f);
}


void SpectrumViewer::valueChanged (Value &value)
{
    if (!mouseMode && value.refersToSameSourceAs(detectedFrequency))
    {
        frequencyToDisplay = detectedFrequency.getValue();
    }
}


void SpectrumViewer::mouseEnter (const MouseEvent &event)
{
    mouseMode = true;
}


void SpectrumViewer::mouseMove (const MouseEvent &event)
{
    mouseXPosition = event.getPosition().getX();
    
    const float normalizedXPosition = (mouseXPosition) / (float)getWidth();
    const float frequency = sampleRate / 2.0f * expTransformInRange0to1 (normalizedXPosition);
    frequencyToDisplay = (int)frequency;
}


void SpectrumViewer::mouseExit (const MouseEvent &event)
{
    mouseMode = false;
}

void SpectrumViewer::createGradientImage()
{
    Graphics g (gradientImage);
    ColourGradient gradient (Colours::darkgreen.darker(), 0.0f, 0.8f * gradientImage.getHeight(), Colours::black, 0.0f, gradientImage.getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();
}

//==============================================================================
SpectrumViewer::FrequencyCaption::FrequencyCaption()
: sampleRate {44100.0}
{
    for (int i = 0; i < numberOfFrequenciesToPlot; ++i)
    {
        // Generate the frequencyString...
        const int frequency = frequenciesToPlot[i];
        String frequencyString;
        if (frequency % 1000 == 0)
        {
            frequencyString = String (frequency/1000) + "K";
        }
        else
        {
            frequencyString = String (frequency);
        }
        
        // ... and put it into a new Label.
        Label* frequencyLabel = new Label();
        frequencyLabel->setText(frequencyString, dontSendNotification);
        
        // Figuring out the width and height of the text and setting the Label accordingly.
        Font labelFont = Font(12.0f);
        frequencyLabel->setFont(labelFont);
        frequencyLabel->setColour (Label::textColourId, Colours::lightgoldenrodyellow);
        const int textWidth = labelFont.getStringWidth (frequencyLabel->getText());
        const int textHeight = labelFont.getHeight();
        frequencyLabel->setBorderSize (BorderSize<int>(0,1,0,1));
        frequencyLabel->setSize(textWidth + 2, textHeight);
        
        frequencyLabels.add(frequencyLabel);
        addAndMakeVisible(frequencyLabel);
    }
}

SpectrumViewer::FrequencyCaption::~FrequencyCaption()
{
}

void SpectrumViewer::FrequencyCaption::paint (Graphics& g)
{
    // Background
    // ----------
    g.fillAll (Colours::black); // Clear the background
    
    // Labels and lines
    // ----------------
    static float lineLength = 5.0f;
    g.setColour (Colours::lightgoldenrodyellow);
    
    for (int i = numberOfFrequenciesToPlot - 1; i >= 0; --i)
    {
        // Figure out the line position...
        const double proportion = frequenciesToPlot[i] / (sampleRate * 0.5);
        int xPosOfLine = logTransformInRange0to1 (proportion) * getWidth();
        
        // ...and the label position.
        const int widthOfLabel = frequencyLabels[i]->getWidth();
        frequencyLabels[i]->setTopLeftPosition(xPosOfLine - widthOfLabel / 2, lineLength);
        
        // Do labels overlap? Hide if neccessary
        frequencyLabels[i]->setVisible (true);
        for (int j = i+1; j < numberOfFrequenciesToPlot; ++j)
        {
            if (frequencyLabels[j]->isVisible()
                && frequencyLabels[j]->getX() <= frequencyLabels[i]->getX() + frequencyLabels[i]->getWidth())
            {
                frequencyLabels[i]->setVisible(false);
                break;
            }
        }
        // Only show the 20K label if it isn't truncated by the border of the whole component.
        if (i == numberOfFrequenciesToPlot - 1)
        {
            if (frequencyLabels[i]->getX() + frequencyLabels[i]->getWidth() > getWidth())
            {
                frequencyLabels[i]->setVisible(false);
            }
        }
        
        if (frequencyLabels[i]->isVisible())
        {
            g.drawVerticalLine(xPosOfLine, 0.0f, lineLength);
        }
    }
}

void SpectrumViewer::FrequencyCaption::resized()
{
}

void SpectrumViewer::FrequencyCaption::setSampleRate (double newSampleRate)
{
    sampleRate = newSampleRate;
}

//#endif // JUCE_MAC || JUCE_IOS || DROWAUDIO_USE_FFTREAL
