// static int passthroughCallback(const void* inputBuffer,
//                          void* outputBuffer,
//                          unsigned long framesPerBuffer,
//                          const PaStreamCallbackTimeInfo*,
//                          PaStreamCallbackFlags,
//                          void* userData)
// {
//     const auto* in = static_cast<const float*>(inputBuffer);
//     auto* out = static_cast<float*>(outputBuffer);
//     auto* state = static_cast<SineState*>(userData);

//     for (unsigned long i = 0; i < framesPerBuffer; ++i)
//     {
//         // stereo output
//         float sample = *in++;
//         *out++ = sample;
//         *out++ = sample;
//     }

//     return paContinue;
// }