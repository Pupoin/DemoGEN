// -*- C++ -*-
//
// Package:    Demo/DemoAnalyzer
// Class:      DemoAnalyzer
// 
/**\class DemoAnalyzer DemoAnalyzer.cc Demo/DemoAnalyzer/plugins/DemoAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Qiang Li
//         Created:  Tue, 15 May 2018 02:39:52 GMT
//
//


// system include files
#include <iostream>
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h" 
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TH1D.h"
#include "TTree.h"
//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

class DemoAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit DemoAnalyzer(const edm::ParameterSet&);
      ~DemoAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
      edm::Service<TFileService> fs;
      edm::EDGetTokenT<GenEventInfoProduct> GenToken_;
      edm::EDGetTokenT<LHEEventProduct> LheToken_;
      edm::EDGetTokenT<edm::View<reco::GenParticle>> genSrc_;

      double theWeight;
      double pweight[1000];
      const int ptNBins;
      const double ptMin;
      const double ptMax;
      TH1D *h1_w;
      TTree* outTree_;
      double Htot, Hbb, Hgg, Hww, Hzz;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
DemoAnalyzer::DemoAnalyzer(const edm::ParameterSet& iConfig):
ptNBins(iConfig.getParameter<int>("PtNBins")),
ptMin(iConfig.getParameter<double>("PtMin")),
ptMax(iConfig.getParameter<double>("PtMax"))

{
   //now do what ever initialization is needed
//   usesResource("TFileService");
   GenToken_=consumes<GenEventInfoProduct> (iConfig.getParameter<edm::InputTag>( "gentag") ) ;
   LheToken_=consumes<LHEEventProduct> (iConfig.getParameter<edm::InputTag>( "lhetag"));
   genSrc_ = consumes<edm::View<reco::GenParticle>>(iConfig.getParameter<edm::InputTag>( "genSrc") ) ;
   h1_w = fs->make<TH1D>("h1_w", "w", ptNBins, ptMin, ptMax);
   outTree_ = fs->make<TTree>("ntree","ntree");
   outTree_->Branch("pweight"        , pweight       ,"pweight[1000]/D");
   Htot=0.; Hbb=0.; Hgg=0.; Hww=0.; Hzz=0.;
}


DemoAnalyzer::~DemoAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
DemoAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   edm::Handle<GenEventInfoProduct> genEvtInfo;
   iEvent.getByToken(GenToken_,genEvtInfo);
   theWeight = genEvtInfo->weight();
  // std::cout<<theWeight<<std::endl;
   h1_w->Fill(theWeight);

   edm::Handle<edm::View<reco::GenParticle> > genParticles;//define genParticle
   iEvent.getByToken(genSrc_, genParticles);

   std::cout<<"--------begin-------------"<<std::endl;
   Htot++;
   for(size_t ik=0; ik<genParticles->size();ik++) 
   {//1_loop on gen
     const reco::Candidate* ph = &(*genParticles)[ik];
   
     if(abs(ph->pdgId())== 25) {//2_loop H
        std::cout<<"ik= "<<ik<<std::endl; 
        std::cout<<"H_pId= "<<ph->pdgId()<<"status="<<ph->status()<<std::endl;
        for(int i=0;ph->daughter(i)!=NULL;i++)
        {//3_loop on Higgs daughter
          const reco::Candidate* pw = ph->daughter(i);
          std::cout<<" i= "<<i<<std::endl;
          std::cout<<" W_pId= "<<pw->pdgId()<<"mass="<<pw->mass()<<"status="<<pw->status()<<std::endl;

          if((pw->pdgId())==24) { Hww++; }
          if((pw->pdgId())==5) { Hbb++; }
          if((pw->pdgId())==21) { Hgg=Hgg+0.5; }
          if((pw->pdgId())==23) { Hzz=Hzz+0.5; }

          if(abs(pw->pdgId())==24) {//4_loop on W daughter          
             for(int j=0;pw->daughter(j)!=NULL;j++) {
             const reco::Candidate* pl = pw->daughter(j);
             std::cout<<"   j= "<<j<<std::endl;
             std::cout<<"   pId= "<<pl->pdgId()<<"status="<<pl->status()<<std::endl;
             }   
          } //4_end loop on W daughter
         }//3_end loop on Higgs daugther
        }//2_end H
//        std::cout<<"fromHardProcessFinalState= "<<(*genParticles)[ik].fromHardProcessFinalState()<<std::endl;
//        std::cout<<"fromHardProcessBeforeFSR= "<<(*genParticles)[ik].fromHardProcessBeforeFSR()<<std::endl;
    }//1_end loop gen 


   edm::Handle<LHEEventProduct> wgtsource;
   iEvent.getByToken(LheToken_, wgtsource);
   for(size_t ki=0; ki<wgtsource->weights().size(); ki++) {
      pweight[ki]=wgtsource->weights()[ki].wgt/wgtsource->originalXWGTUP(); 
      //std::cout<<i<<" "<<pweight[ki]<<std::endl;
   }
   outTree_->Fill();
   std::cout<<"Htot:Hgg:Hbb:Hww:Hzz"<<Htot<<" "<<Hgg<<" "<<Hbb<<" "<<Hww<<" "<<Hzz<<std::endl;
}


// ------------ method called once each job just before starting event loop  ------------
void 
DemoAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
DemoAnalyzer::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
DemoAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(DemoAnalyzer);
