/**
 * @file Parameters.h
 * @author Van-Dyck Adanuty
 *
 *
 */
 
#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <juce_audio_processors/juce_audio_processors.h>

struct Parameters{
    explicit Parameters(juce::AudioProcessor&);

    juce::AudioParameterFloat& drive;
};
#endif //PARAMETERS_H
