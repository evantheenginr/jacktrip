//*****************************************************************
/*
  JackTrip Audio Channel Summing Plugin

  Copyright (c) 2022 Evan Edwards, BRIDGR.io
*/
//*****************************************************************

/**
 * \file Summing.h
 * \author Evan Edwards, based on Reverb.h
 */

#ifndef __SUMMING_H__
#define __SUMMING_H__

#include <iostream>


#include "ProcessPlugin.h"
#include "freeverbdsp.h"  // stereo in and out
#include "freeverbmonodsp.h"  // mono in and out (there is no mono to stereo case in jacktrip as yet)
#include "zitarevdsp.h"      // stereo in and out
#include "zitarevmonodsp.h"  // mono in and out

class Summing : public ProcessPlugin
{
   public:
    Summing(int numInChans, int numOutChans, 
           bool verboseFlag = false) 
        : mNumInChannels(numInChans)
        , mNumOutChannels(numOutChans)
    {
        setVerbose(verboseFlag);
        if (mNumInChannels < 1) {
            std::cerr << "*** Summing.h: must have at least one input audio channels\n";
            mNumInChannels = 1;
        }
        if (mNumInChannels > 1) {
            std::cerr << "*** Summing.h: limiting number of audio output channels to 1\n";
            mNumInChannels = 1;
        }
//#if 0
    std::cout << "Summing: constructed for "
              << mNumInChannels << " input channels and "
              << mNumOutChannels << " output channels\n";
//#endif

    }

    /// \brief The class destructor
    virtual ~Summing(){}

    void init(int samplingRate) override
    {
        ProcessPlugin::init(samplingRate);
        std::cout << "Summing: init(" << samplingRate << ")\n";
        if (samplingRate != fSamplingFreq) {
            std::cerr << "Sampling rate not set by superclass!\n";
            std::exit(1);
        }
        inited = true;
    }
    int getNumInputs() override { return (mNumInChannels); }
    int getNumOutputs() override { return (mNumOutChannels); }
    void compute(int nframes, float** inputs, float** outputs) override;
    const char* getName() const override { return "Reverb"; }

   private:
    float fs;
    int mNumInChannels;
    int mNumOutChannels;
};

#endif
