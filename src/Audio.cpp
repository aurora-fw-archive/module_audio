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

#include <AuroraFW/Audio/Audio.h>

namespace AuroraFW {
	namespace AudioManager {
		AudioFileNotFound::AudioFileNotFound(const char *fileName)
			: _errorMessage(std::string("The specified audio file \"" + std::string(fileName)
				+ "\" couldn't be found/read!")) {}

		const char* AudioFileNotFound::what() const throw()
		{
			return _errorMessage.c_str();
		}

		AudioStream::AudioStream(const char *path, int format)
		{
			// If path is null, run the debug callback
			if(path == nullptr) {
				AuroraFW::Debug::Log("Debug mode activated for AudioStream instance");
				getPAError(Pa_OpenDefaultStream(&_paStream, 0, 2, paFloat32,
											44100, 256, debugCallback, NULL));
			} else {
				// Gets the SNDFILE* from libsndfile
				SF_INFO sndInfo;
				sndInfo.format = format;

				_soundFile = sf_open(path, SFM_READ, &sndInfo);

				// If the soundFile is null, it means there was no audio file
				if(_soundFile == nullptr)
					throw AudioFileNotFound(path);

				// DEBUG: Open the PortAudio stream with debug callback
				getPAError(Pa_OpenDefaultStream(&_paStream, 0, 2, paFloat32,
												44100, 256, audioCallback, NULL));
			}
		}

		AudioStream::~AudioStream()
		{
			// Closes the soundFile
			if(_soundFile != nullptr)
				sf_close(_soundFile);
		}

		void AudioStream::startStream()
		{
			getPAError(Pa_StartStream(_paStream));
		}

		void AudioStream::stopStream()
		{
			getPAError(Pa_StopStream(_paStream));
		}

		AudioSource::AudioSource(const AudioStream& stream, const Math::Vector3D vec)
			: position(vec), _stream(stream) {}

		AudioSource::AudioSource(const AudioStream& stream, const float x, const float y, const float z)
			: position(Math::Vector3D(x, y, z)), _stream(stream) {}
	}
}