/**
 * @file Distortion.cpp
 * @author Van-Dyck Adanuty
 */
 
#include "Distortion/Distortion.h"


Distortion::Distortion()
{
    auto& waveshaper = overSampledChain.template get<waveshaperIndex>();
    waveshaper.functionToUse = [] (float x)
    {
        return std::tanh(x);
    };

    preProcessingChain.get<driveIndex>().setRampDurationSeconds(0.025);
    postProcessingChain.get<trimIndex>().setRampDurationSeconds(0.025);
}

void Distortion::prepare(double sampleRate, int expectedMaxFramesPerBlock, int numChannels)
{
    const juce::dsp::ProcessSpec processSpec {
        .sampleRate = sampleRate,
        .maximumBlockSize = static_cast<juce::uint32>(expectedMaxFramesPerBlock),
        .numChannels = static_cast<juce::uint32>(numChannels),
      };

    currentSampleRate = sampleRate;

    oversampler = std::make_unique<juce::dsp::Oversampling<float>>(
        numChannels,
        3,
        juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple,
        true);

    oversampler->initProcessing(processSpec.maximumBlockSize);

    juce::dsp::ProcessSpec oversampledSpec = processSpec;
    oversampledSpec.sampleRate = sampleRate * oversampler->getOversamplingFactor();

    preProcessingChain.prepare(processSpec);
    overSampledChain.prepare(oversampledSpec);
    postProcessingChain.prepare(processSpec);
    dryWetMixer.prepare(processSpec);

    reset();



    driveTransitionSmoother.reset(sampleRate, 0.020);
    trimTransitionSmoother.reset(sampleRate, 0.020);
    toneTransitionSmoother.reset(sampleRate, 0.020);
    dryWetMixTransitionSmoother.reset(sampleRate, 0.020);

    driveTransitionSmoother.setCurrentAndTargetValue(drive);
    trimTransitionSmoother.setCurrentAndTargetValue(trim);
    toneTransitionSmoother.setCurrentAndTargetValue(tone);
    dryWetMixTransitionSmoother.setCurrentAndTargetValue(dryWetMix);

    if (toneTransitionSmoother.isSmoothing())
    {
        float currentTone = toneTransitionSmoother.getNextValue();
        setTone(currentTone);
    }
    else
    {
        setTone(tone);
    }
}


void Distortion::process(juce::AudioBuffer<float>& buffer) noexcept
{

    //Processor Chain Implementation
    float driveGain;
    float trimGain;

    driveTransitionSmoother.setTargetValue(drive);
    trimTransitionSmoother.setTargetValue(trim);

    if (driveTransitionSmoother.isSmoothing())
    {
        float currentDrive = driveTransitionSmoother.getCurrentValue();
        driveGain = juce::Decibels::decibelsToGain(currentDrive);
    }
    else
    {
        driveGain = juce::Decibels::decibelsToGain(drive);
    }

    if (trimTransitionSmoother.isSmoothing())
    {
        float currentTrim = trimTransitionSmoother.getCurrentValue();
        trimGain = juce::Decibels::decibelsToGain(trim);
    }
    else
    {
        trimGain = juce::Decibels::decibelsToGain(trim);
    }


    preProcessingChain.get<driveIndex>().setGainLinear(driveGain);
    postProcessingChain.get<trimIndex>().setGainLinear(trimGain);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    if (dryWetMixTransitionSmoother.isSmoothing())
    {
        float currentDryWetMix = dryWetMixTransitionSmoother.getCurrentValue();
        dryWetMixer.setWetMixProportion(currentDryWetMix);
    }
    else
    {
        dryWetMixer.setWetMixProportion(dryWetMix);
    }

    dryWetMixer.pushDrySamples(block);

    preProcessingChain.process(context);

    if (oversampler != nullptr)
    {
        juce::dsp::AudioBlock<float> overSampledBlock = oversampler->processSamplesUp(block);
        juce::dsp::ProcessContextReplacing<float> overSampledContext(overSampledBlock);
        overSampledChain.process(overSampledContext);

        oversampler->processSamplesDown(block);
    }
    else
    {
        overSampledChain.process(context);
    }

    postProcessingChain.process(context);

    dryWetMixer.mixWetSamples(block);

}

void Distortion::reset() noexcept
{
    preProcessingChain.reset();
    overSampledChain.reset();
    postProcessingChain.reset();
    dryWetMixer.reset();
}

void Distortion::setDrive(float driveFloat)
{
    drive = driveFloat;
}

void Distortion::setTrim(float trimFloat)
{
    trim = trimFloat;
}

void Distortion::setDryWetMix(float dryWetMixFloat)
{
    dryWetMix = dryWetMixFloat;
}

void Distortion::setTone(float toneFloat)
{
    tone = toneFloat;
    preHighPassFreq = juce::jmap(tone, 600.0f, 40.0f);
    postLowPassFreq = juce::jmap(tone, 1000.0f, 15000.0f);

    auto& prefilter = preProcessingChain.template get<preFilterIndex>();
    *prefilter.state = *FilterCoefs::makeFirstOrderHighPass(currentSampleRate, preHighPassFreq);

    auto& postfilter = postProcessingChain.template get<postFilterIndex>();
    *postfilter.state = *FilterCoefs::makeFirstOrderLowPass(currentSampleRate, postLowPassFreq);

    float overSampledRate = currentSampleRate * oversampler->getOversamplingFactor();
    auto& dcBlocker = overSampledChain.template get<dcBlockerIndex>();
    *dcBlocker.state = *FilterCoefs::makeFirstOrderHighPass(overSampledRate, 5.0f);
}
