#ifndef FFT_H
#define FFT_H

#include <string.h>
#include <math.h>
#include <stdio.h>

#define M_PI 3.14159265358979323846
#define MAX_FRAME_LENGTH 16384

class FFT
{
public:
    FFT(float sampleRate, long fftFrameSize, long osamp);
    void smbFft(float *fftBuffer, long fftFrameSize, long sign);
    void processPitchShift(float* indata, float* outdata,
                           int numSampsToProcess, float pitchShift);
    void processForwardOnly(float* indata, float* outdata,
                            int numSampsToProcess);
    void applyPitchShift(float pitchShift);
    void applyAnalysis();
    void applySynthesis();

private:
    long fftFrameSize;
    long osamp;

    float gInFIFO[MAX_FRAME_LENGTH];
    float gOutFIFO[MAX_FRAME_LENGTH];
    float gFFTworksp[2*MAX_FRAME_LENGTH];
    float gLastPhase[MAX_FRAME_LENGTH/2+1];
    float gSumPhase[MAX_FRAME_LENGTH/2+1];
    float gOutputAccum[2*MAX_FRAME_LENGTH];
    float gAnaFreq[MAX_FRAME_LENGTH];
    float gAnaMagn[MAX_FRAME_LENGTH];
    float gSynFreq[MAX_FRAME_LENGTH];
    float gSynMagn[MAX_FRAME_LENGTH];
    long gRover = false, gInit = false;
    double window;
    double freqPerBin, expct;
    long qpd, index, inFifoLatency, stepSize, fftFrameSize2;
};

#endif // FFT_H
