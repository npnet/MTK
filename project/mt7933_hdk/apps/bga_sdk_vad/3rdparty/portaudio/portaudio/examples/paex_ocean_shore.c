/** @file paex_ocean_shore.c
    @ingroup examples_src
    @brief Generate Pink Noise using Gardner method, and make "waves". Provides an example of how to
           post stuff to/from the audio callback using lock-free FIFOs implemented by the PA ringbuffer.

    Optimization suggested by James McCartney uses a tree
    to select which random value to replace.
<pre>
    x x x x x x x x x x x x x x x x
    x   x   x   x   x   x   x   x
    x       x       x       x
     x               x
       x
</pre>
    Tree is generated by counting trailing zeros in an increasing index.
    When the index is zero, no random number is selected.

    @author Phil Burk  http://www.softsynth.com
            Robert Bielik
*/
/*
 * $Id$
 *
 * This program uses the PortAudio Portable Audio Library.
 * For more information see: http://www.portaudio.com
 * Copyright (c) 1999-2000 Ross Bencina and Phil Burk
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * The text above constitutes the entire PortAudio license; however,
 * the PortAudio community also makes the following non-binding requests:
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version. It is also
 * requested that these non-binding requests be included along with the
 * license above.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "portaudio.h"
#include "pa_ringbuffer.h"
#include "pa_util.h"

#define PINK_MAX_RANDOM_ROWS   (30)
#define PINK_RANDOM_BITS       (24)
#define PINK_RANDOM_SHIFT      ((sizeof(long)*8)-PINK_RANDOM_BITS)

typedef struct {
    long      pink_Rows[PINK_MAX_RANDOM_ROWS];
    long      pink_RunningSum;   /* Used to optimize summing of generators. */
    int       pink_Index;        /* Incremented each sample. */
    int       pink_IndexMask;    /* Index wrapped by ANDing with this mask. */
    float     pink_Scalar;       /* Used to scale within range of -1.0 to +1.0 */
}
PinkNoise;

typedef struct {
    float       bq_b0;
    float       bq_b1;
    float       bq_b2;
    float       bq_a1;
    float       bq_a2;
} BiQuad;

typedef enum {
    State_kAttack,
    State_kPreDecay,
    State_kDecay,
    State_kCnt,
} EnvState;

typedef struct {
    PinkNoise   wave_left;
    PinkNoise   wave_right;

    BiQuad      wave_bq_coeffs;
    float       wave_bq_left[2];
    float       wave_bq_right[2];

    EnvState    wave_envelope_state;
    float       wave_envelope_level;
    float       wave_envelope_max_level;
    float       wave_pan_left;
    float       wave_pan_right;
    float       wave_attack_incr;
    float       wave_decay_incr;

} OceanWave;

/* Prototypes */
static unsigned long GenerateRandomNumber(void);
void InitializePinkNoise(PinkNoise *pink, int numRows);
float GeneratePinkNoise(PinkNoise *pink);
unsigned GenerateWave(OceanWave *wave, float *output, unsigned noOfFrames);

/************************************************************/
/* Calculate pseudo-random 32 bit number based on linear congruential method. */
static unsigned long GenerateRandomNumber(void)
{
    /* Change this seed for different random sequences. */
    static unsigned long randSeed = 22222;
    randSeed = (randSeed * 196314165) + 907633515;
    return randSeed;
}

/************************************************************/
/* Setup PinkNoise structure for N rows of generators. */
void InitializePinkNoise(PinkNoise *pink, int numRows)
{
    int i;
    long pmax;
    pink->pink_Index = 0;
    pink->pink_IndexMask = (1 << numRows) - 1;
    /* Calculate maximum possible signed random value. Extra 1 for white noise always added. */
    pmax = (numRows + 1) * (1 << (PINK_RANDOM_BITS - 1));
    pink->pink_Scalar = 1.0f / pmax;
    /* Initialize rows. */
    for (i = 0; i < numRows; i++)
        pink->pink_Rows[i] = 0;
    pink->pink_RunningSum = 0;
}

/* Generate Pink noise values between -1.0 and +1.0 */
float GeneratePinkNoise(PinkNoise *pink)
{
    long newRandom;
    long sum;
    float output;
    /* Increment and mask index. */
    pink->pink_Index = (pink->pink_Index + 1) & pink->pink_IndexMask;
    /* If index is zero, don't update any random values. */
    if (pink->pink_Index != 0) {
        /* Determine how many trailing zeros in PinkIndex. */
        /* This algorithm will hang if n==0 so test first. */
        int numZeros = 0;
        int n = pink->pink_Index;
        while ((n & 1) == 0) {
            n = n >> 1;
            numZeros++;
        }
        /* Replace the indexed ROWS random value.
         * Subtract and add back to RunningSum instead of adding all the random
         * values together. Only one changes each time.
         */
        pink->pink_RunningSum -= pink->pink_Rows[numZeros];
        newRandom = ((long)GenerateRandomNumber()) >> PINK_RANDOM_SHIFT;
        pink->pink_RunningSum += newRandom;
        pink->pink_Rows[numZeros] = newRandom;
    }

    /* Add extra white noise value. */
    newRandom = ((long)GenerateRandomNumber()) >> PINK_RANDOM_SHIFT;
    sum = pink->pink_RunningSum + newRandom;
    /* Scale to range of -1.0 to 0.9999. */
    output = pink->pink_Scalar * sum;
    return output;
}

float ProcessBiquad(const BiQuad *coeffs, float *memory, float input)
{
    float w = input - coeffs->bq_a1 * memory[0] - coeffs->bq_a2 * memory[1];
    float out = coeffs->bq_b1 * memory[0] + coeffs->bq_b2 * memory[1] + coeffs->bq_b0 * w;
    memory[1] = memory[0];
    memory[0] = w;
    return out;
}

static const float one_over_2Q_LP = 0.3f;
static const float one_over_2Q_HP = 1.0f;

unsigned GenerateWave(OceanWave *wave, float *output, unsigned noOfFrames)
{
    unsigned retval = 0, i;
    float targetLevel, levelIncr, currentLevel;
    switch (wave->wave_envelope_state) {
        case State_kAttack:
            targetLevel = noOfFrames * wave->wave_attack_incr + wave->wave_envelope_level;
            if (targetLevel >= wave->wave_envelope_max_level) {
                /* Go to decay state */
                wave->wave_envelope_state = State_kPreDecay;
                targetLevel = wave->wave_envelope_max_level;
            }
            /* Calculate lowpass biquad coeffs

                alpha = sin(w0)/(2*Q)

                    b0 =  (1 - cos(w0))/2
                    b1 =   1 - cos(w0)
                    b2 =  (1 - cos(w0))/2
                    a0 =   1 + alpha
                    a1 =  -2*cos(w0)
                    a2 =   1 - alpha

                w0 = [0 - pi[
            */
            {
                const float w0 = 3.141592654f * targetLevel / wave->wave_envelope_max_level;
                const float alpha = sinf(w0) * one_over_2Q_LP;
                const float cosw0 = cosf(w0);
                const float a0_fact = 1.0f / (1.0f + alpha);
                wave->wave_bq_coeffs.bq_b1 = (1.0f - cosw0) * a0_fact;
                wave->wave_bq_coeffs.bq_b0 = wave->wave_bq_coeffs.bq_b1 * 0.5f;
                wave->wave_bq_coeffs.bq_b2 = wave->wave_bq_coeffs.bq_b0;
                wave->wave_bq_coeffs.bq_a2 = (1.0f - alpha) * a0_fact;
                wave->wave_bq_coeffs.bq_a1 = -2.0f * cosw0 * a0_fact;
            }
            break;

        case State_kPreDecay:
            /* Reset biquad state */
            memset(wave->wave_bq_left, 0, 2 * sizeof(float));
            memset(wave->wave_bq_right, 0, 2 * sizeof(float));
            wave->wave_envelope_state = State_kDecay;

        /* Deliberate fall-through */

        case State_kDecay:
            targetLevel = noOfFrames * wave->wave_decay_incr + wave->wave_envelope_level;
            if (targetLevel < 0.001f) {
                /* < -60 dB, we're done */
                wave->wave_envelope_state = 3;
                retval = 1;
            }
            /* Calculate highpass biquad coeffs

                alpha = sin(w0)/(2*Q)

                b0 =  (1 + cos(w0))/2
                b1 = -(1 + cos(w0))
                b2 =  (1 + cos(w0))/2
                a0 =   1 + alpha
                a1 =  -2*cos(w0)
                a2 =   1 - alpha

                w0 = [0 - pi/2[
            */
            {
                const float v = targetLevel / wave->wave_envelope_max_level;
                const float w0 = 1.5707963f * (1.0f - (v * v));
                const float alpha = sinf(w0) * one_over_2Q_HP;
                const float cosw0 = cosf(w0);
                const float a0_fact = 1.0f / (1.0f + alpha);
                wave->wave_bq_coeffs.bq_b1 = (float)(- (1 + cosw0) * a0_fact);
                wave->wave_bq_coeffs.bq_b0 = -wave->wave_bq_coeffs.bq_b1 * 0.5f;
                wave->wave_bq_coeffs.bq_b2 = wave->wave_bq_coeffs.bq_b0;
                wave->wave_bq_coeffs.bq_a2 = (float)((1.0 - alpha) * a0_fact);
                wave->wave_bq_coeffs.bq_a1 = (float)(-2.0 * cosw0 * a0_fact);
            }
            break;

        default:
            break;
    }

    currentLevel = wave->wave_envelope_level;
    wave->wave_envelope_level = targetLevel;
    levelIncr = (targetLevel - currentLevel) / noOfFrames;

    for (i = 0; i < noOfFrames; ++i, currentLevel += levelIncr) {
        (*output++) += ProcessBiquad(&wave->wave_bq_coeffs, wave->wave_bq_left, (GeneratePinkNoise(&wave->wave_left))) * currentLevel * wave->wave_pan_left;
        (*output++) += ProcessBiquad(&wave->wave_bq_coeffs, wave->wave_bq_right, (GeneratePinkNoise(&wave->wave_right))) * currentLevel * wave->wave_pan_right;
    }

    return retval;
}


/*******************************************************************/

/* Context for callback routine. */
typedef struct {
    OceanWave          *waves[16];      /* Maximum 16 waves */
    unsigned            noOfActiveWaves;

    /* Ring buffer (FIFO) for "communicating" towards audio callback */
    PaUtilRingBuffer    rBufToRT;
    void               *rBufToRTData;

    /* Ring buffer (FIFO) for "communicating" from audio callback */
    PaUtilRingBuffer    rBufFromRT;
    void               *rBufFromRTData;
}
paTestData;

/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int patestCallback(const void                     *inputBuffer,
                          void                           *outputBuffer,
                          unsigned long                   framesPerBuffer,
                          const PaStreamCallbackTimeInfo *timeInfo,
                          PaStreamCallbackFlags           statusFlags,
                          void                           *userData)
{
    int i;
    paTestData *data = (paTestData *)userData;
    float *out = (float *)outputBuffer;
    (void) inputBuffer; /* Prevent "unused variable" warnings. */

    /* Reset output data first */
    memset(out, 0, framesPerBuffer * 2 * sizeof(float));

    for (i = 0; i < 16; ++i) {
        /* Consume the input queue */
        if (data->waves[i] == 0 && PaUtil_GetRingBufferReadAvailable(&data->rBufToRT)) {
            OceanWave *ptr = 0;
            PaUtil_ReadRingBuffer(&data->rBufToRT, &ptr, 1);
            data->waves[i] = ptr;
        }

        if (data->waves[i] != 0) {
            if (GenerateWave(data->waves[i], out, framesPerBuffer)) {
                /* If wave is "done", post it back to the main thread for deletion */
                PaUtil_WriteRingBuffer(&data->rBufFromRT, &data->waves[i], 1);
                data->waves[i] = 0;
            }
        }
    }
    return paContinue;
}

#define NEW_ROW_SIZE (12 + (8*rand())/RAND_MAX)

OceanWave *InitializeWave(double SR, float attackInSeconds, float maxLevel, float positionLeftRight)
{
    OceanWave *wave = NULL;
    static unsigned lastNoOfRows = 12;
    unsigned newNoOfRows;

    wave = (OceanWave *)PaUtil_AllocateMemory(sizeof(OceanWave));
    if (wave != NULL) {
        InitializePinkNoise(&wave->wave_left, lastNoOfRows);
        while ((newNoOfRows = NEW_ROW_SIZE) == lastNoOfRows);
        InitializePinkNoise(&wave->wave_right, newNoOfRows);
        lastNoOfRows = newNoOfRows;

        wave->wave_envelope_state = State_kAttack;
        wave->wave_envelope_level = 0.f;
        wave->wave_envelope_max_level = maxLevel;
        wave->wave_attack_incr = wave->wave_envelope_max_level / (attackInSeconds * (float)SR);
        wave->wave_decay_incr = - wave->wave_envelope_max_level / (attackInSeconds * 4 * (float)SR);

        wave->wave_pan_left = sqrtf(1.0f - positionLeftRight);
        wave->wave_pan_right = sqrtf(positionLeftRight);
    }
    return wave;
}

static float GenerateFloatRandom(float minValue, float maxValue)
{
    return minValue + ((maxValue - minValue) * rand()) / RAND_MAX;
}

/*******************************************************************/
int main(void);
int main(void)
{
    PaStream           *stream;
    PaError             err;
    paTestData          data = {0};
    PaStreamParameters  outputParameters;
    double              tstamp;
    double              tstart;
    double              tdelta = 0;
    static const double SR  = 44100.0;
    static const int    FPB = 128; /* Frames per buffer: 2.9 ms buffers. */

    /* Initialize communication buffers (queues) */
    data.rBufToRTData = PaUtil_AllocateMemory(sizeof(OceanWave *) * 256);
    if (data.rBufToRTData == NULL) {
        return 1;
    }
    PaUtil_InitializeRingBuffer(&data.rBufToRT, sizeof(OceanWave *), 256, data.rBufToRTData);

    data.rBufFromRTData = PaUtil_AllocateMemory(sizeof(OceanWave *) * 256);
    if (data.rBufFromRTData == NULL) {
        return 1;
    }
    PaUtil_InitializeRingBuffer(&data.rBufFromRT, sizeof(OceanWave *), 256, data.rBufFromRTData);

    err = Pa_Initialize();
    if (err != paNoError)
        goto error;

    /* Open a stereo PortAudio stream so we can hear the result. */
    outputParameters.device = Pa_GetDefaultOutputDevice(); /* Take the default output device. */
    if (outputParameters.device == paNoDevice) {
        fprintf(stderr, "Error: No default output device.\n");
        goto error;
    }
    outputParameters.channelCount = 2;                     /* Stereo output, most likely supported. */
    outputParameters.hostApiSpecificStreamInfo = NULL;
    outputParameters.sampleFormat = paFloat32;             /* 32 bit floating point output. */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    err = Pa_OpenStream(&stream,
                        NULL,                              /* No input. */
                        &outputParameters,
                        SR,                                /* Sample rate. */
                        FPB,                               /* Frames per buffer. */
                        paDitherOff,                       /* Clip but don't dither */
                        patestCallback,
                        &data);
    if (err != paNoError)
        goto error;

    err = Pa_StartStream(stream);
    if (err != paNoError)
        goto error;

    printf("Stereo \"ocean waves\" for one minute...\n");

    tstart = PaUtil_GetTime();
    tstamp = tstart;
    srand((unsigned)time(NULL));

    while ((err = Pa_IsStreamActive(stream)) == 1) {
        const double tcurrent = PaUtil_GetTime();

        /* Delete "waves" that the callback is finished with */
        while (PaUtil_GetRingBufferReadAvailable(&data.rBufFromRT) > 0) {
            OceanWave *ptr = 0;
            PaUtil_ReadRingBuffer(&data.rBufFromRT, &ptr, 1);
            if (ptr != 0) {
                printf("Wave is deleted...\n");
                PaUtil_FreeMemory(ptr);
                --data.noOfActiveWaves;
            }
        }

        if (tcurrent - tstart < 60.0) { /* Only start new "waves" during one minute */
            if (tcurrent >= tstamp) {
                double tdelta = GenerateFloatRandom(1.0f, 4.0f);
                tstamp += tdelta;

                if (data.noOfActiveWaves < 16) {
                    const float attackTime = GenerateFloatRandom(2.0f, 6.0f);
                    const float level = GenerateFloatRandom(0.1f, 1.0f);
                    const float pos = GenerateFloatRandom(0.0f, 1.0f);
                    OceanWave *p = InitializeWave(SR, attackTime, level, pos);
                    if (p != NULL) {
                        /* Post wave to audio callback */
                        PaUtil_WriteRingBuffer(&data.rBufToRT, &p, 1);
                        ++data.noOfActiveWaves;

                        printf("Starting wave at level = %.2f, attack = %.2lf, pos = %.2lf\n", level, attackTime, pos);
                    }
                }
            }
        } else {
            if (data.noOfActiveWaves == 0) {
                printf("All waves finished!\n");
                break;
            }
        }

        Pa_Sleep(100);
    }
    if (err < 0)
        goto error;

    err = Pa_CloseStream(stream);
    if (err != paNoError)
        goto error;

    if (data.rBufToRTData) {
        PaUtil_FreeMemory(data.rBufToRTData);
    }
    if (data.rBufFromRTData) {
        PaUtil_FreeMemory(data.rBufFromRTData);
    }

    Pa_Sleep(1000);

    Pa_Terminate();
    return 0;

error:
    Pa_Terminate();
    fprintf(stderr, "An error occured while using the portaudio stream\n");
    fprintf(stderr, "Error number: %d\n", err);
    fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
    return 0;
}
