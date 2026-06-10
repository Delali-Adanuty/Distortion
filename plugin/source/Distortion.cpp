/**
 * @file Distortion.cpp
 * @author Van-Dyck Adanuty
 */
 
#include "Distortion/Distortion.h"
#include <ranges>


void Distortion::prepare(double sampleRate, int expectedMaxFramesPerBlock)
{
    const juce::dsp::ProcessSpec processSpec {
        .sampleRate = sampleRate,
        .maximumBlockSize = static_cast<juce::uint32>(expectedMaxFramesPerBlock),
        .numChannels = 1u,
      };
}


void Distortion::process(juce::AudioBuffer<float>& buffer) noexcept
{
    // For each frame
    for (const auto frameIndex : std::views::iota(0, buffer.getNumSamples()))
    {
        // for each channel sample in the frame
        for (const auto channelIndex :
             std::views::iota(0, buffer.getNumChannels()))
        {
            // Getting the current input sample
            const auto inputSample = buffer.getSample(channelIndex, frameIndex);


            // Processing
            const auto outputSample = inputSample * drive;

            // Setting the sample as the processed sample
            buffer.setSample(channelIndex, frameIndex, outputSample);
        }
    }
}

void Distortion::reset() noexcept
{
}

void Distortion::setDrive(float driveFloat)
{
    drive = driveFloat;
}