// -*- mode: c++; mode: auto-fill; mode: flyspell-prog; -*-

/*
 *   This source code is part of the Eutelescope package of Marlin.
 *   You are free to use this source files for your own development as
 *   long as it stays in a public research context. You are not
 *   allowed to use it for commercial purpose. You must put this
 *   header with author names in all development based on this file.
 *
 */

#ifndef EUTelTripletGBLUtility_h
#define EUTelTripletGBLUtility_h 1

#include <memory>
#include "marlin/Processor.h"

// lcio includes <.h>
#include "lcio.h"
#include <UTIL/CellIDEncoder.h>
#include <UTIL/CellIDDecoder.h>
#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/TrackerHitImpl.h>
#include <IMPL/TrackImpl.h>
#include <IMPL/TrackerDataImpl.h>
#include <IMPL/LCFlagImpl.h>
#include <Exceptions.h>
#include <IMPL/TrackImpl.h>

// AIDA includes <.h>
#if defined(USE_AIDA) || defined(MARLIN_USE_AIDA)
#include <AIDA/IBaseHistogram.h>
#include <marlin/AIDAProcessor.h>
#include <AIDA/IHistogramFactory.h>
#include <AIDA/IAxis.h>
#include <AIDA/IHistogram1D.h>
#include <AIDA/IHistogram2D.h>
#include <AIDA/IProfile1D.h>
#include <AIDA/IProfile2D.h>
#include <AIDA/IAnalysisFactory.h>
#include <AIDA/IFitter.h>
#include <RAIDA/IFitterROOT.h>
#include <AIDA/IFitResult.h>
#include <AIDA/IFitFactory.h>
#include <AIDA/ITree.h>
#include <AIDA/IFunction.h>
#endif

// ROOT includes
#include <TMatrixD.h>
#include "TH1D.h"
#include "TH1F.h"
#include "TF1.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "TCanvas.h"

// system includes <>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <deque>
#include <algorithm>

// marlin includes ".h"
#include "marlin/Processor.h"
#include "marlin/Exceptions.h"
#include "marlin/ProcessorMgr.h"
#include "marlin/Global.h"

// Eigen include
#include <Eigen/Core>
using namespace marlin;

namespace eutelescope {

//class EUTelTripletGBLUtility : public marlin::Processor {
class EUTelTripletGBLUtility {

public:

    //! Default constructor
    EUTelTripletGBLUtility();

    // Calculate Point-To-Point Jacobian Transport Matrix for distance "ds"
    Eigen::Matrix<double, 5,5> JacobianPointToPoint( double ds );

    // Set your parent
    void setParent(marlin::Processor * par) {
        parent = par;
    };

    void bookHistos();

    std::pair<double,double> doIterativeGaussianFit(AIDA::IHistogram1D* in_hist, int rebinFactor=1) const;
    
    // Determine the best cuts on the tracks - will only print them out, they should be updated manually
    void determineBestCuts() const;

    class hit {
    public:

        // Coordinates and their position uncertainty
        double x;
        double ex;
        double y;
        double ey;
        double z;
        double ez;
        // Plane to which the hit belongs
        unsigned int plane;
        // clustersize of the cluster associated to the hit
        int clustersize;
        int clustersizex;
        int clustersizey;
        // local coords
        double locx;
        double locy;
        int id;

        hit() = default;

        hit(double const * const pos, int sensorID) {
            x = pos[0];
            y = pos[1];
            z = pos[2];
            plane = static_cast<unsigned>(sensorID);
        };

        // Overloading ostream operator for printing hits:
        friend std::ostream& operator << (std::ostream& out, const hit& point) // output
        {
            out << "(" << point.plane << ", "
                << point.x << " +/- " << point.ex << " | "
                << point.y << " +/- " << point.ey << " | "
                << point.z << " +/- " << point.ez << ")";
            return out;
        }
    };

    class triplet {
    public:
        triplet();
        triplet(hit hit0, hit hit1, hit hit2);

        // Keep track of linking status to DUT and REF:
        bool linked_dut;
        //bool linked_ref;

        hit getpoint_at(double z) const;

        // Returns x coordinate of the triplet at given z:
        double getx_at(double z) const;

        // Return dx = (x_end - x_start) for the full triplet:
        double getdx() const;

        // Returns dx = (x_measure - x_triplet) in the given plane ipl:
        double getdx(int ipl) const;

        // Returns dx for a given point:
        double getdx(hit point) const;

        // Returns y coordinate of the triplet at given z:
        double gety_at(double z) const;

        // Return dy = (y_end - y_start) for the full triplet:
        double getdy() const;

        // Returns dy = (y_measure - y_triplet) in the given plane ipl:
        double getdy(int ipl) const;

        // Returns dy for a given point:
        double getdy(hit point) const;

        // Return dz = (z_end - z_start) for the full triplet:
        double getdz() const;

        // Returning the hit for the given plane ID
        hit const & gethit(int plane) const;

        //! Returning the center point of the triplet:
        hit base() const;

        //! Returning the slope of the triplet (x,y):
        hit slope() const;

        friend std::ostream& operator << (std::ostream& out, triplet trip)
        {
            out << "Triplet: " << std::endl;
            for( std::map<unsigned int,hit>::iterator itr = trip.hits.begin(); itr != trip.hits.end(); itr++) {
                out << "    " << itr->second << std::endl;
            }
            return out;
        };

    private:
        void filltriplet(hit hit0, hit hit1, hit hit2) {
            hits.insert( std::pair<unsigned int,hit>(hit0.plane,hit0));
            hits.insert( std::pair<unsigned int,hit>(hit1.plane,hit1));
            hits.insert( std::pair<unsigned int,hit>(hit2.plane,hit2));
        };
        //! The hits belonging to the triplet:
        /* Use map since it's already ordered according to plane IDs.
         * We rely on begin() and rbegin() to deliver pointers to the first and last plane of the triplet.
         */
        std::map<unsigned int,hit> hits;
        std::map<unsigned int, hit> DUThits;
    public:
        bool has_DUT(unsigned int ID) {
            return DUThits.find(ID) != DUThits.end();
        }
        auto get_DUT_Hit(unsigned int ID) const -> decltype(DUThits.at(ID)) {
            return DUThits.at(ID);
        }

        auto number_DUTs() const -> decltype(DUThits.size()) {
            return DUThits.size();
        }

        void push_back_DUT(unsigned int ID, hit const & thisHit) {
            DUThits.insert(std::make_pair(ID, thisHit));
        }

        auto DUT_begin() const -> decltype(DUThits.begin()) {
            return DUThits.begin();
        }
        auto DUT_end() const -> decltype(DUThits.end()) {
            return DUThits.end();
        }


    };

    class track {
    public:
        //! Default Track constructor. To be called with two triplets.
        track(triplet up, triplet down);

        //! Return the track kink angle in x
        double kink_x();

        //! Return the track kink angle in y
        double kink_y();

        //! Return the intersection point of the triplets
        hit intersect();

        //! Return the track upstream triplet
        triplet& get_upstream();

        //! Return the track downstream triplet
        triplet& get_downstream();

    private:
        //! Members to store the up- and downstream triplets
        triplet upstream;
        triplet downstream;
    };

    //! Find hit triplets from three telescope planes
    /*! This runs over all hits in the planes of the telescope and
     * tries to match triplets by comparing with the middle planes.
     * @param hits Reference to the hits which are used to construct the triplets
     * @param triplet_sensor_ids Container with exactly three elements which contain the first, middle and last plane id (in this order)
     * @param only_best_triplet Accept only the best matching triplet, not every conbination which passes cuts
     * @param found_triplets Reference to vector inw hich the triplets should be stored
     * Two cut criteria can be set:
     * @param triplet_res_cut Cut on the hit residual in the middle plane with respect to the triplet defined by first and last plane [mm]
     * @param triplet_slope_cut Cut on the triplet track angle [rad]
     *
     * @return a vector of found triplets among the given set of hits.
     */
    template<typename T>
    void FindTriplets(std::vector<EUTelTripletGBLUtility::hit> const & hits, T const & triplet_sensor_ids, double trip_res_cut, double trip_slope_cut, std::vector<EUTelTripletGBLUtility::triplet> & found_trip, bool only_best_triplet = true, bool upstream = true);

    //! Match the upstream and downstream triplets to tracks
    void MatchTriplets(std::vector<EUTelTripletGBLUtility::triplet> const & up, std::vector<EUTelTripletGBLUtility::triplet> const & down, double z_match, double trip_matching_cut, std::vector<EUTelTripletGBLUtility::track> &track);

    bool AttachDUT(EUTelTripletGBLUtility::triplet & triplet, std::vector<EUTelTripletGBLUtility::hit> const & hits, unsigned int dutID,  std::vector<float> dist_cuts);

    //! Check isolation of triplet within vector of triplets
    bool IsTripletIsolated(EUTelTripletGBLUtility::triplet const & it, std::vector<EUTelTripletGBLUtility::triplet> const &trip, double z_match, double isolation = 0.3);

private:

    //! store the parent, needed for having histograms in the same file as the processor that calls the util class
    marlin::Processor * parent;



protected:
    std::string _inputCollectionTelescope;


    //cut plots
    AIDA::IHistogram1D * upstreamTripletSlopeX;
    AIDA::IHistogram1D * upstreamTripletSlopeY;
    AIDA::IHistogram1D * downstreamTripletSlopeX;
    AIDA::IHistogram1D * downstreamTripletSlopeY;
    AIDA::IHistogram1D * upstreamTripletResidualX;
    AIDA::IHistogram1D * upstreamTripletResidualY;
    AIDA::IHistogram1D * downstreamTripletResidualX;
    AIDA::IHistogram1D * downstreamTripletResidualY;
    AIDA::IHistogram1D * tripletMatchingResidualX;
    AIDA::IHistogram1D * tripletMatchingResidualY;
    AIDA::IHistogram1D * DUTMatchingResidualX;
    AIDA::IHistogram1D * DUTMatchingResidualY;
    AIDA::IHistogram1D * DUTHitNumber;
    AIDA::IHistogram1D * TPMatchingResidualX;
    AIDA::IHistogram1D * TPMatchingResidualY;
};

}

#include "EUTelTripletGBLUtility.tcc"
#endif
