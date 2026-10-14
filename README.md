# Multi-Rate Distortion Processor

![C++](https://img.shields.io/badge/C++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![JUCE](https://img.shields.io/badge/JUCE-Framework-brightgreen.svg?style=for-the-badge)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)

This is a tanh distortion plugin that decreases aliasing with 8× oversampling. I made it in C++ with the JUCE framework.

## Purpose

A nonlinear function adds harmonics to a signal. Harmonics above the Nyquist frequency fold back into the audio band as inharmonic tones (aliasing). This plugin increases the sample rate by 8 before the waveshaper. Then it removes the high harmonics and decreases the rate again. In my tests, this decreases the aliased power by 80 dB.

## Formats

- **VST3 plugin:** Operates in a Digital Audio Workstation (DAW).
- **Standalone application:** Uses the audio drivers of the computer.

## Function

- **Three-stage signal path:** The linear filters operate at the host rate. Only the waveshaper and the DC blocker operate at 8× the host rate.
- **Oversampling:** The JUCE `dsp::Oversampling` class with three 2× stages of half-band, linear-phase FIR filters.
- **Waveshaper:** A symmetric `tanh(x)` function. It adds odd harmonics.
- **Tone control:** One control moves the cutoffs of a high-pass filter before the waveshaper and a low-pass filter after it.
- **Dry/wet mix:** Combines the input signal with the distorted signal.
- **State recall:** The plugin saves all parameters as JSON with a DAW session.

## Controls

| Control | Range | Default | Function |
| :--- | :--- | :--- | :--- |
| Drive | 0 dB to 40 dB | 20 dB | Sets the gain before the waveshaper. More drive gives more distortion. |
| Trim | −40 dB to 0 dB | −20 dB | Sets the output gain. |
| Tone | 0.0 to 1.0 | 0.0 | Opens the passband from 600 Hz–1 kHz (0.0) to 40 Hz–15 kHz (1.0). |
| DryWetMix | 0.0 to 1.0 | 0.0 | Sets the fraction of distorted signal. 0.0 is fully dry. |

The plugin has no custom editor. The host shows its generic controls.

## Build

The project uses CMake 3.22 or later and C++23. CPM downloads JUCE 8.0.13 and GoogleTest.

1. Run `cmake -S . -B build`.
2. Run `cmake --build build`.

## Results

- **Aliasing:** With a 5 kHz sine and 20 dB of drive at 44.1 kHz, the inharmonic power is −100.4 dB. Without oversampling, it is −20.5 dB.
- **Harmonics:** At 750 Hz, the 3rd harmonic is −10.7 dB and the 5th is −17.3 dB. The even harmonics are below −139 dB.
- **Latency:** 64.25 samples (1.46 ms at 44.1 kHz).

## Limitations

- **Drive during playback:** A drive change applies only when the host prepares the plugin again, for example when playback restarts.
- **Partial mix:** The dry signal is not aligned with the wet signal. Mix values between 0.0 and 1.0 cause comb filtering.
- **Latency report:** The plugin does not report its latency to the host. Thus, a DAW does not correct the delay.
- **Default mix:** The default mix is 0.0. Set DryWetMix to more than 0.0 to hear the distortion.
- **Tone changes:** The tone filters change without a ramp. The tone control has only 11 positions.
- **Odd harmonics only:** The symmetric waveshaper does not add even harmonics.
- **CPU load:** The 8× FIR filters use more CPU than a processor at the host rate.

## Technical Documentation

For the signal path, the filter orders, the test conditions, and the known issues, read the **[technical documentation](https://delali-adanuty.github.io/Distortion/)**.

## Reference

W. Herman, *DSPFuzz: A Guitar Distortion Pedal Using The Stanford DSP Shield*, EE264 Final Project, Stanford University, Winter 2015.

---

**Author:** Van-Dyck Adanuty
