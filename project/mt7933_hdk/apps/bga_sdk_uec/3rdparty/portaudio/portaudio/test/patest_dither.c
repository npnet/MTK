/** @file patest_dither.c
    @ingroup test_src
    @brief Attempt to hear difference between dithered and non-dithered signal.

    This only has an effect if the native format is 16 bit.

    @author Phil Burk  http://www.softsynth.com
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
#include <math.h>

#include "portaudio.h"

#define NUM_SECONDS   (5)
#define SAMPLE_RATE   (44100)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif /* #ifndef M_PI */
#define TABLE_SIZE   (200)

typedef struct paTestData {
    float sine[TABLE_SIZE];
    float amplitude;
    int   left_phase;
    int   right_phase;
}
paTestData;

/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int sineCallback(const void *inputBuffer, void *outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo *timeInfo,
                        PaStreamCallbackFlags statusFlags, void *userData)
{
    paTestData *data = (paTestData *)userData;
    float *out = (float *)outputBuffer;
    float amplitude = data->amplitude;
    unsigned int i;
    (void) inputBuffer;

    for (i = 0; i < framesPerBuffer; i++) {
        *out++ = amplitude * data->sine[data->left_phase];  /* left */
        *out++ = amplitude * data->sine[data->right_phase];  /* right */
        data->left_phase += 1;
        if (data->left_phase >= TABLE_SIZE)
            data->left_phase -= TABLE_SIZE;
        data->right_phase += 3; /* higher pitch so we can distinguish left and right. */
        if (data->right_phase >= TABLE_SIZE)
            data->right_phase -= TABLE_SIZE;
    }
    return 0;
}

/*****************************************************************************/
/*
    V18 version did not call Pa_Terminate() if Pa_Initialize() failed.
    This V19 version ALWAYS calls Pa_Terminate(). PS.
*/
PaError PlaySine(paTestData *data, PaStreamFlags flags, float amplitude);
PaError PlaySine(paTestData *data, PaStreamFlags flags, float amplitude)
{
    PaStream           *stream;
    PaStreamParameters  outputParameters;
    PaError             err;

    data->left_phase = data->right_phase = 0;
    data->amplitude  = amplitude;

    err = Pa_Initialize();
    if (err != paNoError)
        goto done;

    outputParameters.device = Pa_GetDefaultOutputDevice();  /* default output device */
    if (outputParameters.device == paNoDevice) {
        fprintf(stderr, "Error: No default output device.\n");
        goto done;
    }
    outputParameters.channelCount = 2;                      /* stereo output */
    outputParameters.hostApiSpecificStreamInfo = NULL;
    outputParameters.sampleFormat = paFloat32;      /* 32 bit floating point output. */
    /* When you change this, also    */
    /* adapt the callback routine!   */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)
                                        ->defaultLowOutputLatency;   /* Low latency. */
    err = Pa_OpenStream(&stream,
                        NULL,                              /* No input. */
                        &outputParameters,
                        SAMPLE_RATE,
                        1024,                              /* frames per buffer */
                        flags,
                        sineCallback,
                        (void *)data);
    if (err != paNoError)
        goto done;

    err = Pa_StartStream(stream);
    if (err != paNoError)
        goto done;

    Pa_Sleep(NUM_SECONDS * 1000);
    printf("CPULoad = %8.6f\n", Pa_GetStreamCpuLoad(stream));

    err = Pa_CloseStream(stream);
done:
    Pa_Sleep(250);    /* Just a small silence. */
    Pa_Terminate();
    return err;
}


/*******************************************************************/
int main(void);
int main(void)
{
    PaError     err;
    paTestData  DATA;
    int         i;
    float       amplitude = 4.0 / (1 << 15);

    printf("PortAudio Test: output EXTREMELY QUIET sine wave with and without dithering.\n");
    /* initialise sinusoidal wavetable */
    for (i = 0; i < TABLE_SIZE; i++) {
        DATA.sine[i] = (float) sin(((double)i / (double)TABLE_SIZE) * M_PI * 2.);
    }
    printf("\nNo treatment..\n");
    fflush(stdout);
    err = PlaySine(&DATA, paClipOff | paDitherOff, amplitude);
    if (err < 0)
        goto done;

    printf("\nClip..\n");
    fflush(stdout);
    err = PlaySine(&DATA, paDitherOff, amplitude);
    if (err < 0)
        goto done;

    printf("\nClip and Dither..\n");
    fflush(stdout);
    err = PlaySine(&DATA, paNoFlag, amplitude);
done:
    if (err) {
        fprintf(stderr, "An error occured while using the portaudio stream\n");
        fprintf(stderr, "Error number: %d\n", err);
        fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
        err = 1; /* Though PlaySine() already called Pa_Terminate(), */
    }        /* we may still call Pa_GetErrorText().             */
    else
        printf("\n(Don't forget to turn the VOLUME DOWN after listening so carefully.)\n");
    return err;  /* 0 or 1. */
}
