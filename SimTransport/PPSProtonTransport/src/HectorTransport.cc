#include "SimTransport/PPSProtonTransport/interface/HectorTransport.h"
#include "Utilities/PPS/interface/PPSUtilities.h"
#include <CLHEP/Units/GlobalSystemOfUnits.h>
#include <CLHEP/Units/GlobalPhysicalConstants.h>
#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Vector/LorentzVector.h>
//Hector headers
#include "H_BeamLine.h"
#include "H_BeamParticle.h"


HectorTransport::HectorTransport(){};

HectorTransport::HectorTransport(const edm::ParameterSet & param, bool verbosity)
{
    // Create LHC beam line
    MODE=HECTOR; // defines the MODE for the transport
    m_verbosity=verbosity;
   edm::ParameterSet hector_par = param.getParameter<edm::ParameterSet>("PPSHector");
   
   m_f_ctpps_f          = (float) hector_par.getParameter<double>("PPSf");
   m_b_ctpps_b          = (float) hector_par.getParameter<double>("PPSb");
   fCrossingAngle_56    = hector_par.getParameter<double>("CrossingAngleBeam1");
   fCrossingAngle_45    = hector_par.getParameter<double>("CrossingAngleBeam2");
   fBeamEnergy          = hector_par.getParameter<double>("BeamEnergy"); // beam energy in GeV
   fEtacut              = hector_par.getParameter<double>("EtaCutForHector");
   fMomentumMin         = hector_par.getParameter<double>("MomentumMin");
   fBeamMomentum        = sqrt(fBeamEnergy*fBeamEnergy - PPSTools::ProtonMassSQ);

    // User definitons
    lengthctpps     = hector_par.getParameter<double>("BeamLineLengthPPS" );

    beam1filename   = hector_par.getParameter<string>("Beam1");
    beam2filename   = hector_par.getParameter<string>("Beam2");  
    fVtxMeanX       = param.getParameter<double>("VtxMeanX");
    fVtxMeanY       = param.getParameter<double>("VtxMeanY");
    fVtxMeanZ       = param.getParameter<double>("VtxMeanZ");
    m_sigmaSTX      = hector_par.getParameter<double>("sigmaSTX" );
    m_sigmaSTY      = hector_par.getParameter<double>("sigmaSTY" );
    m_sigmaSX       = hector_par.getParameter<double>("sigmaSX");
    m_sigmaSY       = hector_par.getParameter<double>("sigmaSY");
    m_sig_E         = hector_par.getParameter<double>("sigmaEnergy");
    fBeamXatIP      = hector_par.getUntrackedParameter<double>("BeamXatIP",fVtxMeanX);
    fBeamYatIP      = hector_par.getUntrackedParameter<double>("BeamYatIP",fVtxMeanY);
    bApplyZShift    = hector_par.getParameter<bool>("ApplyZShift");
    //PPS

    SetBeamLine();
    PPSTools::fBeamMomentum=fBeamMomentum;
    PPSTools::fBeamEnergy=fBeamEnergy;
    PPSTools::fCrossingAngleBeam1=fCrossingAngle_56;
    PPSTools::fCrossingAngleBeam2=fCrossingAngle_45;
    fPPSRegionStart_56=m_b_ctpps_b;
    fPPSRegionStart_45=m_f_ctpps_f;
}
HectorTransport::~HectorTransport()
{
        this->clear();

}
void HectorTransport::process( const HepMC::GenEvent * ev , const edm::EventSetup & iSetup, CLHEP::HepRandomEngine * _engine)
{
     engine = _engine;
     iSetup.getData( pdt );
     GenProtonsLoop(ev,iSetup);
     addPartToHepMC(const_cast<HepMC::GenEvent*>(ev));
}
bool HectorTransport::transportProton(const HepMC::GenParticle* gpart)
{
     H_BeamParticle* h_p  = nullptr;

     double px,py,pz,e;
     unsigned int line = (gpart)->barcode();

     double mass = gpart->generatedMass();
     double charge = 1.;

     px = gpart->momentum().px();	  
     py = gpart->momentum().py();	  
     pz = gpart->momentum().pz();	  
     e =  gpart->momentum().e();	  

     e = sqrt(pow(mass,2)+pow(px,2)+pow(py,2)+pow(pz,2));

     int direction;
     direction = (pz>0)?1:-1;

     // Apply Beam and Crossing Angle Corrections
     LorentzVector* p_out = new LorentzVector(px,py,pz,e);
     PPSTools::LorentzBoost(*p_out,"LAB");

     ApplyBeamCorrection(*p_out);

     // from mm to cm        
     double XforPosition = gpart->production_vertex()->position().x()/cm;//cm
     double YforPosition = gpart->production_vertex()->position().y()/cm;//cm
     double ZforPosition = gpart->production_vertex()->position().z()/cm;//cm

     h_p = new H_BeamParticle(mass,charge);
     h_p->set4Momentum(-direction*p_out->px(), p_out->py(), fabs(p_out->pz()), p_out->e());
// shift the beam position to the given beam position at IP (in cm)
     XforPosition=(XforPosition-fVtxMeanX)+fBeamXatIP*mm_to_cm;
     YforPosition=(YforPosition-fVtxMeanY)+fBeamYatIP*mm_to_cm;
     ZforPosition=(ZforPosition); // move the closed orbit only in the X,Y plane
//
// shift the starting position of the track to Z=0 if configured so (all the variables below are still in cm)
     if (bApplyZShift) {
        double fCrossingAngle = (p_out->pz()>0)?fCrossingAngle_45:-fCrossingAngle_56;
        XforPosition = XforPosition+(tan((long double)fCrossingAngle*urad)-((long double)p_out->px())/((long double)p_out->pz()))*ZforPosition;
        YforPosition = YforPosition-((long double)p_out->py())/((long double)p_out->pz())*ZforPosition;
        ZforPosition = 0.;
     }

//
// set position, but do not invert the coordinate for the angles (TX,TY) because it is done by set4Momentum
     h_p->setPosition(-direction*XforPosition*cm_to_um,YforPosition*cm_to_um,h_p->getTX(),h_p->getTY(),-direction*ZforPosition*cm_to_m);
     bool is_stop;
     float x1_ctpps;
     float y1_ctpps;

     H_BeamLine* _beamline = nullptr;
     double _targetZ=0;
     switch (direction) { 
             case -1: _beamline = &*m_beamline56;// negative side propagation
                      _targetZ  = m_b_ctpps_b;
                      break;
             case 1:  _beamline = &*m_beamline45;
                      _targetZ  = m_f_ctpps_f;
                      break;
     }
     // insert protection for NULL beamlines here
     h_p->computePath(&*_beamline);
     is_stop = h_p->stopped(&*_beamline);
     if(m_verbosity) LogDebug("HectorTransportEventProcessing") << "HectorTransport:filterPPS: barcode = "
             << line << " is_stop=  "<< is_stop;
     if (is_stop) return false;
     //
     //propagating
     //
     h_p->propagate( _targetZ );

     p_out = new LorentzVector(PPSTools::HectorParticle2LorentzVector(*h_p,direction));

     p_out->setPx(direction*p_out->px());
     x1_ctpps = direction*h_p->getX()*um_to_mm; 
     y1_ctpps = h_p->getY()*um_to_mm;

     if(m_verbosity) LogDebug("HectorTransportEventProcessing") <<
             "HectorTransport:filterPPS: barcode = " << line << " x=  "<< x1_ctpps <<" y= " << y1_ctpps;
         
     m_beamPart[line]    = p_out;
     m_xAtTrPoint[line]  = x1_ctpps;
     m_yAtTrPoint[line]  = y1_ctpps;
     return true;
}
void HectorTransport::GenProtonsLoop( const HepMC::GenEvent * evt ,const edm::EventSetup & iSetup)
{
/*
   Loop over genVertex looking for transportable protons
*/
    unsigned int line;

    for (HepMC::GenEvent::particle_const_iterator eventParticle =evt->particles_begin(); eventParticle != evt->particles_end(); ++eventParticle ) {
        if (!((*eventParticle)->status() == 1 && (*eventParticle)->pdg_id()==2212 )) continue;
        if (!(fabs((*eventParticle)->momentum().eta())>fEtacut && fabs((*eventParticle)->momentum().pz())>fMomentumMin)) continue;
        line = (*eventParticle)->barcode();

        if ( m_beamPart.find(line) != m_beamPart.end() ) continue;

        HepMC::GenParticle * gpart = (*eventParticle);	

        transportProton(gpart);
    } 
}
bool HectorTransport::SetBeamLine()
{

    m_beamline45=nullptr;
    m_beamline56=nullptr;
    edm::FileInPath b1(beam1filename.c_str());
    edm::FileInPath b2(beam2filename.c_str());
    if(m_verbosity) {
        edm::LogInfo("HectorTransportSetup") << "===================================================================\n"  
            << " * * * * * * * * * * * * * * * * * * * * * * * * * * * *           \n"  
            << " *                                                         *       \n"  
            << " *                   --<--<--  A fast simulator --<--<--     *     \n"  
            << " *                 | --<--<--     of particle   --<--<--     *     \n"  
            << " *  ----HECTOR----<                                          *     \n"  
            << " *                 | -->-->-- transport through-->-->--      *     \n"   
            << " *                   -->-->-- generic beamlines -->-->--     *     \n"  
            << " *                                                           *     \n"   
            << " * JINST 2:P09005 (2007)                                     *     \n"  
            << " *      X Rouby, J de Favereau, K Piotrzkowski (CP3)         *     \n"  
            << " *       http://www.fynu.ucl.ac.be/hector.html               *     \n"  
            << " *                                                           *     \n"  
            << " * Center for Cosmology, Particle Physics and Phenomenology  *     \n"  
            << " *              Universite catholique de Louvain             *     \n"  
            << " *                 Louvain-la-Neuve, Belgium                 *     \n"  
            << " *                                                         *       \n"  
            << " * * * * * * * * * * * * * * * * * * * * * * * * * * * *           \n"   
            << " HectorTransport configuration: \n" 
            << " lengthctpps      = " << lengthctpps << "\n"
            << " m_f_ctpps_f      =  " << m_f_ctpps_f << "\n"
            << " m_b_ctpps_b      =  " << m_b_ctpps_b << "\n"
            << "===================================================================\n";
    }  

    // construct beam line for PPS (forward 1 backward 2):                                                                                           
    if(lengthctpps>0. ) {
        m_beamline45 = std::unique_ptr<H_BeamLine>(new H_BeamLine(-1, lengthctpps + 0.1 )); // (direction, length)
        m_beamline45->fill( b2.fullPath(), 1, "IP5");
        m_beamline56 = std::unique_ptr<H_BeamLine>(new H_BeamLine( 1, lengthctpps + 0.1 )); //
        m_beamline56->fill( b1.fullPath(), 1, "IP5");
    }
    else {
        if ( m_verbosity ) LogDebug("HectorTransportSetup") << "HectorTransport: WARNING: lengthctpps=  " << lengthctpps;
        return false;
    }
    if (m_verbosity) {
          std::cout  << "====================================================================\n"
                     << "                  Forward beam line elements \n";
          m_beamline45->showElements();
          std::cout << "====================================================================\n"
                    << "                 Backward beam line elements \n";
          m_beamline56->showElements();
    }

    return true;
}
