#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <rubberband/RubberBandLiveShifter.h>

class PitchShifter {
public:
    PitchShifter(int sampleRate, float semitones)
        : m_sampleRate_(sampleRate),
          m_shifter_(
              static_cast<size_t>(sampleRate),
              1, // mono
              RubberBand::RubberBandLiveShifter::OptionWindowShort |
              RubberBand::RubberBandLiveShifter::OptionFormantPreserved
          )
    {
        setSemitones(semitones);

        m_blockSize_ = static_cast<int>(m_shifter_.getBlockSize());
        m_startDelay_ = static_cast<int>(m_shifter_.getStartDelay());
        m_delayRemaining_ = m_startDelay_;

        m_inBuffer_.assign(m_blockSize_, 0.0f);
        m_outBuffer_.assign(m_blockSize_, 0.0f);
    }

    int blockSize() const {
        return m_blockSize_;
    }

    void setSemitones(float semitones) {
        double ratio = std::pow(2.0, semitones / 12.0);
        m_shifter_.setPitchScale(ratio);
    }

    void reset() {
        m_shifter_.reset();
        m_delayRemaining_ = m_startDelay_;
        std::fill(m_inBuffer_.begin(), m_inBuffer_.end(), 0.0f);
        std::fill(m_outBuffer_.begin(), m_outBuffer_.end(), 0.0f);
    }

    // Wet-only mono processing
    void processWetMono(const float* input, float* output, int frames) {
        if (frames != m_blockSize_) {
            for (int i = 0; i < frames; ++i) output[i] = 0.0f;
            return;
        }

        for (int i = 0; i < frames; ++i) {
            m_inBuffer_[i] = input ? input[i] : 0.0f;
        }

        const float* inPtrs[1] = { m_inBuffer_.data() };
        float* outPtrs[1] = { m_outBuffer_.data() };

        m_shifter_.shift(inPtrs, outPtrs);

        for (int i = 0; i < frames; ++i) {
            if (m_delayRemaining_ > 0) {
                output[i] = 0.0f;
                --m_delayRemaining_;
            } else {
                output[i] = m_outBuffer_[i];
            }
        }
    }

private:
    int m_sampleRate_;
    int m_blockSize_ = 0;
    int m_startDelay_ = 0;
    int m_delayRemaining_ = 0;

    RubberBand::RubberBandLiveShifter m_shifter_;
    std::vector<float> m_inBuffer_;
    std::vector<float> m_outBuffer_;
};

// #pragma once
// #include <vector>
// #include <signalsmith-stretch/signalsmith-stretch.h>

// class PitchShifter {
// public:
//     PitchShifter(int sampleRate, int maxFrames, float semitones)
//         : m_channels_(1),
//           m_sampleRate_(sampleRate),
//           m_inBuffer_(maxFrames, 0.0f),
//           m_outBuffer_(maxFrames, 0.0f) {
//         m_stretch_.presetCheaper(m_channels_, m_sampleRate_, true);
//         m_stretch_.setTransposeSemitones(semitones);
//         m_stretch_.setFormantBase(800.0f / m_sampleRate_);
//         // m_stretch_.setFormantBase(0); 
//         m_stretch_.setFormantFactor(1.0f);
//     }

//     void setSemitones(float semitones) {
//         m_stretch_.setTransposeSemitones(semitones);
//     }

//     void processWetMono(const float* input, float* output, int frames) {
//         if (frames > static_cast<int>(m_inBuffer_.size()) ||
//             frames > static_cast<int>(m_outBuffer_.size())) {
//             for (int i = 0; i < frames; ++i) output[i] = 0.0f;
//             return;
//         }

//         for (int i = 0; i < frames; ++i) {
//             m_inBuffer_[i] = input ? input[i] : 0.0f;
//         }

//         float* inPtrs[1]  = { m_inBuffer_.data() };
//         float* outPtrs[1] = { m_outBuffer_.data() };

//         m_stretch_.process(inPtrs, frames, outPtrs, frames);

//         for (int i = 0; i < frames; ++i) {
//             output[i] = m_outBuffer_[i];
//         }
//     }

// private:
//     int m_channels_;
//     int m_sampleRate_;
//     std::vector<float> m_inBuffer_;
//     std::vector<float> m_outBuffer_;
//     signalsmith::stretch::SignalsmithStretch<float> m_stretch_;
// };