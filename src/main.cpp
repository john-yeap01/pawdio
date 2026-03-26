#include <iostream>
#include "portaudio.h"
#include "PitchShifter.h"
#include "Mixer.h"

constexpr int kSampleRate = 44100;
constexpr int kFramesPerBuffer = 1024;

// struct AudioState {
//     Mixer harmonizer;
//     std::vector<float> monoOut;

//     AudioState() : harmonizer(kSampleRate, kFramesPerBuffer),
//                    monoOut(kFramesPerBuffer, 0.0f) {}
// };

struct AudioState {
    PitchShifter shifter;
    std::vector<float> wetOut;

    AudioState()
        : shifter(kSampleRate, 7.0f),
          wetOut(shifter.blockSize(), 0.0f) {}
};

static void checkPaError(PaError err, const char* step)
{
    if (err != paNoError)
    {
        std::cerr << step << " failed: " << Pa_GetErrorText(err) << '\n';
        std::exit(1);
    }
}

// static int audioCallback(const void* inputBuffer,
//                          void* outputBuffer,
//                          unsigned long framesPerBuffer,
//                          const PaStreamCallbackTimeInfo*,
//                          PaStreamCallbackFlags,
//                          void* userData)
// {
//     auto* state = static_cast<AudioState*>(userData);
//     const auto* in = static_cast<const float*>(inputBuffer);
//     auto* out = static_cast<float*>(outputBuffer);

//     state->harmonizer.processMono(in, state->monoOut.data(),
//                                static_cast<int>(framesPerBuffer));

//     for (unsigned long i = 0; i < framesPerBuffer; ++i) {
//         float s = state->monoOut[i];
//         *out++ = s;
//         *out++ = s;
//     }

//     return paContinue;
// }

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

    if (static_cast<int>(framesPerBuffer) != state->shifter.blockSize()) {
        for (unsigned long i = 0; i < framesPerBuffer; ++i) {
            *out++ = 0.0f;
            *out++ = 0.0f;
        }
        return paContinue;
    }

    state->shifter.processWetMono(in, state->wetOut.data(),
                                  static_cast<int>(framesPerBuffer));

    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        float dry = in ? in[i] : 0.0f;
        float wet = state->wetOut[i];

        float mixed = 0.8f * dry + 0.55f * wet;

        if (mixed > 1.0f) mixed = 1.0f;
        if (mixed < -1.0f) mixed = -1.0f;

        *out++ = mixed;
        *out++ = mixed;
    }

    return paContinue;
}

int main()
{
    PaStream* stream = nullptr;
    AudioState state;

    unsigned long framesPerBuffer =
        static_cast<unsigned long>(state.shifter.blockSize());

    checkPaError(Pa_Initialize(), "Pa_Initialize");

    checkPaError(
        Pa_OpenDefaultStream(
            &stream,
            1,                  // mono input
            2,                  // stereo output
            paFloat32,
            kSampleRate,
            framesPerBuffer,
            audioCallback,
            &state),
        "Pa_OpenDefaultStream");

    checkPaError(Pa_StartStream(stream), "Pa_StartStream");

    std::cout << "Processing microphone input with Rubber Band...\n";
    Pa_Sleep(10000);

    checkPaError(Pa_StopStream(stream), "Pa_StopStream");
    checkPaError(Pa_CloseStream(stream), "Pa_CloseStream");
    checkPaError(Pa_Terminate(), "Pa_Terminate");

    return 0;
}