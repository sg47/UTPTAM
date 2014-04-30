// -*- c++ -*-
// Copyright 2008 Isis Innovation Limited
// 
// This file declares the MapPoint class
// 
// The map is made up of a bunch of mappoints.
// Each one is just a 3D point in the world;
// it also includes information on where and in which key-frame the point was
// originally made from, so that pixels from that keyframe can be used
// to search for that point.
// Also stores stuff like inlier/outlier counts, and privat information for 
// both Tracker and MapMaker.

#ifndef __MAP_POINT_H
#define __MAP_POINT_H
#include <TooN/TooN.h>
#include <cvd/image_ref.h>
#include <cvd/timer.h>
#include <set>
#include <cvd/rgb.h> //@hack for 3dpoints color
#include <cvd/byte.h> //@hack for 3dpoints color

namespace PTAMM {

using namespace TooN;

class KeyFrame;
class TrackerData;


// Each MapPoint has an associated MapMakerData class
// Where the mapmaker can store extra information

struct MapMakerData
{
  std::set<KeyFrame*> sMeasurementKFs;   // Which keyframes has this map point got measurements in?
  std::set<KeyFrame*> sNeverRetryKFs;    // Which keyframes have measurements failed enough so I should never retry?
  inline int GoodMeasCount()            
  {  return static_cast<int>(sMeasurementKFs.size()); }
};


struct MapPoint
{
  // Constructor inserts sensible defaults and zeros pointers.
  inline MapPoint()
  {
    bBad = false;
    pTData = NULL;
    pMMData = NULL;
    nMEstimatorOutlierCount = 0;
    nMEstimatorInlierCount = 0;
    dCreationTime = CVD::timer.get_time();
  };
  
  // Where in the world is this point? The main bit of information, really.
  Vector<3> v3WorldPos;
  // Is it a dud? In that case it'll be moved to the trash soon.
  bool bBad;
  
  // What pixels should be used to search for this point?
  KeyFrame *pPatchSourceKF; // The KeyFrame the point was originally made in
  int nSourceLevel;         // Pyramid level in source KeyFrame
  CVD::ImageRef irCenter;   // This is in level-coords in the source pyramid level
  
  // What follows next is a bunch of intermediate vectors - they all lead up
  // to being able to calculate v3Pixel{Down,Right}_W, which the PatchFinder
  // needs for patch warping!
  
  Vector<3> v3Center_NC;             // Unit vector in Source-KF coords pointing at the patch center
  Vector<3> v3OneDownFromCenter_NC;  // Unit vector in Source-KF coords pointing towards one pixel down of the patch center
  Vector<3> v3OneRightFromCenter_NC; // Unit vector in Source-KF coords pointing towards one pixel right of the patch center
  Vector<3> v3Normal_NC;             // Unit vector in Source-KF coords indicating patch normal
  
  Vector<3> v3PixelDown_W;           // 3-Vector in World coords corresponding to a one-pixel move down the source image
  Vector<3> v3PixelRight_W;          // 3-Vector in World coords corresponding to a one-pixel move right the source image
  void RefreshPixelVectors();        // Calculates above two vectors
  
  // Info for the Mapmaker (not to be trashed by the tracker:)
  MapMakerData *pMMData;
  
  // Info for the Tracker (not to be trashed by the MapMaker:)
  TrackerData *pTData;
  
  // Info provided by the tracker for the mapmaker:
  int nMEstimatorOutlierCount;
  int nMEstimatorInlierCount;
  
  // Random junk (e.g. for visualisation)
  double dCreationTime; //timer.get_time() time of creation
  
  //@hack by camparijet for 3d point texture...
  CVD::Rgb<CVD::byte> pColor;
};

}

#endif