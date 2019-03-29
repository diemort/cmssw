#ifndef SimPPS_RPDigiProducer_RP_DET_DIGITIZER_H
#define SimPPS_RPDigiProducer_RP_DET_DIGITIZER_H

#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include <vector>
#include <string>


#include "SimTracker/Common/interface/SiG4UniversalFluctuation.h"
#include "SimGeneral/NoiseGenerators/interface/GaussianTailNoiseGenerator.h"

#include "SimPPS/RPDigiProducer/interface/RPSimTypes.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


#include "SimPPS/RPDigiProducer/interface/RPHitChargeConverter.h"
#include "SimPPS/RPDigiProducer/interface/RPVFATSimulator.h"
#include "DataFormats/CTPPSDigi/interface/TotemRPDigi.h"
#include "SimPPS/RPDigiProducer/interface/RPDisplacementGenerator.h"
#include "SimPPS/RPDigiProducer/interface/RPGaussianTailNoiseAdder.h"
#include "SimPPS/RPDigiProducer/interface/RPPileUpSignals.h"


namespace CLHEP{
        class HepRandomEngine;
}


class RPDetDigitizer
{
  public:
    RPDetDigitizer(const edm::ParameterSet &params, CLHEP::HepRandomEngine& eng, RPDetId det_id, const edm::EventSetup& iSetup);
    void run(const std::vector<PSimHit> &input, const std::vector<int> &input_links, 
        std::vector<TotemRPDigi> &output_digi, 
        simRP::DigiPrimaryMapType &output_digi_links); 
      
  private:
    std::unique_ptr<RPGaussianTailNoiseAdder> theRPGaussianTailNoiseAdder;
    std::unique_ptr<RPPileUpSignals> theRPPileUpSignals;
    std::unique_ptr<RPHitChargeConverter> theRPHitChargeConverter;
    std::unique_ptr<RPVFATSimulator> theRPVFATSimulator;
    std::unique_ptr<RPDisplacementGenerator> theRPDisplacementGenerator;

  private:
    int numStrips;
    double theNoiseInElectrons;   // Noise (RMS) in units of electrons.
    double theStripThresholdInE;  // Strip noise treshold in electorns.
    bool noNoise;                 //if the nos is included
    RPDetId det_id_;
    bool misalignment_simulation_on_;
    int verbosity_;
    bool links_persistence_;
};

#endif  //SimCTPPS_RPDigiProducer_RP_DET_DIGITIZER_H
