/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors:
*   Jan Kašpar (jan.kaspar@gmail.com)
*   Rafał Leszko (rafal.leszko@gmail.com)
*
****************************************************************************/

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"

#include "DataFormats/CTPPSDigi/interface/TotemFEDInfo.h"

#include <string>

//----------------------------------------------------------------------------------------------------

class TotemDAQTriggerDQMSource: public DQMEDAnalyzer
{
  public:
    TotemDAQTriggerDQMSource(const edm::ParameterSet& ps);
    ~TotemDAQTriggerDQMSource() override;

  protected:
    void bookHistograms(DQMStore::IBooker &, edm::Run const &, edm::EventSetup const &) override;
    void analyze(edm::Event const& e, edm::EventSetup const& eSetup) override;

  private:
    unsigned int verbosity;

    edm::EDGetTokenT<std::vector<TotemFEDInfo>> tokenFEDInfo;
    edm::EDGetTokenT<std::vector<TotemFEDInfo>> tokenFEDInfoDiamonds;

    MonitorElement *daq_bx_diff;
    MonitorElement *daq_event_bx_diff;
    MonitorElement *daq_event_bx_diff_vs_fed;
};

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

using namespace std;
using namespace edm;

//----------------------------------------------------------------------------------------------------

TotemDAQTriggerDQMSource::TotemDAQTriggerDQMSource(const edm::ParameterSet& ps) :
  verbosity(ps.getUntrackedParameter<unsigned int>("verbosity", 0))
{
  tokenFEDInfo = consumes<vector<TotemFEDInfo>>(ps.getParameter<edm::InputTag>("tagFEDInfo"));
  tokenFEDInfoDiamonds = consumes<vector<TotemFEDInfo>>(ps.getParameter<edm::InputTag>("tagFEDInfoDiamonds"));
}

//----------------------------------------------------------------------------------------------------

TotemDAQTriggerDQMSource::~TotemDAQTriggerDQMSource()
{
}

//----------------------------------------------------------------------------------------------------

void TotemDAQTriggerDQMSource::bookHistograms(DQMStore::IBooker &ibooker, edm::Run const &, edm::EventSetup const &)
{
  ibooker.cd();

  ibooker.setCurrentFolder("CTPPS/DAQ/");

  daq_bx_diff = ibooker.book1D("bx_diff", ";OptoRx_{i}.BX - OptoRx_{j}.BX", 21, -10.5, +10.5);
  daq_event_bx_diff = ibooker.book1D("daq_event_bx_diff", ";OptoRx_{i}.BX - Event.BX", 21, -10.5, +10.5);
  daq_event_bx_diff_vs_fed = ibooker.book2D("daq_event_bx_diff_vs_fed", ";OptoRx.ID;OptoRx.BX - Event.BX", 10, 575.5, 585.5, 21, -10.5, +10.5);
}

//----------------------------------------------------------------------------------------------------

void TotemDAQTriggerDQMSource::analyze(edm::Event const& event, edm::EventSetup const& eventSetup)
{
  // get input
  Handle<vector<TotemFEDInfo>> fedInfo;
  event.getByToken(tokenFEDInfo, fedInfo);
  Handle<vector<TotemFEDInfo>> fedInfoDiamonds;
  event.getByToken(tokenFEDInfoDiamonds, fedInfoDiamonds);

  // check validity
  bool daqValid = fedInfo.isValid()  & fedInfoDiamonds.isValid();

  vector<TotemFEDInfo> fedInfoTot(*fedInfo);
  fedInfoTot.insert( fedInfoTot.end(), fedInfoDiamonds->begin(), fedInfoDiamonds->end() );

  if (!daqValid)
  {
    if (verbosity)
    {
      LogPrint("TotemDAQTriggerDQMSource") <<
        "WARNING in TotemDAQTriggerDQMSource::analyze > some of the inputs are not valid."
        << "\n    fedInfo.isValid = " << fedInfo.isValid()
        << "\n    fedInfoDiamonds.isValid = " << fedInfoDiamonds.isValid();
    }
  }

  // DAQ plots
  if (daqValid)
  {
    for (auto &it1 : fedInfoTot)
    {
      daq_event_bx_diff->Fill(it1.getBX() - event.bunchCrossing());
      daq_event_bx_diff_vs_fed->Fill(it1.getFEDId(), it1.getBX() - event.bunchCrossing());

      for (auto &it2 : fedInfoTot)
      {
        if (it2.getFEDId() <= it1.getFEDId())
          continue;

        daq_bx_diff->Fill(it2.getBX() - it1.getBX());
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------

DEFINE_FWK_MODULE(TotemDAQTriggerDQMSource);
