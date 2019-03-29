#ifndef SimPPS_RPDigiProducer_RP_GAUSSIAN_TAIL_NOISE_ADDER_H
#define SimPPS_RPDigiProducer_RP_GAUSSIAN_TAIL_NOISE_ADDER_H

#include "SimPPS/RPDigiProducer/interface/RPHitChargeConverter.h"
#include "SimPPS/RPDigiProducer/interface/RPSimTypes.h"

class RPGaussianTailNoiseAdder
{
  public:
    RPGaussianTailNoiseAdder(int numStrips, double theNoiseInElectrons, 
        double theStripThresholdInE, int verbosity);
    simRP::strip_charge_map addNoise(
        const simRP::strip_charge_map &theSignal);
  private:
    simRP::strip_charge_map the_strip_charge_map;
    int numStrips_;
    double theNoiseInElectrons;
    double theStripThresholdInE;
    double strips_above_threshold_prob_;
    int verbosity_;
};

#endif  //SimPPS_RPDigiProducer_RP_GAUSSIAN_TAIL_NOISE_ADDER_H
