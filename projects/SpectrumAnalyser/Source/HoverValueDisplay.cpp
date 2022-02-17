#include "../JuceLibraryCode/JuceHeader.h"
#include "HoverValueDisplay.h"

//==============================================================================
HoverValueDisplay::HoverValueDisplay()
  : frequencyValue (var(0))
{
    frequencyValue.addListener (this);
    
    pitchLabel.setColour (Label::textColourId, Colours::lightgoldenrodyellow);
    pitchLabel.setFont (Font (16.0f));
    pitchLabel.setJustificationType(Justification::horizontallyCentred);
    addAndMakeVisible (&pitchLabel);
    
    setInterceptsMouseClicks(false, false);
}

HoverValueDisplay::~HoverValueDisplay()
{
    frequencyValue.removeListener (this);
}

void HoverValueDisplay::paint (Graphics& g) {}

void HoverValueDisplay::resized()
{
    pitchLabel.setBounds (44, 20, getWidth() - 80, 20);
}

void HoverValueDisplay::referToFrequencyValue (const Value & valueToReferTo)
{
    frequencyValue.referTo(valueToReferTo);
}

void HoverValueDisplay::valueChanged (Value & value)
{
    if (value.refersToSameSourceAs(frequencyValue))
    {
        const int frequency = frequencyValue.getValue();
        String pitchString = String (frequency);
        pitchString << " Hz";
        
        if (frequency > 7)
        {
            // At 7 Hz, the function below returns the String "-1" (without a letter in front of it).
            pitchString << " (" << drow::Pitch::fromFrequency (frequency).getMidiNoteName() << ")";
        }

        pitchLabel.setText (pitchString, NotificationType::dontSendNotification);
    }
}
