import FWCore.ParameterSet.Config as cms

process = cms.Process('RECODQM')

# minimum of logs
process.MessageLogger = cms.Service("MessageLogger",
  statistics = cms.untracked.vstring(),
  destinations = cms.untracked.vstring('cerr'),
  cerr = cms.untracked.PSet(
      threshold = cms.untracked.string('WARNING')
  )
)

# load DQM framework
process.load("DQM.Integration.config.environment_cfi")
process.dqmEnv.subSystemFolder = "CTPPS"
process.dqmEnv.eventInfoFolder = "EventInfo"
process.dqmSaver.path = ""
process.dqmSaver.tag = "CTPPS"

# raw data source
process.source = cms.Source("NewEventStreamFileReader",
  fileNames = cms.untracked.vstring(
    'file:/eos/cms/store/group/dpg_ctpps/comm_ctpps/TotemTiming/Minidaq/860/run312860_ls0002_streamA_StorageManager.dat'
  )
)

process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32(-1)
)

# raw-to-digi conversion
process.load("EventFilter.CTPPSRawToDigi.ctppsRawToDigi_cff")

# local RP reconstruction chain with standard settings
process.load("RecoCTPPS.Configuration.recoCTPPS_cff")

# CTPPS DQM modules
process.load("DQM.CTPPS.ctppsDQM_cff")

process.path = cms.Path(
  process.ctppsRawToDigi *
  process.recoCTPPS *
  process.ctppsDQM
)

process.end_path = cms.EndPath(
  process.dqmEnv +
  process.dqmSaver
)

process.schedule = cms.Schedule(
  process.path,
  process.end_path
)
