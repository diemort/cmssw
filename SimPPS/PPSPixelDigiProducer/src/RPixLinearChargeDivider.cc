#include "SimPPS/PPSPixelDigiProducer/interface/RPixLinearChargeDivider.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"
#include "DataFormats/GeometryVector/interface/LocalVector.h"
#include "Geometry/VeryForwardGeometry/interface/CTPPSPixelTopology.h"

RPixLinearChargeDivider::RPixLinearChargeDivider(const edm::ParameterSet &params,  CLHEP::HepRandomEngine& eng,
						 uint32_t det_id) : rndEngine(eng) , _det_id(det_id)
{
  verbosity_ = params.getParameter<int>("RPixVerbosity");
  fluctuate = new SiG4UniversalFluctuation();
  fluctuateCharge_ = params.getParameter<bool>("RPixLandauFluctuations");
  chargedivisions_ = params.getParameter<int>("RPixChargeDivisions");
  deltaCut_ = params.getParameter<double>("RPixDeltaProductionCut");
}

RPixLinearChargeDivider::~RPixLinearChargeDivider(){
  delete fluctuate;
}

std::vector<RPixEnergyDepositUnit> RPixLinearChargeDivider::divide(const PSimHit& hit)
{
  LocalVector direction = hit.exitPoint() - hit.entryPoint();
  if(direction.z()>10 || direction.x()>200 || direction.y()>200)
    {
      the_energy_path_distribution_.clear();
      return the_energy_path_distribution_;
    }

  int NumberOfSegmentation = chargedivisions_ ;
  double eLoss = hit.energyLoss();  // Eloss in GeV
  the_energy_path_distribution_.resize(NumberOfSegmentation);
 
  if(fluctuateCharge_)
    {
      int pid = hit.particleType();
      double momentum = hit.pabs();
      double length = direction.mag();  // Track length in Silicon
      FluctuateEloss(pid, momentum, eLoss, length, NumberOfSegmentation, the_energy_path_distribution_);
      for (int i=0; i<NumberOfSegmentation; i++)
	{
	  the_energy_path_distribution_[i].Position() 
            = hit.entryPoint()+double((i+0.5)/NumberOfSegmentation)*direction;
	}
    }
  else
    {
      for(int i=0; i<NumberOfSegmentation; i++)
	{
	  the_energy_path_distribution_[i].Position() 
            = hit.entryPoint()+double((i+0.5)/NumberOfSegmentation)*direction;
	  the_energy_path_distribution_[i].Energy() = eLoss/(double)NumberOfSegmentation;
	}
    }
  
  if(verbosity_)
    {
      edm::LogInfo("RPixLinearChargeDivider")<<_det_id<<" charge along the track:";
      double sum=0;
      for(unsigned int i=0; i<the_energy_path_distribution_.size(); i++)
	{
	  edm::LogInfo("RPixLinearChargeDivider")<<the_energy_path_distribution_[i].X()<<" "
		   <<the_energy_path_distribution_[i].Y()<<" "
		   <<the_energy_path_distribution_[i].Z()<<" "
		   <<the_energy_path_distribution_[i].Energy();
	  sum += the_energy_path_distribution_[i].Energy();
	}
      edm::LogInfo("RPixLinearChargeDivider")<<"energy dep. sum="<<sum;
    }
  
  return the_energy_path_distribution_;
}
    
void RPixLinearChargeDivider::FluctuateEloss(int pid, double particleMomentum, 
					     double eloss, double length, int NumberOfSegs, 
					     std::vector<RPixEnergyDepositUnit>  &elossVector)
{
  double particleMass = 139.6; // Mass in MeV, Assume pion
  pid = std::abs(pid);
  if (pid != 211) {       // Mass in MeV
    if (pid == 11)        particleMass = 0.511;
    else if (pid == 13)   particleMass = 105.7;
    else if (pid == 321)  particleMass = 493.7;
    else if (pid == 2212) particleMass = 938.3;
  }
  
  double segmentLength = length/NumberOfSegs;

// Generate charge fluctuations.
  double de=0.;
  double sum=0.;
  double segmentEloss = (eloss*1000)/NumberOfSegs; //eloss in MeV
  for (int i=0;i<NumberOfSegs;i++)
    {
       double deltaCutoff = deltaCut_;
      de = fluctuate->SampleFluctuations(particleMomentum*1000, particleMass, 
	 deltaCutoff, segmentLength, segmentEloss, &(rndEngine))/1000; //convert to GeV
      elossVector[i].Energy()=de;
      sum+=de;
    }

  if(sum>0.)
    {  // If fluctuations give eloss>0.
    // Rescale to the same total eloss
      double ratio = eloss/sum;
      for (int ii=0;ii<NumberOfSegs;ii++) elossVector[ii].Energy()=ratio*elossVector[ii].Energy();
    }
  else 
    {  // If fluctuations gives 0 eloss
      double averageEloss = eloss/NumberOfSegs;
      for (int ii=0;ii<NumberOfSegs;ii++) elossVector[ii].Energy()=averageEloss; 
    }
  return;
}

