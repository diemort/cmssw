import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras
process = cms.Process('CTPPSFastSimulation', eras.ctpps_2016)

# minimal logger settings
process.MessageLogger = cms.Service("MessageLogger",
    statistics = cms.untracked.vstring(),
    destinations = cms.untracked.vstring('cerr'),
    cerr = cms.untracked.PSet(
        threshold = cms.untracked.string('WARNING')
    )
)

# number of events
process.source = cms.Source("EmptySource")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10000)
)

# particle-data table
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")

# particle generator
process.generator = cms.EDProducer("RandomXiThetaGunProducer",
  particleId = cms.uint32(2212),

  energy = cms.double(6500),  # nominal beam energy, GeV
  xi_min = cms.double(0.),
  xi_max = cms.double(0.20),
  theta_x_mean = cms.double(0),
  theta_x_sigma = cms.double(100E-6), # in rad
  theta_y_mean = cms.double(0),
  theta_y_sigma = cms.double(100E-6),

  nParticlesSector45 = cms.uint32(1),
  nParticlesSector56 = cms.uint32(1),
)

# random seeds
process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",
    sourceSeed = cms.PSet(initialSeed =cms.untracked.uint32(98765)),
    generator = cms.PSet(initialSeed = cms.untracked.uint32(98766)),
    beamDivergenceVtxGenerator = cms.PSet(initialSeed =cms.untracked.uint32(3849))
)

# smearing settings
process.load("IOMC.EventVertexGenerators.beamDivergenceVtxGenerator_cfi")
process.beamDivergenceVtxGenerator.src = cms.InputTag("generator", "unsmeared")

process.beamDivergenceVtxGenerator.simulateBeamDivergence = True
process.beamDivergenceVtxGenerator.simulateVertex = True

# values in rad
process.beamDivergenceVtxGenerator.beamDivergenceX = 10E-6
process.beamDivergenceVtxGenerator.beamDivergenceY = 20E-6

# values in cm
process.beamDivergenceVtxGenerator.vertexMeanX = 0.01
process.beamDivergenceVtxGenerator.vertexMeanY = 0.02
process.beamDivergenceVtxGenerator.vertexMeanZ = 1

process.beamDivergenceVtxGenerator.vertexSigmaX = 3E-3
process.beamDivergenceVtxGenerator.vertexSigmaY = 2E-3
process.beamDivergenceVtxGenerator.vertexSigmaZ = 5

# smearing validation
process.load("Validation.CTPPS.smearingValidator_cfi")
process.smearingValidator.tagBeforeSmearing = cms.InputTag("generator", "unsmeared")
process.smearingValidator.tagAfterSmearing = cms.InputTag("beamDivergenceVtxGenerator")

# processing path
process.p = cms.Path(
    process.generator
    * process.beamDivergenceVtxGenerator
    * process.smearingValidator
)
