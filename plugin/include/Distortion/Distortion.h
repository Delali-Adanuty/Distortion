/**
 * @file Distortion.h
 * @author Van-Dyck Adanuty
 *
 *
 */
 
#ifndef DISTORTION_H
#define DISTORTION_H

#include <juce_dsp/juce_dsp.h>


class Distortion {
public:
    void prepare(double sampleRate, int expectedMaxFramesPerBlock);

    void process(juce::AudioBuffer<float>& buffer) noexcept;

    void reset() noexcept;

    void setDrive(float driveFloat);

private:
    float drive = 5.0f;
};



#endif //DISTORTION_H
