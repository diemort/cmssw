/****************************************************************************
 *
 * This is a part of CTPPS offline software.
 * Authors:
 *   Laurent Forthomme (laurent.forthomme@cern.ch)
 *   Nicola Minafra (nicola.minafra@cern.ch)
 *   Mateusz Szpyrka (mateusz.szpyrka@cern.ch)
 *
 ****************************************************************************/

#ifndef RecoCTPPS_TotemRPLocal_TotemTimingTrackRecognition
#define RecoCTPPS_TotemRPLocal_TotemTimingTrackRecognition

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Common/interface/DetSet.h"
#include "DataFormats/Common/interface/DetSetVector.h"

#include "DataFormats/CTPPSReco/interface/CTPPSTimingRecHit.h"
#include "DataFormats/CTPPSReco/interface/CTPPSTimingLocalTrack.h"
#include "DataFormats/CTPPSReco/interface/CTPPSDiamondLocalTrack.h"
#include "DataFormats/CTPPSReco/interface/TotemTimingLocalTrack.h"

#include <string>
#include <cmath>
#include <set>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "TF1.h"

/**
 * Class intended to perform general CTPPS timing detectors track recognition,
 * as well as construction of specialized classes (for now CTPPSDiamond and TotemTiming local tracks).
**/
class TotemTimingTrackRecognition
{
  public:

    /* General parameters in the set:
     * "threshold": float
     * "thresholdFromMaximum": float
     * "resolution": float
     * "sigma": float
     * "pixelEfficiencyFunction": TF1
     * Parameters specific to TotemTimingLocalTrack:
     * "startFromX": float
     * "stopAtX": float
     * "startFromY": float
     * "stopAtY": float
     */
    TotemTimingTrackRecognition(const edm::ParameterSet& parameters);

    ~TotemTimingTrackRecognition();



    // Class public interface:

    // Resets internal state of the class instance.
    void clear();


    // Adds new hit to the set from which the tracks are reconstructed.
    void addHit(const TotemTimingRecHit& recHit);


    /// Produces a collection of tracks for the current station, given its hits collection
    int produceTracks(edm::DetSet<TotemTimingLocalTrack>& tracks);



  private:

    // Algorithm parameters:
    float threshold;
    float thresholdFromMaximum;
    float resolution;
    float sigma;
    float startFromX;
    float stopAtX;
    float startFromY;
    float stopAtY;
    TF1 pixelEfficiencyFunction;


    typedef std::vector<CTPPSTimingRecHit> HitVector;
    typedef std::unordered_map<int, HitVector> HitVectorMap;


    // Stores CTPPSTimingRecHit vectors grouped by OOTIndex
    HitVectorMap hitVectorMap;


    // Functions used to extract hit's key by which the hits are grouped in hitVectorMap
    //TODO: template specialization
    int getHitKey(const CTPPSTimingRecHit& obj);


    int getHitKey(const CTPPSDiamondRecHit& hit);


    /* Structure representing parameters set for single dimension.
     * Intended to use when producing partial tracks.
     */
    struct DimensionParameters {
      float threshold;
      float thresholdFromMaximum;
      float resolution;
      float sigma;
      float rangeBegin;
      float rangeEnd;
      TF1 hitFunction;
    };


    /* Produces all partial tracks from given set with regard to single dimension.
     *
     * @hits: vector of hits from which the tracks are created
     * @param: describes all parameters used by 1D track recognition algorithm
     * @getHitCenter: functor extracting hit's center in the dimension
     *      that the partial tracks relate to
     * @getHitRangeWidth: analogical to getHitCenter, but extracts hit's width
     *      in specific dimension
     * @result: vector to which produced tracks are appended
     */
    void producePartialTracks(
        const HitVector& hits,
        const DimensionParameters& param,
        float (*getHitCenter)(const CTPPSTimingRecHit&),
        float (*getHitRangeWidth)(const CTPPSTimingRecHit&),
        void (*setTrackCenter)(TotemTimingLocalTrack&, float),
        void (*setTrackSigma)(TotemTimingLocalTrack&, float),
        std::vector<TotemTimingLocalTrack>& result
      );



    /* Creates a partial track that consists of all hits in the hit set.
     *
     * @hits: vector of hits from which the track is created
     * @getHitCenter: functor extracting hit's center in the dimension
     *      that the track relates to
     * @getHitRangeWidth: analogical to getHitCenter, but extracts hit's width
     *      in specific dimension
     * @result: track buffer to which found values are saved
     *
     * @return: true if the track was successfully produced. false otherwise
     */
    bool getHitSpatialRange(
        const HitVector& hits,
        float (*getHitCenter)(const CTPPSTimingRecHit&),
        float (*getHitRangeWidth)(const CTPPSTimingRecHit&),
        float& rangeBegin,
        float& rangeEnd
      );
};

#endif
