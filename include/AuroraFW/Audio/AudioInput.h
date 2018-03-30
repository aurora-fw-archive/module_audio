/****************************************************************************
** ┌─┐┬ ┬┬─┐┌─┐┬─┐┌─┐  ┌─┐┬─┐┌─┐┌┬┐┌─┐┬ ┬┌─┐┬─┐┬┌─
** ├─┤│ │├┬┘│ │├┬┘├─┤  ├┤ ├┬┘├─┤│││├┤ ││││ │├┬┘├┴┐
** ┴ ┴└─┘┴└─└─┘┴└─┴ ┴  └  ┴└─┴ ┴┴ ┴└─┘└┴┘└─┘┴└─┴ ┴
** A Powerful General Purpose Framework
** More information in: https://aurora-fw.github.io/
**
** Copyright (C) 2017 Aurora Framework, All rights reserved.
**
** This file is part of the Aurora Framework. This framework is free
** software; you can redistribute it and/or modify it under the terms of
** the GNU Lesser General Public License version 3 as published by the
** Free Software Foundation and appearing in the file LICENSE included in
** the packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
****************************************************************************/

#ifndef AURORAFW_AUDIO_AUDIO_INPUT_H
#define AURORAFW_AUDIO_AUDIO_INPUT_H

// AuroraFW
#include <AuroraFW/Global.h>
#include <AuroraFW/Audio/AudioUtils.h>
#include <AuroraFW/Audio/AudioBackend.h>

namespace AuroraFW {
	namespace AudioManager {
		struct AFW_API AudioIStream {
			friend int audioInputCallback(const void* , void* , size_t ,
			const PaStreamCallbackTimeInfo* , PaStreamCallbackFlags, void* );

			AudioIStream(const char* , AudioInfo* , int );
			~AudioIStream();

			void record();
			void pause();
			void stop();

			bool isRecording();
			bool isPaused();
			bool isStopped();
			bool isBufferFull();

			void clearBuffer();
			void clearBuffer(unsigned int start, unsigned int finish);

			bool save();

			const char* path;
			AudioInfo* info;
			float* buffer;	// TODO - Check if this needs to be private
			const int bufferSize;

		private:
			PaStream* _paStream;
			unsigned int _streamPosFrame = 0;
		};
	}
}

#endif // AURORAFW_AUDIO_AUDIO_INPUT_H
