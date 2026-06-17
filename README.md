# Multi-Rate Distortion Processor

![C++](https://img.shields.io/badge/C++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![JUCE](https://img.shields.io/badge/JUCE-Framework-brightgreen.svg?style=for-the-badge)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)

**Author:** Van-Dyck Adanuty 

A professional-grade, real-time C++ audio distortion processor built utilizing the JUCE framework. This plugin leverages a multi-rate DSP architecture to apply non-linear waveshaping at high resolutions, effectively eliminating harmonic aliasing while maintaining a strictly lock-free audio thread and highly optimized CPU footprint.

---

## Core Features

* **Multi-Rate DSP Architecture:** A 3-stage processor chain that isolates linear equalizers to the native host rate while running non-linear algorithms at high resolution.
* **Anti-Aliasing Engine:** 8x oversampling block utilizing 141st-order linear-phase FIR equiripple filters for mathematically transparent interpolation and decimation.
* **State Management:** Complete DAW session recall via JSON state serialization and ~20ms linear parameter smoothing to prevent audio discontinuities during live adjustments.

---

## System Architecture

To maximize CPU efficiency and prevent digital aliasing, the signal flow is divided into three isolated stages:

### 1. Pre-Processing (Native Host Rate)
Linear operations do not generate harmonics, so they are processed efficiently at the DAW's native sample rate (e.g., 44.1 kHz).
* **Input Drive:** Dynamic gain staging.
* **High-Pass Filter:** 1st-order IIR filter to remove low-frequency rumble before distortion.

### 2. Nonlinear Core (8x Oversampled)
The signal is bridged into a high-speed processor chain to provide the bandwidth necessary for non-linear harmonic generation.
* **FIR Interpolation:** Upsamples the signal by a factor of 8 (e.g., to 352.8 kHz).
* **Waveshaper:** Applies an asymmetrical $\tanh(x)$ soft-clipping transfer function.
* **DC Blocker:** A 5Hz high-pass filter calculated at the oversampled rate to re-center the waveform and correct asymmetrical DC offset.
* **FIR Decimation:** Steep anti-aliasing filter that removes frequencies above the native Nyquist limit before downsampling.

### 3. Post-Processing (Native Host Rate)
The downsampled signal is cleaned up and prepared for output.
* **Low-Pass Filter:** 1st-order IIR filter to tame high-frequency digital harshness.
* **Output Trim:** Final gain normalization.
* **Dry/Wet Mixer:** Phase-accurate linear interpolation for parallel processing.

---

## Build Instructions

This project uses CMake for cross-platform builds. The instructions below are configured for a standard Windows development environment.

### Prerequisites

* **CMake** (v3.21 or higher)
* **C++ Compiler** supporting C++20 (MSVC / Visual Studio Build Tools recommended)
* **JUCE Framework** (Handled automatically via CMake `FetchContent` or local subdirectory)

### Compilation Steps

1. **Clone the repository:**
   ```bash
   git clone https://github.com/Delali-Adanuty/Distortion
   cd Distortion
Generate the build files:

2. **Generate the bulid files:**
   ```bash
    cmake -B build -G "Visual Studio 17 2022" 
    Build the plugin:
3. **Build the plugin:**
    ```bash
    cmake --build build --config Release
    
**Installation:**
The compiled .vst3 file will automatically be copied to your system's default VST3 directory (e.g., C:\Program Files\Common Files\VST3) during the build process, provided your CMake configuration is set up to deploy it.

**Acknowledgments & References**
A massive credit goes to the Stanford EE264 paper(https://web.stanford.edu/class/ee264/projects/EE264_w2015_final_project_herman.pdf), which is where I learned the core mathematics of nonlinear aliasing and the exact algorithmic steps needed to build this architecture:

Digital Implementations of Musical Distortion (Herman, Stanford University)
