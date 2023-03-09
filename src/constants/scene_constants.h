/*\
|*|  Loopidity - multi-track multi-channel audio looper designed for live handsfree use
|*|  Copyright 2017 bill-auger <https://github.com/bill-auger/loopidity/issues>
|*|
|*|  This file is part of the Loopidity program.
|*|
|*|  Loopidity is free software: you can redistribute it and/or modify
|*|  it under the terms of the GNU General Public License version 3
|*|  as published by the Free Software Foundation.
|*|
|*|  Loopidity is distributed in the hope that it will be useful,
|*|  but WITHOUT ANY WARRANTY; without even the implied warranty of
|*|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
|*|  GNU General Public License for more details.
|*|
|*|  You should have received a copy of the GNU General Public License
|*|  along with Loopidity.  If not, see <http://www.gnu.org/licenses/>.
\*/


// magnitudes
#define X_PADDING               10
#define Y_PADDING               18
#define BORDER_PAD              5
#define HISTOGRAMS_H            17 // should be odd
#define HISTOGRAMS_T            (Y_PADDING - 2)                   // drawRecordingLoop() (offset from SCENE_T)
#define HISTOGRAMS_B            (HISTOGRAMS_T + HISTOGRAMS_H - 1) // ""
#define HISTOGRAM_FRAMES_T      (HISTOGRAMS_T - 1)                // ""
#define HISTOGRAM_FRAMES_B      (HISTOGRAMS_B + 1)                // ""
#define HISTOGRAM_IMG_W         103 // (N_PEAKS_COURSE + 2)       // drawHistogram() (offset from HISTOGRAMS_T)
#define HISTOGRAM_IMG_H         19  // (HISTOGRAMS_H + 2)         // ""
#define HISTOGRAM_FRAME_R       (N_PEAKS_COURSE + 1)              // ""
#define HISTOGRAM_FRAME_B       (HISTOGRAMS_H + 1)                // ""
#define HISTOGRAM_PEAK_H        (HISTOGRAMS_H / 2)                // ""
#define HISTOGRAM_0             HISTOGRAM_PEAK_H + 1              // ""
#define LOOP_0                  PEAK_RADIUS
#define LOOP_W                  (X_PADDING + LOOP_DIAMETER)
#define LOOPS_L                 16
#define LOOPS_T                 (HISTOGRAM_FRAMES_B + BORDER_PAD + 1)
#define LOOPS_0                 (LOOPS_T + PEAK_RADIUS + 1)
#define LOOPS_B                 (LOOPS_T + LOOP_DIAMETER + 1)
#define LOOP_FRAMES_T           (HISTOGRAM_FRAMES_T - BORDER_PAD - 1)
#define LOOP_FRAMES_B           (LOOPS_B + BORDER_PAD + 1)
#define SCENE_W                 1000 // SCOPE_IMG is 1000x101
#define SCENE_H                 ((Y_PADDING * 3) + HISTOGRAMS_H + LOOP_DIAMETER)
#define SCENE_L                 (LOOPS_L - BORDER_PAD - 1)
#define SCENE_R                 (SCENE_L + 999) // SCOPE_IMG is 1000x101 , LOOP_DIAMETER is 101 when PEAK_RADIUS is 50
#define SCENE_T                 (HEADER_H + (BORDER_PAD * 2) + (SCENE_H * a_scene->getSceneN()))
#define SCENE_B                 (SCENE_T + SCENE_H)
#define SCENE_FRAME_L           (SCENE_L - BORDER_PAD - 1)
#define SCENE_FRAME_R           (SCENE_R + BORDER_PAD + 1)
#define SCENE_FRAME_T           (SCENE_T + LOOP_FRAMES_T - BORDER_PAD - 1)
#define SCENE_FRAME_B           (SCENE_T + LOOP_FRAMES_B + BORDER_PAD + 1)
#define SCENE_MASK_RECT         { 0 , 0 , SCENE_W , 0 }
#define SCENE_RECT              { SCENE_L , 0 , 0 , 0 }
#define HISTOGRAM_RECT          { 0 , HISTOGRAM_FRAMES_T , 0 , 0 }
#define HISTOGRAM_MASK_RECT     { 0 , 0 , 1 , 0 }
#define HISTOGRAM_GRADIENT_RECT { 0 , 0 , 0 , 0 }
#define ROT_LOOP_IMG_RECT       { 0 , 0 , 0 , 0 }
#define PIE_SLICE_DEGREES       (360.0 / (float)N_PEAKS_FINE)
#define PIE_12_OCLOCK           -90
#define N_SECONDS_PER_HOUR      3600
#define N_MINUTES_PER_HOUR      60
#define N_SECONDS_PER_MINUTE    60

// colors
#define SCOPE_PEAK_MAX_COLOR  0x800000FF
#define SCOPE_PEAK_ZERO_COLOR 0x008000FF
#define STATE_RECORDING_COLOR 0xFF0000FF
#define STATE_PENDING_COLOR   0xFFFF00FF
#define STATE_PLAYING_COLOR   0x00FF00FF
#define STATE_IDLE_COLOR      0x808080FF
#define PEAK_CURRENT_COLOR    0xFFFF00FF
#define HISTOGRAM_PEAK_COLOR  0x008000FF
#define LOOP_PEAK_MAX_COLOR   0x800000FF
#define LOOP_IMG_MASK_COLOR   0xFFFFFFFF

// loop states
#define STATE_LOOP_PLAYING 1
#define STATE_LOOP_PENDING 2
#define STATE_LOOP_MUTED   3
