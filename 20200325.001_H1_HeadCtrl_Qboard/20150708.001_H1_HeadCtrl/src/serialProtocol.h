/*********************************************************************
 *
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2011, TheCorpora.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the TheCorpora nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.

 *
 * \author Miguel Angel Julian
 *********************************************************************/

#ifndef SerialProtocol_h
#define SerialProtocol_h

	#include "comands.h"
	#include "Mouth.h"

	#define BOARD_ID 1
	#define LIBRARY_VERSION 1

	#define INPUT_FLAG		0xff
	#define OUTPUT_FLAG 	0xfe
	#define INPUT_ESCAPE 	0xfd

	uint8_t commandNumber;
	uint8_t nInputData;
	uint8_t inputData[128];
	uint8_t nOutputData;
	uint8_t outputData[128];

	typedef struct tagCCommand
	{
	  uint8_t commandNumber;
	  uint8_t nInputData;
	  uint8_t inputData[128];
	  uint8_t nOutputData;
	  uint8_t outputData[128];
	}CCommand;

	CCommand command_;

	bool isInputEscaped_;
	bool isInputCorrect_;

	void fSerial_InitSerialProtocol(uint32_t *);
	void fSerial_Write(uint8_t*, uint32_t);
	void fSerial_processCommands(void);
	bool fSerial_procesaEntrada(uint8_t* buf, uint8_t length);
	void fSerial_sendResponse(void);
	void fSerial_sendNack(void);
	void fSerial_processSerial(void);
	uint8_t pearson(uint8_t *key, uint8_t len);
#endif

