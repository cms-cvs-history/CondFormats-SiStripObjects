#include "CondFormats/SiStripObjects/interface/FineDelayAnalysis.h"
#include "DataFormats/SiStripCommon/interface/SiStripHistoTitle.h"
#include "DataFormats/SiStripCommon/interface/SiStripEnumsAndStrings.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/SiStripCommon/interface/SiStripPulseShape.h"
#include "TProfile.h"
#include "TF1.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>

using namespace sistrip;

// ----------------------------------------------------------------------------
// 
FineDelayAnalysis::FineDelayAnalysis( const uint32_t& key ) 
  : CommissioningAnalysis(key,"FineDelayAnalysis"),
    max_(sistrip::invalid_),
    error_(sistrip::invalid_),
    histo_(0,"")
{
  deconv_fitter_ = new TF1("deconv_fitter",fdeconv_convoluted,-50,50,5);
  deconv_fitter_->FixParameter(0,0);
  deconv_fitter_->SetParLimits(1,-10,10);
  deconv_fitter_->SetParLimits(2,0,200);
  deconv_fitter_->SetParLimits(3,5,100);
  deconv_fitter_->FixParameter(3,50);
  deconv_fitter_->SetParLimits(4,0,50);
  deconv_fitter_->SetParameters(0.,-2.82,0.96,50,20);
}
// ----------------------------------------------------------------------------
// 
FineDelayAnalysis::FineDelayAnalysis() 
  : CommissioningAnalysis("FineDelayAnalysis"),
    max_(sistrip::invalid_),
    error_(sistrip::invalid_),
    histo_(0,"")
{
  deconv_fitter_ = new TF1("deconv_fitter",fdeconv_convoluted,-50,50,5);
  deconv_fitter_->FixParameter(0,0);
  deconv_fitter_->SetParLimits(1,-10,10);
  deconv_fitter_->SetParLimits(2,0,200);
  deconv_fitter_->SetParLimits(3,5,100);
  deconv_fitter_->FixParameter(3,50);
  deconv_fitter_->SetParLimits(4,0,50);
  deconv_fitter_->SetParameters(0.,-2.82,0.96,50,20);
}

// ----------------------------------------------------------------------------
// 
void FineDelayAnalysis::print( std::stringstream& ss, uint32_t not_used ) { 
  header( ss );
  ss << " Delay corresponding to the maximum of the pulse : " << max_ << std::endl
     << " Error on the position (from the fit)            : " << error_ << std::endl;
}

// ----------------------------------------------------------------------------
// 
void FineDelayAnalysis::reset() {
  error_ = sistrip::invalid_;
  max_ = sistrip::invalid_;
  histo_ = Histo(0,"");
  deconv_fitter_->FixParameter(0,0);
  deconv_fitter_->SetParLimits(1,-10,10);
  deconv_fitter_->SetParLimits(2,0,200);
  deconv_fitter_->SetParLimits(3,5,100);
  deconv_fitter_->FixParameter(3,50);
  deconv_fitter_->SetParLimits(4,0,50);
  deconv_fitter_->SetParameters(0.,-2.82,0.96,50,20);
}

// ----------------------------------------------------------------------------
// 
void FineDelayAnalysis::extract( const std::vector<TH1*>& histos) {
  
  // Check
  if ( histos.size() != 1 ) {
    edm::LogWarning(mlCommissioning_) << " Unexpected number of histograms: " << histos.size();
  }
  
  // Extract
  std::vector<TH1*>::const_iterator ihis = histos.begin();
  for ( ; ihis != histos.end(); ihis++ ) {
    
    // Check pointer
    if ( !(*ihis) ) {
      edm::LogWarning(mlCommissioning_) << " NULL pointer to histogram!";
      continue;
    }
    
    // Check name
    SiStripHistoTitle title( (*ihis)->GetName() );
    if ( title.runType() != sistrip::FINE_DELAY ) {
      edm::LogWarning(mlCommissioning_) 
	<< " Unexpected commissioning task: "
	<< SiStripEnumsAndStrings::runType(title.runType());
      continue;
    }
    
    // Extract timing histo
    histo_.first = *ihis;
    histo_.second = (*ihis)->GetName();
    
  }
  
}

// ----------------------------------------------------------------------------
// 
void FineDelayAnalysis::analyse() { 
  if ( !histo_.first ) {
    edm::LogWarning(mlCommissioning_) << " NULL pointer to histogram!" ;
    return;
  }
  
  // we will run on a tmp copy of the histogram, for security
  TProfile* prof = (TProfile*)((TProfile*)(histo_.first))->Clone();
  // prune the profile
  pruneProfile(prof);
  // correct for the binning
  correctBinning(prof);
  // correct for clustering effects
  correctProfile(prof);
  // fit
  histo_.first->Fit(deconv_fitter_,"QL");
  
  // Set monitorables
  max_ = deconv_fitter_->GetMaximumX();
  error_ = deconv_fitter_->GetParError(1);

}

// ----------------------------------------------------------------------------
//
void FineDelayAnalysis::pruneProfile(TProfile* profile) const
{
  // loop over bins to find the max stat
  uint32_t nbins=profile->GetNbinsX();
  uint32_t max = 0;
  for(uint32_t bin=1;bin<=nbins;++bin) max = max < profile->GetBinEntries(bin) ? uint32_t(profile->GetBinEntries(bin)) : max;
  // loop over bins to clean
  uint32_t min = max/10;
  for(uint32_t bin=1;bin<=nbins;++bin)
    if(profile->GetBinEntries(bin)<min) {
      profile->SetBinContent(bin,0.);
      profile->SetBinError(bin,0.);
    }
}

// ----------------------------------------------------------------------------
//
void FineDelayAnalysis::correctBinning(TProfile* prof) const
{
  prof->GetXaxis()->SetLimits(prof->GetXaxis()->GetXmin()-prof->GetBinWidth(1)/2.,
                              prof->GetXaxis()->GetXmax()-prof->GetBinWidth(1)/2.);
}

// ----------------------------------------------------------------------------
//
float FineDelayAnalysis::limit(float SoNcut) const
{
  return 3.814567e+00+8.336601e+00*SoNcut-1.511334e-01*pow(SoNcut,2);
}

// ----------------------------------------------------------------------------
//
float FineDelayAnalysis::correctMeasurement(float mean, float SoNcut) const
{
  if(mean>limit(SoNcut))
    return -8.124872e+00+9.860108e-01*mean-3.618158e-03*pow(mean,2)+2.037263e-05*pow(mean,3);
  else return 0.;
}

// ----------------------------------------------------------------------------
//
void FineDelayAnalysis::correctProfile(TProfile* profile, float SoNcut) const
{
  uint32_t nbins=profile->GetNbinsX();
  float min = limit(SoNcut);
  for(uint32_t bin=1;bin<=nbins;++bin)
    if(profile->GetBinContent(bin)<min) {
      profile->SetBinContent(bin,0.);
      profile->SetBinError(bin,0.);
      profile->SetBinEntries(bin,0);
    }
    else {
      profile->SetBinContent(bin,
                             profile->GetBinEntries(bin)*
                             correctMeasurement(profile->GetBinContent(bin),SoNcut));
    }
}
