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
    Distortion();

    void prepare(double sampleRate, int expectedMaxFramesPerBlock);

    void process(juce::AudioBuffer<float>& buffer) noexcept;

    void reset() noexcept;

    void setDrive(float driveFloat);

    void setTrim(float trimFloat);

private:
    enum
    {
        driveIndex,
        waveshaperIndex,
        trimIndex
    };

    float drive;
    float trim;

    juce::dsp::ProcessorChain<
        juce::dsp::Gain<float>,
        juce::dsp::WaveShaper<float>,
        juce::dsp::Gain<float>
    > processorChain;
};



#endif //DISTORTION_H
