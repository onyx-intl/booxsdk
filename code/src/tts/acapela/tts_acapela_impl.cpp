
#include "tts_acapela.h"

namespace tts
{
static const int BPS = 16;
static const int CHANNELS = 2;
static const int FREQ = 44100;

/// Callback function.
int AcapelaImpl::synthCallback(short *wav, int numsamples, espeak_EVENT *events)
{
    char * d = reinterpret_cast<char *>(wav);
    AcapelaImpl *object = static_cast<AcapelaImpl *>(events->user_data);
    if (object)
    {
        // Convert mono to stereo. The espeak only generates mono data.
        for(int i = 0; i < numsamples; ++i)
        {
            QByteArray x(d, 2);
            object->data_.append(x);
            object->data_.append(x);
            d += 2;
        }
    }
    return 0;
}


AcapelaImpl::AcapelaImpl()
{
    initialized_ = false;
    stop_ = false;
}


AcapelaImpl::~AcapelaImpl()
{
    bs2b_clear(bs2b_handler_);

    destroy();
}

bool AcapelaImpl::initialize(const QLocale & locale, Sound & sound)
{
    sound.setBitsPerSample(BPS);
    sound.setChannels(CHANNELS);
    sound.setSamplingRate(FREQ);
    sound.setRec();
    return create(locale);
}

ulong AcapelaImpl::process(char *in_data, const ulong size, char **out_data)
{
    memcpy(*out_data, in_data, size);
    uint samples = size / (BPS / 8) / 2;
    int16_t * data = reinterpret_cast<int16_t *>(*out_data);
    bs2b_cross_feed_s16le(bs2b_handler_, data, samples);
    return size;
}

bool AcapelaImpl::synthText(const QString & text)
{
    stop_ = false;
    data_.clear();

    QByteArray d = text.toUtf8();
    int synth_flags = espeakPHONEMES | espeakENDPAUSE | espeakCHARS_UTF8;
    espeak_Synth(d.constData(), d.size() + 1, 0, POS_CHARACTER, 0, synth_flags, 0, this);

    emit synthDone(true, data_);
    return true;
}

bool AcapelaImpl::create(const QLocale & locale)
{
    if (initialized_)
    {
        return true;
    }

    QDir dir(QDir::home());
    QString path = SHARE_ROOT;
    if (!path.isEmpty())
    {
        dir.cd(path);
    }

    qDebug("Resource path %s", qPrintable(dir.absolutePath()));
    /* nscCreateServerContext */
    nscRESULT Result;
    if ((Result = nscCreateServerContext(NSC_AF_LOCAL,0,"127.0.0.1",&hSrv)) != NSC_OK)
    {
        printf("ERROR: nscCreateServerContext return  %d \n",Result);
        return false;
    }

    if ((Result = nscCreateDispatcher(&hDispatch)) != NSC_OK)
    {
        printf("ERROR: nscCreateDispatcher return  %d \n",Result);
        return false;
    }

    nscSetVoicesPaths(hSrv, dir.absolutePath().toLocal8Bits());
    if( (Result = nscGetServerInfo(hSrv, &SrvInfo))!=NSC_OK)
    {
        printf("ERROR: nscGetServerInfo return %d \n",Result);
        return false;
    }

    initialized_ = true;
    return true;
}

bool AcapelaImpl::destroy()
{
    if (!initialized_)
    {
        return false;
    }
    initialized_ = false;
    return true;
}

void AcapelaImpl::collectAllPlayers()
{
    NSC_FINDVOICE_DATA FindVoice;
    if (ptabFindVoice) {
        free(ptabFindVoice);
    }
    ptabFindVoice=(NSC_FINDVOICE_DATA *)malloc(sizeof(NSC_FINDVOICE_DATA) * SrvInfo.nMaxNbVoice);
    if (ptabFindVoice==NULL)
    {
        printf("ERROR: not enough memory\n");
        return ;
    }

    printf("Searching voice...\n");
    if( (Result = nscFindFirstVoice(hSrv,NULL,0,0,0,&FindVoice,&hVoice))==NSC_OK)
    {
        ptabFindVoice[idtabFindVoice++]=FindVoice;
        while((Result = nscFindNextVoice(hVoice,&FindVoice))==NSC_OK)
        {
            ptabFindVoice[idtabFindVoice++]=FindVoice;
        }
    }
    else
    {
        printf("ERROR: nscFindFirstVoice return  %d \n",Result);
        return;
    }
    nscCloseFindVoice(hVoice);
    printf("%d voices found:\n",idtabFindVoice);


    nbvoice = idtabFindVoice;
    idtabFindVoice = 0;
    while (nbvoice>0)
    {
        printf("  %s\n",ptabFindVoice[idtabFindVoice].cVoiceName);
        idtabFindVoice++;
        nbvoice--;
    }

    /* Init channel  */
    printf("\n");
    printf("Enter a voice name? : ");
    fgets(voice,sizeof(voice),stdin);
    /*Voice name must match exactly the enumerated value so terminating \n char must be removed */
    voice[strlen(voice)-1]='\0';
    printf("Channel initialising with %s ... ",voice);
    if( (Result = nscInitChannel(hSrv,voice,0,0,hDispatch,&ChId))==NSC_OK){
        printf("Channel initialised\n");
    }
    else{
        printf("ERROR: nscInitChannel return  %d \n",Result);
        return -1;
    }

    /* Lock channel */
    printf("Channel locking...   ");
    if( (Result = nscLockChannel(hSrv,ChId,hDispatch,&hTTS))==NSC_OK){
        printf("Channel locked\n");
    }
    else
    {
        printf("ERROR: nscLockChannel return  %d \n",Result);
        return -1;
    }

    /* AddText channel */
    printf("\n");
    tadd[0]='\0';
    while(tadd[0]=='\0'){
        printf("Text to add to channel? : ");
        fgets(tadd,sizeof(tadd),stdin);
        tadd[strlen(tadd)-1]='\0';
    }
    int IdText = 1;
    printf("Text %i adding...   ", IdText);

    if( (Result = nscAddTextEx(hTTS,"UTF-8",tadd,strlen(tadd),(void *)IdText))==NSC_OK){
        //if( (Result = nscAddText(hTTS,tadd,(void *)IdText))==NSC_OK){
        printf("Text %i added\n", IdText);
    }
    else{
        printf("ERROR: nscAddText return  %d \n",Result);
        return -1;
    }

}


}
