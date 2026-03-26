#include <iostream>
#include "portaudio.h"
#include "PitchShifter.h"
#include "Mixer.h"

constexpr int kSampleRate = 44100;
constexpr int kFramesPerBuffer = 1024;

struct AudioState {
    Mixer harmonizer;
    std::vector<float> monoOut;

    AudioState() : harmonizer(kSampleRate, kFramesPerBuffer),
                   monoOut(kFramesPerBuffer, 0.0f) {}
};

static void checkPaError(PaError err, const char* step)
{
    if (err != paNoError)
    {
        std::cerr << step << " failed: " << Pa_GetErrorText(err) << '\n';
        std::exit(1);
    }
}

static int audioCallback(const void* inputBuffer,
                         void* outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo*,
                         PaStreamCallbackFlags,
                         void* userData)
{
    auto* state = static_cast<AudioState*>(userData);
    const auto* in = static_cast<const float*>(inputBuffer);
    auto* out = static_cast<float*>(outputBuffer);

    state->harmonizer.processMono(in, state->monoOut.data(),
                               static_cast<int>(framesPerBuffer));

    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float s = state->monoOut[i];
        *out++ = s;
        *out++ = s;
    }

    return paContinue;
}

int main()
{
    PaStream* stream = nullptr;
    AudioState state;

    checkPaError(Pa_Initialize(), "Pa_Initialize");

    checkPaError(
        Pa_OpenDefaultStream(
            &stream,
            1,                  // 1 input channels
            2,                  // stereo output
            paFloat32,          // 32-bit floating point
            kSampleRate,
            kFramesPerBuffer,
            audioCallback,
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