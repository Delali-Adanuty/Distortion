# Building a Distortion Plugin: How I Learned to Stop Worrying and Love Aliasing

For a long time, distortion was just a word guitarists threw around. 
Then I took Signal Processing and the word got a lot scarier. In an academic DSP context, 
distortion means aliasing and this is just a buzzword for sampling a signal at a rate
low enough that it folds back into the audible range and comes out the other side 
as ugly, inharmonic garbage. So I had a problem: how do you build the musical distortion
that guitarists love without accidentally triggering the kind that DSP professors warn you about?

A standard plugin can't force the host DAW to run at a higher sample rate, so that was off the table. 
After some deep googling I landed on a Stanford EE264 paper by Woody Herman, *DSPFuzz
A Guitar Distortion Pedal Using
The Stanford DSP Shield*, which laid out the multi-rate architecture and filter 
design rationale I needed. The math clicked. This is that project.

---

## Three Stages, One Signal

I didn't know replicating the effects of aliasing digitally at a high level 
would require oversampling (makes sense a bit seeing as distortion is a sampling rate issue). 
I thought a simple math function that "clips" the waveform would be enough but alas...

### Stage 1 — Pre-Processing (native rate)

The signal comes in at whatever the host is running, typically 44.1 kHz. A variable input drive scales the signal, and a 1st-order high-pass filter strips out sub-bass frequencies before they turn the distortion into mud. Because this is all linear math, there's no aliasing risk and no reason to burn CPU upsampling it yet.

### Stage 2 — The Nonlinear Core (8× oversampled)

This is where things get interesting.

A 141st-order Parks-McClellan FIR equiripple interpolation filter — pulled directly from Herman's design (JUCE already has this filter so I didn't have to implement it;) — brings the sample rate up to 352.8 kHz. At that resolution, the harmonics the waveshaper generates fall so far above the audible range that by the time we decimate back down, they're gone before they can fold.

For the waveshaper itself I diverged from the paper. Herman's implementation uses rational and polynomial approximations suited for fixed-point DSP hardware. Since I was building a software plugin without those hardware constraints, 
I went with an asymmetric tanh(x) soft clipper instead. It treats the positive and negative halves of the waveform slightly differently to produce even-order harmonics and give the distortion its analog character.

A DC blocker sits immediately after the waveshaper to re-center the waveform that asymmetric clipping shifts off zero. Then a steep FIR decimation filter kills everything above Nyquist before dropping the signal back to native rate.

### Stage 3 — Post-Processing

Back at native rate, a low-pass post-EQ filter takes the edge off harsh high-frequency fizz, and an output trim brings the level back under control. A phase-accurate dry/wet mixer blends the processed and unprocessed signals, and the full plugin state serializes to JSON for DAW session recall.

---

## Challenges


**The Tone Knob Illusion.**
Users see one knob. Under the hood, that single 0.0–1.0 float is 
simultaneously driving the pre-distortion high-pass cutoff up and 
the post-distortion low-pass cutoff down. Keeping the UX simple while 
the DSP does something more complex was a good lesson in decoupling what users interact with from what's actually running.

**Parameter smoothing.**
Every user-facing parameter runs through a ~20ms smoothing ramp. 
Without it, snapping a value instantly creates a mathematical 
discontinuity and this causes the speaker cone being asked to jump positions in a 
fraction of a millisecond, which it physically cannot do. The result is an 
audible click or "noise". I now catch these in commercially released tracks more than
I'd like to admit.

---

## Takeaways

**Profiling is truth.**
I attached Visual Studio's ETW profiler to a live FL Studio session to see what the architecture actually cost. The native-rate linear filters barely registered. The 141st-order FIR equiripple filters, for all their weight, stayed comfortably under 10% total CPU overhead. The isolated multi-rate design did exactly what it was supposed to.

**Embrace the ProcessorChain.**
JUCE's `ProcessorChain` was a new one. I didn't know about it but it was so useful and life-changing and now I don't think I'll ever do another juce project without it. Once it clicked, it forced a cleaner architecture than I would have written on my own. Routing a multi-rate signal through isolated, modular stages turned out to be significantly easier to reason about than one tangled processing block would have been.


**I now know what distortion means and I know the math behind it. Win win!!!!**

---

Shoutouts to the ones who came before us and modeled acoustic effects digitally. 
It's always nice to learn of a scientific advancement that quietly influences your 
daily life greatly.