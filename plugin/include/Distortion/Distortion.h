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

    void setTone(float toneFloat);

private:
    enum
    {
        driveIndex,
        preFilterIndex,
        waveshaperIndex,
        dcBlockerIndex,
        postFilterIndex,
        trimIndex
    };

    float drive = 20.0f;
    float trim = -20.0f;
    float tone = 0.0f;
    float dryWetMix = 0.0f;
    float preHighPassFreq = 600.0f;
    float postLowPassFreq = 1000.0f;
    float currentSampleRate;

    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;

    juce::dsp::ProcessorChain<
        juce::dsp::Gain<float>,
        juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>,
        juce::dsp::WaveShaper<float>,
        juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>,
        juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>,
        juce::dsp::Gain<float>
    > processorChain;


    juce::dsp::DryWetMixer<float> dryWetMixer;
};



#endif //DISTORTION_H
