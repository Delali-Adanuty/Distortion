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

    void prepare(double sampleRate, int expectedMaxFramesPerBlock, int numChannels);

    void process(juce::AudioBuffer<float>& buffer) noexcept;

    void reset() noexcept;

    void setDrive(float driveFloat);

    void setTrim(float trimFloat);

    void setDryWetMix(float dryWetMixFloat);

private:
    enum
    {
        driveIndex,
        waveshaperIndex,
        trimIndex
    };

    float drive;
    float trim;
    float dryWetMix;

    juce::dsp::ProcessorChain<
        juce::dsp::Gain<float>,
        juce::dsp::WaveShaper<float>,
        juce::dsp::Gain<float>
    > processorChain;


    juce::dsp::DryWetMixer<float> dryWetMixer;
};



#endif //DISTORTION_H
