import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

runOnMC = True
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
if runOnMC:
   process.GlobalTag.globaltag = '94X_mc2017_realistic_v14' 
elif not(runOnMC):
   process.GlobalTag.globaltag = '94X_mc2017_realistic_v14'

process.load("FWCore.MessageService.MessageLogger_cfi")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
       '/store/mc/RunIIFall17MiniAOD/GJets_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8/MINIAODSIM/94X_mc2017_realistic_v10-v2/10000/0A308ACD-50F7-E711-905C-0CC47A78A4A6.root'
    )
)

process.TFileService = cms.Service('TFileService',
    fileName = cms.string("eff.root")
)


process.demo = cms.EDAnalyzer('DemoAnalyzer',
    gentag = cms.InputTag('generator'),
    lhetag = cms.InputTag('externalLHEProducer'),
    PtNBins = cms.int32(20),
    PtMin = cms.double(-10.),
    PtMax = cms.double(10.),
)


process.p = cms.Path(process.demo)
