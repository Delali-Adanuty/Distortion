/**
 * @file Parameters.cpp
 * @author Van-Dyck Adanuty
 */

#include "Distortion/Parameters.h"

auto& addParameterToProcessor(juce::AudioProcessor& processor, auto parameter){
    auto &parameterReference = *parameter;
    processor.addParameter(parameter.release());
    return parameterReference;
}

juce::AudioParameterFloat& createDriveParameter(juce::AudioProcessor& processor){
  constexpr auto versionHint = 1;

  auto parameter = std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"drive", versionHint},
      "Drive",
      juce::NormalisableRange<float>{0.0f, 100.0f, 0.1f, 1.0f},
      50.0f
  );

  return addParameterToProcessor(processor, std::move(parameter));
}

Parameters::Parameters(juce::AudioProcessor& processor)
    : drive{createDriveParameter(processor)}
{
}