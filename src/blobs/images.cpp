
#include "images.h"
#include "histogram_gradient.h"
#include "loop_gradient.h"
#include "scope_gradient.h"


const GimpImage GimpImage::HistogramGradient =
{
  HistogramGradientImg.width           , HistogramGradientImg.height     ,
  HistogramGradientImg.bytes_per_pixel , HistogramGradientImg.pixel_data
} ;

const GimpImage GimpImage::LoopGradient      =
{
  LoopGradientImg     .width           , LoopGradientImg     .height     ,
  LoopGradientImg     .bytes_per_pixel , LoopGradientImg     .pixel_data
} ;

const GimpImage GimpImage::ScopeGradient     =
{
  ScopeGradientImg    .width           , ScopeGradientImg    .height     ,
  ScopeGradientImg    .bytes_per_pixel , ScopeGradientImg    .pixel_data
} ;
