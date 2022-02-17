#pragma once

#include "SpectrumAnalyserHeader.h"

//==============================================================================
/*
*/
class HoverValueDisplay    : public Component,
                         public Value::Listener
{
public:
    HoverValueDisplay();
    ~HoverValueDisplay();

    void paint (Graphics&) override;
    void resized() override;
    void referToFrequencyValue (const Value & valueToReferTo);
    
    virtual void valueChanged (Value & value) override;
    
private:
    Value frequencyValue;
    Label pitchLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HoverValueDisplay)
};
