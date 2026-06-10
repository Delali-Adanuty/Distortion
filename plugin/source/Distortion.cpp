/**
 * @file Distortion.cpp
 * @author Van-Dyck Adanuty
 */
 
#include "Distortion/Distortion.h"
#include <ranges>


Distortion::Distortion()
{
    auto& waveshaper = processorChain.template get<waveshaperIndex>();
    waveshaper.functionToUse = [] (float x)
    {
        return std::tanh(x);
    };

    processorChain.get<driveIndex>().setRampDurationSeconds(0.025);
    processorChain.get<trimIndex>().setRampDurationSeconds(0.025);
}

void Distortion::prepare(double sampleRate, int expectedMaxFramesPerBlock, int numChannels)
{
    const juce::dsp::ProcessSpec processSpec {
        .sampleRate = sampleRate,
        .maximumBlockSize = static_cast<juce::uint32>(expectedMaxFramesPerBlock),
        .numChannels = static_cast<juce::uint32>(numChannels),
      };

    processorChain.prepare(processSpec);
    dryWetMixer.prepare(processSpec);
}


void Distortion::process(juce::AudioBuffer<float>& buffer) noexcept
{

    //Processor Chain Implementation


    float driveGain = juce::Decibels::decibelsToGain(drive);
    float trimGain = juce::Decibels::decibelsToGain(trim);

    processorChain.get<driveIndex>().setGainLinear(driveGain);
    processorChain.get<trimIndex>().setGainLinear(trimGain);

    juce::dsp::AudioBlock<float> block(buffer);

    juce::dsp::ProcessContextReplacing<float> context(block);

    dryWetMixer.setWetMixProportion(dryWetMix);

    dryWetMixer.pushDrySamples(block);

    processorChain.process(context);

    dryWetMixer.mixWetSamples(block);

}

void Distortion::reset() noexcept
{
    processorChain.reset();
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
