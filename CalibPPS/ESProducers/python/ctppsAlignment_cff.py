import FWCore.ParameterSet.Config as cms

from CalibPPS.ESProducers.ctppsRPAlignmentCorrectionsDataESSourceXML_cfi import *
ctppsRPAlignmentCorrectionsDataESSourceXML.RealFiles = cms.vstring(
  # internal pixel alignment
  #"CalibPPS/ESProducers/data/alignment/RPixGeometryCorrections_old.xml",
  "CalibPPS/ESProducers/data/alignment/RPixGeometryCorrections-2017-2018.xml",

  # global alignment of tracking RPs (strips and pixels)
  #"CalibPPS/ESProducers/data/alignment/alignment_export_2019_03_18.5.xml",
  #"CalibPPS/ESProducers/data/alignment/alignment_export_2019_04_18.1.xml",
  #"CalibPPS/ESProducers/data/alignment/alignment_export_2019_05_09.1.xml",
  #"CalibPPS/ESProducers/data/alignment/alignment_export_2019_07_08.xml",
  #"CalibPPS/ESProducers/data/alignment/alignment_export_2019_07_11.xml",
  #"CalibPPS/ESProducers/data/alignment/alignment_export_2019_09_19.xml",
  #"CalibPPS/ESProducers/data/alignment/alignment_export_2019_09_25.xml",
  #"CalibPPS/ESProducers/data/alignment/alignment_export_2019_09_29.xml",
  #"CalibPPS/ESProducers/data/alignment/alignment_export_2019_10_29.xml",
  #"CalibPPS/ESProducers/data/alignment/alignment_export_2019_12_04.xml", # alignment used for UL re-reco of Run2
  "CalibPPS/ESProducers/data/alignment/alignment_export_2020_03_23.xml", # test version of 2018 vertical alignment

  # alignment of timing RPs (diamonds and UFSDs)
  #"CalibPPS/ESProducers/data/alignment/timing_RP_2017_preTS2_v1.xml",
  "CalibPPS/ESProducers/data/alignment/timing_RP_2017_preTS2_v2.xml",

  #"CalibPPS/ESProducers/data/alignment/timing_RP_2017_postTS2_v1.xml",
  #"CalibPPS/ESProducers/data/alignment/timing_RP_2017_postTS2_v2.xml",
  #"CalibPPS/ESProducers/data/alignment/timing_RP_2017_postTS2_v3.xml",
  "CalibPPS/ESProducers/data/alignment/timing_RP_2017_postTS2_v4.xml",

  #"CalibPPS/ESProducers/data/alignment/timing_RP_2018_v1.xml"
  #"CalibPPS/ESProducers/data/alignment/timing_RP_2018_v2.xml"
  "CalibPPS/ESProducers/data/alignment/timing_RP_2018_v4.xml"
)

esPreferLocalAlignment = cms.ESPrefer("CTPPSRPAlignmentCorrectionsDataESSourceXML", "ctppsRPAlignmentCorrectionsDataESSourceXML")
