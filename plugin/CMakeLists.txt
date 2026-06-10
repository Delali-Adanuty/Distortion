cmake_minimum_required(VERSION 3.22)

project(DistortionPlugin VERSION 0.1.0)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Sets the Windows VST3 installation location to one
    # that doesn't require elevated permissions
    set_directory_properties(PROPERTIES JUCE_VST3_COPY_DIR "C:/Program Files/Common Files/VST3")
endif()

set(DISTORTION_PLUGIN_NAME "Distortion")
juce_add_plugin(${PROJECT_NAME}
        COMPANY_NAME
        Adanuty
        IS_SYNTH
        FALSE
        NEEDS_MIDI_INPUT
        FALSE
        NEEDS_MIDI_OUPUT
        FALSE
        PLUGIN_MANUFACTURER_CODE
        Adty
        PLUGIN_CODE
        DSTN
        FORMATS
        VST3 Standalone
        PRODUCT_NAME
        ${DISTORTION_PLUGIN_NAME}
        COPY_PLUGIN_AFTER_BUILD
        TRUE
)

target_sources(${PROJECT_NAME}
        PRIVATE
        source/PluginEditor.cpp
        source/PluginProcessor.cpp
        source/Parameters.cpp
        source/Distortion.cpp
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        juce::juce_audio_utils
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags

)

target_compile_definitions(${PROJECT_NAME}
        PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

if(MSVC)
    target_compile_definitions(${PROJECT_NAME}
            PRIVATE
            _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING
    )
endif()