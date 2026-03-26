#pragma once
#include <vector>
#include <signalsmith-stretch/signalsmith-stretch.h>

class PitchShifter {
public:
    PitchShifter(int sampleRate, int maxFrames, float semitones)
        : m_channels_(1),
          m_sampleRate_(sampleRate),
          m_inBuffer_(maxFrames, 0.0f),
          m_outBuffer_(maxFrames, 0.0f) {
        m_stretch_.presetCheaper(m_channels_, m_sampleRate_, true);
        m_stretch_.setTransposeSemitones(semitones);
        m_stretch_.setFormantBase(800.0f / m_sampleRate_);
        // m_stretch_.setFormantBase(0); 
        m_stretch_.setFormantFactor(1.0f);
    }

    void setSemitones(float semitones) {
        m_stretch_.setTransposeSemitones(semitones);
    }

    void processWetMono(const float* input, float* output, int frames) {
        if (frames > static_cast<int>(m_inBuffer_.size()) ||
            frames > static_cast<int>(m_outBuffer_.size())) {
            for (int i = 0; i < frames; ++i) output[i] = 0.0f;
            return;
        }

        for (int i = 0; i < frames; ++i) {
            m_inBuffer_[i] = input ? input[i] : 0.0f;
        }

        float* inPtrs[1]  = { m_inBuffer_.data() };
        float* outPtrs[1] = { m_outBuffer_.data() };

        m_stretch_.process(inPtrs, frames, outPtrs, frames);

        for (int i = 0; i < frames; ++i) {
            output[i] = m_outBuffer_[i];
        }
    }

private:
    int m_channels_;
    int m_sampleRate_;
    std::vector<float> m_inBuffer_;
    std::vector<float> m_outBuffer_;
    signalsmith::stretch::SignalsmithStretch<float> m_stretch_;
};