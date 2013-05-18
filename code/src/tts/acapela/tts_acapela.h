#ifndef BOOX_LIB_TTS_ACAPELA_H_
#define BOOX_LIB_TTS_ACAPELA_H_

#include "../tts_interface.h"
#include "src/speak_lib.h"

namespace tts
{

/// Acapela based tts backend.
class AcapelaImpl : public TTSInterface
{
public:
    AcapelaImpl();
    ~AcapelaImpl();

public:
    virtual bool initialize(const QLocale & locale, Sound & sound);
    virtual bool synthText(const QString & text);
    virtual void stop() { stop_ = true; }

private:
    bool create(const QLocale & locale);
    bool destroy();
    bool isStopped() { return stop_; }
    ulong process(char *in_data, const ulong size, char **out_data);
    static int synthCallback(short *wav, int numsamples, espeak_EVENT *events);

private:
    QFile file_;
    bool initialized_;
    bool stop_;
    QByteArray data_;
};

}

#endif
