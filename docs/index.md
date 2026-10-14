# Multi-Rate Distortion Processor: Technical Description

## 1. Scope

This document gives the signal path, parameters, and test results of the Multi-Rate Distortion Processor. The processor is a JUCE audio plugin (VST3) and standalone application. It applies tanh distortion at 8× the host sample rate to decrease aliasing.

---

## 2. Background

A nonlinear function adds harmonics to a signal. If a harmonic is above the Nyquist frequency, it folds back into the audio band as an inharmonic tone. This is aliasing. A plugin cannot change the sample rate of the host. Thus, the plugin must increase the sample rate internally before the nonlinear stage and decrease it after that stage.

The design follows the multi-rate structure in *DSPFuzz: A Guitar Distortion Pedal Using The Stanford DSP Shield* (W. Herman, Stanford EE264, Winter 2015). I made two changes:

- **Filters:** Herman designed one 141st-order FIR filter in MATLAB. I use the JUCE `dsp::Oversampling` class, which has three half-band FIR stages (Section 3.3).
- **Waveshaper:** Herman used rational and polynomial approximations for fixed-point hardware. My plugin operates in floating point, so I used the exact tanh function.

---

## 3. Signal Path

### 3.1 Overview

The `Distortion` class contains three JUCE `ProcessorChain` objects and one `DryWetMixer`:

| Stage | Sample rate | Processors |
| :--- | :--- | :--- |
| 1. Input stage | Host rate | Drive gain, first-order high-pass filter |
| 2. Nonlinear core | 8 × host rate | tanh waveshaper, 5 Hz DC blocker |
| 3. Output stage | Host rate | First-order low-pass filter, trim gain |
| 4. Mix | Host rate | Dry/wet mixer |

Before stage 1, the mixer keeps a copy of the input (the dry signal). After stage 3, the mixer combines the dry signal with the processed signal (the wet signal).

### 3.2 Input Stage

1. A `dsp::Gain` processor applies the drive gain (0 dB to 40 dB). The gain has a 25 ms linear ramp.
2. A first-order IIR high-pass filter removes low-frequency energy before the waveshaper. Less low-frequency energy gives less intermodulation distortion. The tone parameter sets the cutoff frequency (Section 4.1).

### 3.3 Nonlinear Core

The `dsp::Oversampling` object uses half-band equiripple FIR filters at maximum quality. It increases the sample rate in three 2× stages, for a total factor of 8. At 44.1 kHz, the internal rate is 352.8 kHz.

| Stage | Up filter order | Up stopband | Down filter order | Down stopband |
| :--- | :--- | :--- | :--- | :--- |
| 1× to 2× | 118 | −90 dB | 78 | −75 dB |
| 2× to 4× | 50 | −80 dB | 34 | −65 dB |
| 4× to 8× | 46 | −70 dB | 30 | −55 dB |

The first stage has the steepest filters, because it is nearest to the audio band. All filters are FIR filters with linear phase.

At the oversampled rate:

1. The waveshaper applies `y = tanh(x)`. This function is symmetric (odd). Thus, it adds only odd harmonics (Section 9.1).
2. A first-order high-pass filter at 5 Hz removes DC. The tanh function does not add a DC offset. Thus, this filter is a protection stage. It removes DC that comes from the input.

Then the `Oversampling` object decreases the rate back to the host rate. Its down filters remove the harmonics above the original Nyquist frequency before the rate decreases.

### 3.4 Output Stage

1. A first-order IIR low-pass filter decreases high-frequency content. The tone parameter sets the cutoff frequency (Section 4.1).
2. A `dsp::Gain` processor applies the trim gain (−40 dB to 0 dB). The gain has a 25 ms linear ramp.

### 3.5 Dry/Wet Mix

The `DryWetMixer` uses the linear mix rule. It has an internal 50 ms ramp for mix changes.

The mixer does not compensate for the oversampling latency. Thus, the dry signal arrives 64.25 samples before the wet signal. Refer to Section 9.3.

---

## 4. Parameters

| Parameter | Range | Step | Default | Function |
| :--- | :--- | :--- | :--- | :--- |
| Drive | 0 dB to 40 dB | 0.1 dB | 20 dB | Gain before the waveshaper. More drive gives more distortion. |
| Trim | −40 dB to 0 dB | 0.1 dB | −20 dB | Gain after the output low-pass filter. |
| Tone | 0.0 to 1.0 | 0.1 | 0.0 | Sets the cutoff frequencies of the two tone filters. |
| DryWetMix | 0.0 to 1.0 | 0.1 | 0.0 | Fraction of wet signal in the output. 0.0 is fully dry. |

The plugin has no custom editor. The host shows its generic parameter controls.

### 4.1 Tone Control Range

One tone value, t, sets the cutoff of the input high-pass filter and the output low-pass filter:

```
High-pass cutoff = 600 Hz − 560 Hz × t     (600 Hz at t = 0, 40 Hz at t = 1)
Low-pass cutoff  = 1000 Hz + 14000 Hz × t  (1 kHz at t = 0, 15 kHz at t = 1)
```

When t increases, both filters open. At t = 0, the passband is approximately 600 Hz to 1 kHz. This gives a narrow, mid-range sound. At t = 1, the passband is approximately 40 Hz to 15 kHz.

### 4.2 Parameter Changes

| Parameter | How the change gets to the audio |
| :--- | :--- |
| Drive | Through a smoothed value. Refer to Section 10.1. |
| Trim | Directly to the `dsp::Gain` processor, which has a 25 ms ramp. |
| Tone | Directly. The code calculates new filter coefficients at each audio block. There is no ramp. |
| DryWetMix | Directly to the `DryWetMixer`, which has a 50 ms ramp. |

---

## 5. Latency

The oversampling filters add a latency of 64.25 samples. This is 1.46 ms at 44.1 kHz and 1.34 ms at 48 kHz. The value is not an integer, because the class uses fractional latency by default.

This latency is small enough for live use. But the plugin does not report the latency to the host. Thus, a DAW does not correct it. Refer to Section 10.3.

---

## 6. State Save and Recall

The plugin saves its state as JSON through `JsonSerializer`. The JSON contains:

- The plugin name.
- A format version number (1).
- The four parameter values, with a maximum of two decimal places.

When the host loads a state, the plugin reads the version and the name first:

- If the JSON is not valid, the plugin does not change the parameters.
- If the version or the name does not agree with the plugin, the plugin sets all four parameters to 0.0. Refer to Section 10.6.

---

## 7. Build

The project uses CMake 3.22 or later and C++23. CPM downloads JUCE 8.0.13 and GoogleTest 1.17.0 into `libs/`.

1. Run `cmake -S . -B build`.
2. Run `cmake --build build`.

The build makes VST3 and Standalone versions. On Windows, the build copies the VST3 file to `C:/Program Files/Common Files/VST3`.

---

## 8. Development Notes

### 8.1 One Tone Control for Two Filters

The user sees one tone control. One value from 0.0 to 1.0 moves the cutoffs of two filters at the same time (Section 4.1). I kept the two filter cutoffs internal, so the user needs to set only one control.

### 8.2 CPU Measurement

I attached the Visual Studio ETW profiler to a live FL Studio session. The linear filters at the host rate used very little CPU. The full plugin, with the 8× FIR filters, used less than 10% of the total CPU.

---

## 9. Test Results

I measured these results with a test program that calls the `Distortion` class directly. The program uses JUCE 8.0.13, the same version as the plugin.

### 9.1 Harmonic Content

Test conditions: 48 kHz, 750 Hz sine at −6 dBFS, drive 20 dB, tone 1.0, mix 1.0, trim 0 dB.

| Harmonic | Level relative to the fundamental |
| :--- | :--- |
| 2nd | −139.8 dB |
| 3rd | −10.7 dB |
| 4th | −141.2 dB |
| 5th | −17.3 dB |
| 6th | −146.1 dB |
| 7th | −23.2 dB |

The even harmonics are at the numerical noise floor. This result agrees with the symmetric tanh function.

### 9.2 Aliasing

Test conditions: 44.1 kHz, 5.0 kHz sine at −6 dBFS, drive 20 dB, tone 1.0, mix 1.0, trim 0 dB.

I measured the total inharmonic power from 20 Hz to 20 kHz, relative to the fundamental. Inharmonic power is all power that is not at a harmonic of 5.0 kHz. For a reference, I applied the same gain, filters, and tanh function at 44.1 kHz without oversampling.

| Version | Inharmonic power |
| :--- | :--- |
| Without oversampling | −20.5 dB |
| Plugin (8× oversampling) | −100.4 dB |
| **Decrease** | **79.9 dB** |

### 9.3 Dry/Wet Alignment

Test conditions: 48 kHz, low-level white noise, drive 0 dB, tone 1.0, mix 0.5.

At a 50% mix, the 64.25-sample difference between the dry and wet signals causes comb filtering. The frequency response has notches at:

```
f = (2n + 1) × fs / (2 × 64.25)
```

At 48 kHz, the first notch is at approximately 373 Hz. Then there is a notch at each odd multiple of 373 Hz. The deepest notch from 100 Hz to 12 kHz is −42.7 dB at 2.6 kHz.

At a mix of 0.0 or 1.0, this comb filtering does not occur.

---

## 10. Known Issues and Limitations

### 10.1 Comb Filtering at Partial Mix

The dry signal is not delayed to align with the wet signal (Section 9.3). Mix values between 0.0 and 1.0 cause comb filtering.

### 10.2 The Plugin Does Not Report Latency

The plugin does not call `setLatencySamples()`. Thus, a DAW does not correct the 64.25-sample delay.

### 10.3 Tone Changes

- The tone filters change without a ramp. There is a tone smoother, but processBlock() sends the tone value directly to setTone(), so the smoother does not operate. Each 0.1 step moves the low-pass cutoff by 1.4 kHz at once. This can cause small clicks.
- The code makes new filter coefficient objects at each audio block. This allocates memory on the audio thread.
- The tone parameter has a step of 0.1, so it has only 11 positions.

### 10.4 Default Values

- The default mix is 0.0. Thus, a new plugin instance gives only the dry signal. Set DryWetMix to a value more than 0.0 to hear the distortion.
- The default tone is 0.0. Thus, the default passband is approximately 600 Hz to 1 kHz.

### 10.5 Algorithm Limitations

- **Odd harmonics only:** The symmetric tanh function does not add even harmonics.
- **CPU load:** The 8× FIR filters use more CPU than a processor at the host rate.
- **Fixed factor:** The factor is 8 at all host rates. At 96 kHz, the internal rate is 768 kHz. This uses more CPU than necessary.
- **Pre-ringing:** Linear-phase filters can cause a small pre-ringing before sharp transients.

### 10.6 State Recall With a Different Version

If the version or the name in the saved JSON does not agree, `serialise()` stops before it reads the parameters. JUCE then gives a parameter set with all values at zero, and the plugin applies it. The result is drive 0 dB, trim 0 dB, tone 0.0, and mix 0.0. A future change to the format version will cause this for all old sessions.


---

## 11. Reference

W. Herman, *DSPFuzz: A Guitar Distortion Pedal Using The Stanford DSP Shield*, EE264 Final Project, Stanford University, Winter 2015.

---

**Author:** Van-Dyck Adanuty
