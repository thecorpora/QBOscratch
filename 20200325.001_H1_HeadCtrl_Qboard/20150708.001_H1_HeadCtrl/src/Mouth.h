/*
 * Mouth.h
 *
 *  Created on: 16/1/2016
 *      Author: Angel
 */

#ifndef SOURCES_MOUTH_H_
#define SOURCES_MOUTH_H_

#include "asf.h"
#include <string.h>


#define REFRESH_TIME		10	// Mouth colummn refresh time
#define NROWS		4
#define NCOLUMS 	4
#define NFRAMES		12

typedef struct tag_Mouth
{
	union
	{
		struct
		{
			uint32_t Row_idx:4;			// Present row bit ( 0x01 << n)
			uint32_t Column_idx:4;		// Present Column data
			uint32_t nose:3;			// Present nose values
			uint32_t frame_Sync:1;		// Frame sync bit set when led matrix restart refresh
			uint32_t frame_idx:5;		// Present frame index
			uint32_t nFrames:5;			// Number of animation frames
			uint32_t Anim_En:1;			// Frame enable bit
			uint32_t Anim_Rpt:1;		// Animation repeat
			uint32_t:7;
		}Ctrl;
		uint32_t dwCtrl;
	};

	uint16_t FrameCnt;				// Frame period counter
	uint16_t FramePeriode;			// Frame period in ms
	uint8_t Mouth_Data[NFRAMES][NCOLUMS];
}stMouth;

void fInit_Mouth(void);
void fsetMouth(uint8_t, uint8_t);
void fAnimation_Runtime(void);
uint32_t fMouth_Main(void);
void fMouth_SetNoseColor(uint8_t);
void fMouth_SetData(uint32_t);
void fConfig_Animation(uint16_t, uint16_t, uint16_t );
void fStart_Animation(uint16_t, uint16_t, uint16_t);
void fTrig_Animation(void);


#endif /* SOURCES_MOUTH_H_ */
