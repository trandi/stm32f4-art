/*
 * * ******************************************************************************
  * @file     saw_osc.h
  * @author  Dan Oprescu, Xavier Halgand
  * @version
  * @date    April 2017
 *
 * from : blepvco - minBLEP-based, hard-sync-capable LADSPA VCOs.
 *
 * Copyright (C) 2004-2005 Sean Bolton.
 *
 * Much of the LADSPA framework used here comes from VCO-plugins
 * 0.3.0, copyright (c) 2003-2004 Fons Adriaensen.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#ifndef _SAW_OSC_H
#define _SAW_OSC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "minblep_tables.h"

#define FILLEN 256
#define DELAYLINE_LEN           14000  // max delay in samples
#define DELAY                   13000  // actual delay (in samples)
#define FREQ2                   8.0f     // default modulation frequency
#define FEEDB                   0.4f
#define _2PI                    6.283185307f


void sawtooth_active (float sRate);
void sawtooth_runproc (uint16_t* audiobuff, uint16_t offset, uint16_t len, float pass, float f1);


#ifdef __cplusplus
}
#endif

#endif /* _SAW_OSC_H */
