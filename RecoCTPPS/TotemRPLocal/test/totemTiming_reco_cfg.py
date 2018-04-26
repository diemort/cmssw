import FWCore.ParameterSet.Config as cms

process = cms.Process('CTPPS')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_hlt_relval', '')

# raw data source
#process.source = cms.Source("NewEventStreamFileReader",
#    fileNames = cms.untracked.vstring(
#        '/store/t0streamer/Data/Physics/000/286/591/run286591_ls0521_streamPhysics_StorageManager.dat',
#        '/store/t0streamer/Minidaq/A/000/303/982/run303982_ls0001_streamA_StorageManager.dat',
#    )
#)
process.source = cms.Source('PoolSource',
    fileNames = cms.untracked.vstring(
        '/store/data/Commissioning2018/ZeroBias/RAW/v1/000/314/816/00000/28999A0C-5445-E811-8C7D-FA163EBC8376.root',
    ),
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000)
)

# raw-to-digi conversion
process.load("EventFilter.CTPPSRawToDigi.ctppsRawToDigi_cff")

# rechits production
process.load('RecoCTPPS.TotemRPLocal.totemTimingRecHits_cfi')

# rechits production
process.load('Geometry.VeryForwardGeometry.geometryRPFromDD_2018_cfi')
process.load('RecoCTPPS.TotemRPLocal.totemTimingRecHits_cfi')


process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string("file:AOD.root"),
    outputCommands = cms.untracked.vstring(
        'drop *',
        'keep *_totem*_*_*',
    ),
)

# execution configuration
process.p = cms.Path(
    process.ctppsRawToDigi*
    process.totemTimingRecHits
)

process.outpath = cms.EndPath(process.output)

