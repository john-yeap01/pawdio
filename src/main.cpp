#include <cmath>
#include <iostream>
#include "portaudio.h"

constexpr double kPi = 3.14159265358979323846;
constexpr int kSampleRate = 44100;
constexpr int kFramesPerBuffer = 256;
constexpr float kAmplitude = 0.2f;
constexpr double kFrequency = 440.0;

struct SineState
{
    double phase = 0.0;
    double phaseInc = 2.0 * kPi * kFrequency / static_cast<double>(kSampleRate);
};

static void checkPaError(PaError err, const char* step)
{
    if (err != paNoError)
    {
        std::cerr << step << " failed: " << Pa_GetErrorText(err) << '\n';
        std::exit(1);
    }
}

// static int audioCallback(const void*,
//                          void* outputBuffer,
//                          unsigned long framesPerBuffer,
//                          const PaStreamCallbackTimeInfo*,
//                          PaStreamCallbackFlags,
//                          void* userData)
// {
//     auto* out = static_cast<float*>(outputBuffer);
//     auto* state = static_cast<SineState*>(userData);

//     for (unsigned long i = 0; i < framesPerBuffer; ++i)
//     {
//         float s = kAmplitude * static_cast<float>(std::sin(state->phase));

//         // stereo output
//         *out++ = s;
//         *out++ = s;

//         state->phase += state->phaseInc;
//         if (state->phase >= 2.0 * kPi)
//             state->phase -= 2.0 * kPi;
//     }

//     return paContinue;
// }

static int passthroughCallback(const void* inputBuffer,
                         void* outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo*,
                         PaStreamCallbackFlags,
                         void* userData)
{
    const auto* in = static_cast<const float*>(inputBuffer);
    auto* out = static_cast<float*>(outputBuffer);
    auto* state = static_cast<SineState*>(userData);

    for (unsigned long i = 0; i < framesPerBuffer; ++i)
    {
        // stereo output
        float sample = *in++;
        *out++ = sample;
        *out++ = sample;
    }

    return paContinue;
}

int main()
{
    PaStream* stream = nullptr;
    SineState state;

    checkPaError(Pa_Initialize(), "Pa_Initialize");

    checkPaError(
        Pa_OpenDefaultStream(
            &stream,
            1,                  // no input channels
            2,                  // stereo output
            paFloat32,          // 32-bit floating point
            kSampleRate,
            kFramesPerBuffer,
            passthroughCallback,
            &state),
        "Pa_OpenDefaultStream");

    checkPaError(Pa_StartStream(stream), "Pa_StartStream");

    std::cout << "Playing tone for 3 seconds...\n";
    Pa_Sleep(10000);

    checkPaError(Pa_StopStream(stream), "Pa_StopStream");
    checkPaError(Pa_CloseStream(stream), "Pa_CloseStream");
    checkPaError(Pa_Terminate(), "Pa_Terminate");

    return 0;
}