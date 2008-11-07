import FWCore.ParameterSet.Config as cms

process = cms.Process("TestCabling")

process.load("DQM.SiStripCommon.MessageLogger_cfi")

#process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
#process.GlobalTag.globaltag = 'IDEAL_V9::All'

process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(2) )

process.test = cms.EDAnalyzer("testSiStripFedCabling")

process.p1 = cms.Path( process.test )

