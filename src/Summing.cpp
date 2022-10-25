//*****************************************************************
/*
  JackTrip Audio Channel Summing Plugin

  Copyright (c) 2022 Evan Edwards, BRIDGR.io
*/
//*****************************************************************

/**
 * \file Summing.cpp
 * \author Evan Edwards, based on Reverb.h
 */

#include "Summing.h"

#include "jacktrip_types.h"

//*******************************************************************************
void Summing::compute(int nframes, float** inputs, float** outputs)
{
    std::cerr << "compute called for summing plugin"
    if (not inited) {
        std::cerr << "*** Summing " << this << ": init never called! Doing it now.\n";
        if (fSamplingFreq <= 0) {
            fSamplingFreq = 48000;
            std::cout << "Summing " << this
                      << ": *** HAD TO GUESS the sampling rate (chose 48000 Hz) ***\n";
        }
        init(fSamplingFreq);
    }

    //Do the processing
    /*
    if (mReverbLevel <= 1.0) {
        if (mNumInChannels == 1) {
            freeverbMonoP->compute(nframes, inputs, outputs);
        } else {
            assert(mNumInChannels == 2);
            freeverbStereoP->compute(nframes, inputs, outputs);
        }
    } else {
        if (mNumInChannels == 1) {
            zitarevMonoP->compute(nframes, inputs, outputs);
        } else {
            assert(mNumInChannels == 2);
            zitarevStereoP->compute(nframes, inputs, outputs);
        }
    }
    */
}
