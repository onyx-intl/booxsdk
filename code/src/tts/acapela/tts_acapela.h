#ifndef BOOX_LIB_TTS_ACAPELA_H_
#define BOOX_LIB_TTS_ACAPELA_H_

#include "../tts_interface.h"
#include "src/audio.h"
#include "src/nscube.h"


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
    void collectAllPlayers();

private:
    QFile file_;
    bool initialized_;
    bool stop_;
    QByteArray data_;

    nscHSRV                 hSrv;               /* Handle of an Acapela Telecom server context returned by nscCreateServerContext */
    NSC_SRVINFO_DATA        SrvInfo;            /* This structure describes informations about server */

    NSC_FINDVOICE_DATA *ptabFindVoice;
    nscHANDLE               hVoice;             /* Handle to be used in subsequent calls to nscFindNextVoice */
    nscHANDLE               hDispatch;          /* Handle will be passed to the event dispatching functions and returned by nscCreateDispatcher */
    nscHANDLE               hTTS;               /* Handle of channel returned by nscLockChannel */
    nscCHANID               ChId;               /* Id Of channel returned by nscInitChannel */
    NSC_EXEC_DATA           ExecData;           /* Informations using by a call to nscExecChannel */
    NSC_FINDVOICE_DATA  *ptabFindVoice;
    int                 idtabFindVoice = 0;
    char IPadr[20];
    int nbvoice;
    char voice[50];
    char tadd[500];
    char NomFich[200];
    char output[500];
    totalSamples =0;

};

}

#endif
