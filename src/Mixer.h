#pragma once
#include <vector>
#include "PitchShifter.h"

class Mixer {
public:
    Mixer(int sampleRate, int maxFrames)
        : m_voice1_(sampleRate, maxFrames, 4.0f),   // major third
          m_voice2_(sampleRate, maxFrames, -5.0f),   // perfect fifth
          m_voice1Buffer_(maxFrames, 0.0f),
          m_voice2Buffer_(maxFrames, 0.0f),
          m_dry_(0.75f),
          m_wet1_(0.45f),
          m_wet2_(0.45f) {}

    void setVoice1Semitones(float semitones) {
        m_voice1_.setSemitones(semitones);
    }

    void setVoice2Semitones(float semitones) {
        m_voice2_.setSemitones(semitones);
    }

    void setMix(float dry, float wet1, float wet2) {
        m_dry_ = dry;
        m_wet1_ = wet1;
        m_wet2_ = wet2;
    }

    void processMono(const float* input, float* output, int frames) {
        m_voice1_.processWetMono(input, m_voice1Buffer_.data(), frames);
        m_voice2_.processWetMono(input, m_voice2Buffer_.data(), frames);

        for (int i = 0; i < frames; ++i) {
            float dry = input ? input[i] : 0.0f;

            float mixed =
                m_dry_  * dry +
                m_wet1_ * m_voice1Buffer_[i] +
                m_wet2_ * m_voice2Buffer_[i];

            if (mixed > 1.0f) mixed = 1.0f;
            if (mixed < -1.0f) mixed = -1.0f;

            output[i] = mixed;
        }
    }

private:
    PitchShifter m_voice1_;
    PitchShifter m_voice2_;

    std::vector<float> m_voice1Buffer_;
    std::vector<float> m_voice2Buffer_;

    float m_dry_;
    float m_wet1_;
    float m_wet2_;
};