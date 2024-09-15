/*
 * Mouth.c
 *
 *  Created on: 16/1/2016
 *      Author: Angel
 */

#include "asf.h"
#include "Mouth.h"

volatile stMouth Mouth __attribute__ ((aligned (4)));

void fsetMouth(uint8_t Rows, uint8_t Collumns)
{
	uint32_t port_data = (Rows & 0x0f) << 17;
	port_data |= (~Collumns & 0x1f) << 12;
	PORT->Group[0].OUTSET.reg = port_data & 0x001FF000;		// Set active leds
	PORT->Group[0].OUTCLR.reg = ~port_data & 0x001FF000;	// clear idle leds
}

//	Runtime machine must be called periodically at refresh frequency

void fAnimation_Runtime(void)
{
	if(Mouth.FrameCnt++ > Mouth.FramePeriode && Mouth.Ctrl.frame_Sync == 1)
	{
		Mouth.FrameCnt = 0;

		if(Mouth.Ctrl.Anim_En)
		{
			if(++Mouth.Ctrl.frame_idx > Mouth.Ctrl.nFrames)
			{
				Mouth.Ctrl.frame_idx =0;
				Mouth.Ctrl.Anim_En = Mouth.Ctrl.Anim_Rpt;
			}
		}
	}
}

uint32_t fMouth_Main(void)
{

	 Mouth.Ctrl.Row_idx <<= 1;
	 if(Mouth.Ctrl.Row_idx == 0)
	 {
		 Mouth.Ctrl.Row_idx = 1;
		 Mouth.Ctrl.Column_idx=0;
		 Mouth.Ctrl.frame_Sync = 1;
	 }
	 else
	 {
		 Mouth.Ctrl.frame_Sync = 0;
	 }

	 fAnimation_Runtime();
	 fsetMouth(Mouth.Ctrl.Row_idx, Mouth.Mouth_Data[Mouth.Ctrl.frame_idx][Mouth.Ctrl.Column_idx++]);
	 return Mouth.dwCtrl;
}

void fMouth_SetNoseColor(uint8_t value)
{
	ioport_set_pin_level(G_NOSE_B,!(value & 0x01));
	ioport_set_pin_level(G_NOSE_R,!(value & 0x02));
	ioport_set_pin_level(G_NOSE_G,!(value & 0x04));
}

void fMouth_SetData(uint32_t data)
{
	Mouth.Mouth_Data[0][0] = (data >> 24) & 0x1f;
	Mouth.Mouth_Data[0][1] = (data >> 16) & 0x1f;
	Mouth.Mouth_Data[0][2] = (data >> 8) & 0x1f;
	Mouth.Mouth_Data[0][3] = data & 0x1f;
}

void fConfig_Animation(uint16_t nFrames, uint16_t Repeat, uint16_t Periode)
{
	if(nFrames < NFRAMES)Mouth.Ctrl.nFrames = nFrames;
	Mouth.FramePeriode = Periode;
	Mouth.Ctrl.Anim_Rpt = (Repeat != 0);
	Mouth.Ctrl.Anim_En = 1;
}

void fTrig_Animation(void)
{
	Mouth.Ctrl.Anim_En = 1;
}

void fInit_Mouth(void)
{

	uint32_t x = 0;
	memset((uint8_t *)&Mouth, 0, sizeof(Mouth));
	fMouth_SetNoseColor(0);

	Mouth.Ctrl.Row_idx = 0x01;	// Set first Row active
	Mouth.Ctrl.Column_idx = 0x00;	// Set first Row active
	Mouth.Ctrl.Anim_En = 0;
	Mouth.Ctrl.Anim_Rpt = 0;
	Mouth.FramePeriode = 50;

	Mouth.Mouth_Data[x][3]= 0b00000;
	Mouth.Mouth_Data[x][2]= 0b00000;
	Mouth.Mouth_Data[x][1]= 0b00000;
	Mouth.Mouth_Data[x][0]= 0b00000;
	x++;

	Mouth.Mouth_Data[x][3]= 0b11111;
	Mouth.Mouth_Data[x][2]= 0b10001;
	Mouth.Mouth_Data[x][1]= 0b01110;
	Mouth.Mouth_Data[x][0]= 0b00000;
	x++;


	Mouth.Mouth_Data[x][3]= 0b11111;
	Mouth.Mouth_Data[x][2]= 0b10001;
	Mouth.Mouth_Data[x][1]= 0b10001;
	Mouth.Mouth_Data[x][0]= 0b01110;
	x++;

	Mouth.Mouth_Data[x][3]= 0b11111;
	Mouth.Mouth_Data[x][2]= 0b11111;
	Mouth.Mouth_Data[x][1]= 0b11111;
	Mouth.Mouth_Data[x][0]= 0b01110;
	x++;

	Mouth.Mouth_Data[x][3]= 0b11111;
	Mouth.Mouth_Data[x][2]= 0b11111;
	Mouth.Mouth_Data[x][1]= 0b01110;
	Mouth.Mouth_Data[x][0]= 0b00000;
	x++;

	Mouth.Mouth_Data[x][3]= 0b00000;
	Mouth.Mouth_Data[x][2]= 0b11111;
	Mouth.Mouth_Data[x][1]= 0b00000;
	Mouth.Mouth_Data[x][0]= 0b00000;
	x++;

	Mouth.Ctrl.nFrames = x;
	
	fsetMouth(Mouth.Ctrl.Row_idx, Mouth.Mouth_Data[0][0]);
	

}

