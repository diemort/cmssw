import FWCore.ParameterSet.Config as cms

from SimCTPPS.OpticsParameterisation.lhcBeamConditions_cff import lhcBeamConditions_2016PreTS2

from SimCTPPS.OpticsParameterisation.ctppsDetectorPackages_cff import detectorPackages_2018_fake

ctppsFastProtonSimulation = cms.EDProducer('CTPPSFastProtonSimulation',
    verbosity = cms.untracked.uint32(0),

    hepMCTag = cms.InputTag('generator', 'unsmeared'),

    beamConditions = lhcBeamConditions_2016PreTS2,

    opticsFileBeam1 = cms.FileInPath('CondFormats/CTPPSOpticsObjects/data/2016_preTS2/version4-vale1/beam1/parametrization_6500GeV_0p4_185_reco.root'),
    opticsFileBeam2 = cms.FileInPath('CondFormats/CTPPSOpticsObjects/data/2016_preTS2/version4-vale1/beam2/parametrization_6500GeV_0p4_185_reco.root'),

    detectorPackages = detectorPackages_2018_fake,

    produceScoringPlaneHits = cms.bool(True),
    produceRecHits = cms.bool(True),

    produceHitsRelativeToBeam = cms.bool(False),

    checkApertures = cms.bool(True),

    roundToPitch = cms.bool(False),

    pitchStrips = cms.double(66e-3), # mm
    insensitiveMarginStrips = cms.double(34e-3), # mm

    pitchPixelsHor = cms.double(150e-3), # mm
    pitchPixelsVer = cms.double(100e-3), # mm
)
