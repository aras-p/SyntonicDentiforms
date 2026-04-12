#include "MusicPlayer.h"

#include <algorithm>
#include <atomic>
#include <mutex>
#include <vector>

#define SOKOL_AUDIO_IMPL
#include "../../external/sokol_audio.h"

#include "../../external/stb_vorbis.c"

struct CMusicPlayer::Impl
{
    static constexpr int kRequestedSampleRate = 44100;
    static constexpr int kRequestedChannels = 2;
    static constexpr int kDecodeBufferFrames = 4096;

    stb_vorbis *vorbis = nullptr;
    stb_vorbis_info info{};

    std::atomic<uint64_t> decodedSourceFrames{0};

    std::vector<float> decodeBuffer; // interleaved source samples
    int decodeBufferFrames = 0;
    int decodeBufferPos = 0;
    bool eof = false;

    double srcFramesPerDstFrame = 1.0;
    double srcCursor = 0.0;

    mutable std::mutex stateMutex;

    static void audioCallback(float *buffer, int numFrames, int numChannels, void *userData)
    {
        static_cast<Impl *>(userData)->render(buffer, numFrames, numChannels);
    }

    void render(float *buffer, int numFrames, int numChannels)
    {
        std::fill(buffer, buffer + numFrames * numChannels, 0.0f);

        if (!vorbis || eof)
            return;

        for (int dstFrame = 0; dstFrame < numFrames; ++dstFrame)
        {
            float l = 0.0f;
            float r = 0.0f;
            if (!readResampledStereoFrame(l, r))
                break;

            if (numChannels == 1)
            {
                buffer[dstFrame] = 0.5f * (l + r);
            }
            else
            {
                buffer[dstFrame * numChannels + 0] = l;
                buffer[dstFrame * numChannels + 1] = r;
                for (int c = 2; c < numChannels; ++c)
                {
                    buffer[dstFrame * numChannels + c] = 0.0f;
                }
            }
        }
    }

    bool refillDecodeBuffer()
    {
        if (!vorbis || eof)
            return false;

        const int srcChannels = info.channels;
        const int maxFloats = kDecodeBufferFrames * srcChannels;

        const int framesRead = stb_vorbis_get_samples_float_interleaved(
            vorbis,
            srcChannels,
            decodeBuffer.data(),
            maxFloats);

        decodeBufferFrames = framesRead;
        decodeBufferPos = 0;

        if (framesRead <= 0)
        {
            eof = true;
            return false;
        }
        return true;
    }

    bool getDecodedFrameStereo(int frameIndex, float &l, float &r)
    {
        if (frameIndex < 0 || frameIndex >= decodeBufferFrames)
        {
            l = 0.0f;
            r = 0.0f;
            return false;
        }

        const int ch = info.channels;
        const int base = frameIndex * ch;

        if (ch == 1)
        {
            l = decodeBuffer[base];
            r = l;
        }
        else
        {
            l = decodeBuffer[base + 0];
            r = decodeBuffer[base + 1];
        }
        return true;
    }

    bool readResampledStereoFrame(float &l, float &r)
    {
        if (!vorbis || eof)
        {
            l = 0.0f;
            r = 0.0f;
            return false;
        }

        if (decodeBufferFrames == 0)
        {
            if (!refillDecodeBuffer())
            {
                l = 0.0f;
                r = 0.0f;
                return false;
            }
        }

        int i0 = static_cast<int>(srcCursor);
        float frac = static_cast<float>(srcCursor - double(i0));

        while (i0 >= decodeBufferFrames)
        {
            srcCursor -= decodeBufferFrames;
            decodedSourceFrames.fetch_add(decodeBufferFrames, std::memory_order_relaxed);
            if (!refillDecodeBuffer())
            {
                l = 0.0f;
                r = 0.0f;
                return false;
            }
            i0 = static_cast<int>(srcCursor);
            frac = static_cast<float>(srcCursor - double(i0));
        }

        float l0 = 0.0f, r0 = 0.0f;
        float l1 = 0.0f, r1 = 0.0f;

        if (!getDecodedFrameStereo(i0, l0, r0))
        {
            l = 0.0f;
            r = 0.0f;
            return false;
        }

        if (!getDecodedFrameStereo(i0 + 1, l1, r1))
        {
            l1 = l0;
            r1 = r0;
        }

        l = l0 + (l1 - l0) * frac;
        r = r0 + (r1 - r0) * frac;

        srcCursor += srcFramesPerDstFrame;
        return true;
    }

    double currentTimeSeconds() const
    {
        std::lock_guard<std::mutex> lock(stateMutex);
        if (!vorbis || info.sample_rate <= 0)
            return 0.0;

        const double absoluteSourceFrames =
            double(decodedSourceFrames.load(std::memory_order_relaxed)) + srcCursor;

        return absoluteSourceFrames / double(info.sample_rate);
    }

    void closeFile()
    {
        std::lock_guard<std::mutex> lock(stateMutex);

        if (vorbis)
        {
            stb_vorbis_close(vorbis);
            vorbis = nullptr;
        }

        info = {};
        decodedSourceFrames.store(0, std::memory_order_relaxed);
        decodeBufferFrames = 0;
        decodeBufferPos = 0;
        eof = false;
        srcFramesPerDstFrame = 1.0;
        srcCursor = 0.0;
        decodeBuffer.clear();
    }
};

CMusicPlayer::CMusicPlayer()
{
    m_impl = new Impl();

    saudio_desc desc = {};
    desc.sample_rate = Impl::kRequestedSampleRate;
    desc.num_channels = Impl::kRequestedChannels;
    desc.stream_userdata_cb = &Impl::audioCallback;
    desc.user_data = m_impl;
    saudio_setup(&desc);
}

CMusicPlayer::~CMusicPlayer()
{
    if (m_impl)
    {
        m_impl->closeFile();
    }
    saudio_shutdown();
    delete m_impl;
    m_impl = nullptr;
}

void CMusicPlayer::play(const char *fileName)
{
    m_impl->closeFile();

    if (!fileName || !fileName[0])
        return;

    int error = 0;
    stb_vorbis *v = stb_vorbis_open_filename(fileName, &error, nullptr);
    if (!v)
        return;

    std::lock_guard<std::mutex> lock(m_impl->stateMutex);

    m_impl->vorbis = v;
    m_impl->info = stb_vorbis_get_info(v);
    m_impl->decodedSourceFrames.store(0, std::memory_order_relaxed);
    m_impl->eof = false;
    m_impl->srcCursor = 0.0;
    m_impl->decodeBufferFrames = 0;
    m_impl->decodeBufferPos = 0;
    m_impl->decodeBuffer.resize(Impl::kDecodeBufferFrames * std::max(1, m_impl->info.channels));

    const int outRate = saudio_sample_rate();
    m_impl->srcFramesPerDstFrame = (outRate > 0)
                                       ? double(m_impl->info.sample_rate) / double(outRate)
                                       : 1.0;
}

double CMusicPlayer::getTime() const
{
    return m_impl->currentTimeSeconds();
}
