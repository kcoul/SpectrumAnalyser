/*
  ==============================================================================

    SamWithBubble.cpp
    Created: 12 Jul 2014 9:17:15am
    Author:  Samuel Gaehwiler

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "HoverValueDisplay.h"

//==============================================================================
HoverValueDisplay::HoverValueDisplay()
  : frequencyValue (var(0))
{
    frequencyValue.addListener (this);
    
    pitchLabel.setColour (Label::textColourId, Colours::lightgoldenrodyellow);
//    pitchLabel.setColour (Label::backgroundColourId, Colours::red);
    Font pitchLabelFont = Font (16.0f);
    pitchLabel.setFont (pitchLabelFont);
    pitchLabel.setJustificationType(Justification::horizontallyCentred);
    addAndMakeVisible (&pitchLabel);
    
    setInterceptsMouseClicks(false, false);
}

HoverValueDisplay::~HoverValueDisplay()
{
    frequencyValue.removeListener (this);
}

void HoverValueDisplay::paint (Graphics& g)
{
    // label and text
    pitchLabel.setBounds (44, 20, getWidth() - 80, 20);
    
    g.setColour (Colours::lightgoldenrodyellow);
    g.setFont (12.0f);

}

void HoverValueDisplay::resized()
{
}

void HoverValueDisplay::referToFrequencyValue (const Value & valueToReferTo)
{
    frequencyValue.referTo(valueToReferTo);
    // pitchLabel.getTextValue().referTo(valueToReferTo);
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
            // TO DAVE: At 7 Hz, the function below returns the String "-1" (without a letter in front of it).
            pitchString << " (" << drow::Pitch::fromFrequency (frequency).getMidiNoteName() << ")";
        }

        pitchLabel.setText (pitchString, NotificationType::dontSendNotification);
    }
    
}
