#pragma once
#include <vector>
#include <signalsmith-stretch/signalsmith-stretch.h>

class PitchShifter {
public:
    PitchShifter(int sampleRate, int maxFrames)
        : m_channels_(1),
          m_sampleRate_(sampleRate),
          m_inBuffer_(maxFrames, 0.0f),
          m_outBuffer_(maxFrames, 0.0f) {
        m_stretch_.presetCheaper(m_channels_, m_sampleRate_, true);
        m_stretch_.setTransposeSemitones(7.0f); // test: perfect fifth up
    }

    void processMono(const float* input, float* output, int frames) {
        if (frames > static_cast<int>(m_inBuffer_.size()) ||
            frames > static_cast<int>(m_outBuffer_.size())) {
            return; // later: handle properly, but do not allocate in callback
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


