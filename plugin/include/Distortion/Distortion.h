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


    float drive = 20.0f;
    float trim = -20.0f;
    float tone = 0.0f;
    float dryWetMix = 0.0f;
    float preHighPassFreq = 600.0f;
    float postLowPassFreq = 1000.0f;
    float currentSampleRate;

    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;

    enum
    {
        driveIndex,
        preFilterIndex
    };
    juce::dsp::ProcessorChain<
        juce::dsp::Gain<float>,
        juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>
    > preProcessingChain;

    enum
    {
        waveshaperIndex,
        dcBlockerIndex
    };
    juce::dsp::ProcessorChain<
        juce::dsp::WaveShaper<float>,
        juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>
    > overSampledChain;

    enum{
        postFilterIndex,
        trimIndex
    };

    juce::dsp::ProcessorChain<
        juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>,
        juce::dsp::Gain<float>
    > postProcessingChain;


    juce::dsp::DryWetMixer<float> dryWetMixer;

    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> driveTransitionSmoother{0.0f};
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> trimTransitionSmoother{0.0f};
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> toneTransitionSmoother{0.0f};
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> dryWetMixTransitionSmoother{0.0f};

};



#endif //DISTORTION_H
