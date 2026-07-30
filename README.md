# Multi-Rate Distortion Processor

![C++](https://img.shields.io/badge/C++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![JUCE](https://img.shields.io/badge/JUCE-Framework-brightgreen.svg?style=for-the-badge)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)

**Author:** Van-Dyck Adanuty 

Digital non-linear waveshaping inherently generates harmonics that exceed the Nyquist frequency, which reflect back into the audible spectrum as inharmonic aliasing. A standard single-rate waveshaper cannot compensate for this. This project implements a multi-rate DSP architecture that isolates non-linear algorithms to a high-resolution oversampled environment, generating harmonic distortion while attenuating digital artifacts.

Read more about the architecture and filter design **[here](https://delali-adanuty.github.io/Distortion/)**

---

## Core Features

* **Multi-Rate DSP Architecture:** A 3-stage processor chain that isolates linear equalizers to the native host rate while running non-linear algorithms at an 8x oversampled resolution.
* **Anti-Aliasing Engine:** 8x oversampling block utilizing 141st-order linear-phase FIR equiripple filters for a time-aligned impulse response during interpolation and decimation.
* **State Management:** Complete DAW session recall via JSON state serialization and ~20ms linear parameter smoothing to prevent numerical instability and audio discontinuities during live adjustments.

---

## System Architecture

To mitigate digital aliasing, the signal flow is divided into three isolated stages:

### 1. Pre-Processing (Native Host Rate)
Linear operations do not generate harmonics, so they are processed efficiently at the DAW's native sample rate (e.g., 44.1 kHz).
* **Input Drive:** Dynamic gain staging.
* **High-Pass Filter:** 1st-order IIR filter to attenuate low-frequency energy before distortion, minimizing intermodulation.

### 2. Nonlinear Core (8x Oversampled)
The signal is upsampled into a high-speed processor chain to provide the bandwidth necessary for non-linear harmonic generation.
* **FIR Interpolation:** Upsamples the signal by a factor of 8 (e.g., to 352.8 kHz).
* **Waveshaper:** Applies an asymmetrical tanh(x) soft-clipping transfer function.
* **DC Blocker:** A 5Hz high-pass filter calculated at the oversampled rate to re-center the waveform and correct the asymmetric DC offset.
* **FIR Decimation:** Steep anti-aliasing filter that attenuates frequencies above the native Nyquist limit before downsampling.

### 3. Post-Processing (Native Host Rate)
The downsampled signal is filtered and prepared for output.
* **Low-Pass Filter:** 1st-order IIR filter to attenuate residual high-frequency artifacts.
* **Output Trim:** Final gain normalization.
* **Dry/Wet Mixer:** Phase-accurate linear interpolation for parallel processing.

---

## Limitations

* **Group Delay:** The use of 141st-order linear-phase FIR filters for interpolation and decimation introduces inherent latency to the signal path, making it less suitable for live tracking scenarios where zero latency is required.
* **CPU Overhead:** The 8x oversampling and FIR convolution demand a higher CPU footprint compared to standard single-rate implementations.
* **Fixed Oversampling Rate:** The system is currently optimized for a static 8x multiplier rather than adapting dynamically to the host's base sample rate.


## Acknowledgments & References

Credit to the Stanford EE264 paper, *Digital Implementations of Musical Distortion* (Herman, Stanford University), which details the mathematics of non-linear aliasing and the filter design rationale utilized in this architecture.
