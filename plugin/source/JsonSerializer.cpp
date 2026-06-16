/**
 * @file JsonSerializer.cpp
 * @author Van-Dyck Adanuty
 */

#include <iostream>
#include <Distortion/JsonSerializer.h>
#include <juce_core/juce_core.h>

struct SerializableParameters {
        float drive;
        float trim;
        float dryWetMix;
        float tone;

        static constexpr auto marshallingVersion = 1;

        template <typename Archive, typename T>
        static void serialise(Archive& archive, T& t) {
            using namespace juce;

            if (archive.getVersion() != 1) {
                return;
            }

            std::string pluginName = JucePlugin_Name;

            archive(named("pluginName", pluginName));

            if (pluginName != JucePlugin_Name) {
                return;
            }

            archive(
                named("drive", t.drive),
                named("trim", t.trim),
                named("dryWetMix", t.dryWetMix),
                named("tone", t.tone));
        }
    };

SerializableParameters from(const Parameters& parameters) {
    return {
        .drive = parameters.drive.get(),
        .trim = parameters.trim.get(),
        .dryWetMix = parameters.dryWetMix.get(),
        .tone = parameters.tone.get()
    };
}


void JsonSerializer::serialize(const Parameters& parameters,
                               juce::OutputStream& output) {

  // TODO: serialize parameters to the output stream as JSON
    const auto parametersToSerialize = from(parameters);

    const auto json = juce::ToVar::convert(parametersToSerialize);

    if ( !json.has_value()) {
        return;
    }

    juce::JSON::writeToStream(output, *json, juce::JSON::FormatOptions{}
        .withSpacing(juce::JSON::Spacing::multiLine)
        .withMaxDecimalPlaces(2));
}

juce::Result JsonSerializer::deserialize(juce::InputStream& input,
                                         Parameters& parameters) {


    // TODO: deserialize parameters from the JSON input stream
    juce::var parsedResult;
    const auto result = juce::JSON::parse(input.readEntireStreamAsString(),
        parsedResult);

    if (result.failed()) {
        return result;
    }

    const auto parsedParameters = juce::FromVar::convert<SerializableParameters>(parsedResult);

    if (!parsedParameters.has_value()) {
        return juce::Result::fail("failed to parse parameters from JSON representation");
    }


    parameters.drive = parsedParameters->drive;
    parameters.trim = parsedParameters->trim;
    parameters.dryWetMix = parsedParameters->dryWetMix;
    parameters.tone = parsedParameters->tone;



    return juce::Result::ok();
}