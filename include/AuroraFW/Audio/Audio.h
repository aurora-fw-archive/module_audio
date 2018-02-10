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

#ifndef AURORAFW_AUDIO_AUDIO_H
#define AURORAFW_AUDIO_AUDIO_H

// AuroraFW
#include <AuroraFW/Global.h>
#include <AuroraFW/Audio/AudioBackend.h>
#include <AuroraFW/Audio/AudioUtils.h>
#include <AuroraFW/Math/Algorithm.h>

// LibSNDFile
#include <sndfile.h>

namespace AuroraFW {
	namespace AudioManager {
		enum class AudioFallout {
			Linear,
			Exponencial
		};

		enum class AudioPlayMode {
			Once,
			Loop,
		};

		enum class AudioStatus {
			Play,
			Pause,
			Stop,
			// FIXME: This value should be internal to AuroraFW, find a solution so users don't mess with it
			CallbackStop
		};

		class AFW_EXPORT AudioFileNotFound : public std::exception
		{
		private:
			const std::string _errorMessage;
		public:
			AudioFileNotFound(const char* );
			virtual const char* what() const throw();
		};

		struct AFW_EXPORT AudioSource {
			AudioSource();
			AudioSource(const Math::Vector3D);
			AudioSource(const float , const float , const float );
			AudioSource(const AudioSource& );

			AudioFallout falloutType;

			void setPosition(Math::Vector3D );
			void setMedDistance(float );
			void setMaxDistance(float );

			const float getPanning();
			const float getStrength();

			const Math::Vector3D getPosition();
			const float getMedDistance();
			const float getMaxDistance();

			void calculateValues();

		private:
			void _calculatePan();
			void _calculateStrength();

			Math::Vector3D _position;
			float _medDistance;
			float _maxDistance;

			float _strength = 1;
			float _pan = 0;
		};

		struct AFW_EXPORT AudioOStream {
			friend struct AudioSource;
			friend int audioOutputCallback(const void* , void* , unsigned long , const PaStreamCallbackTimeInfo* , PaStreamCallbackFlags , void* );

			AudioOStream();
			AudioOStream(const char* , AudioSource* = nullptr, bool = false);
			~AudioOStream();

			void play();
			void pause();
			void stop();

			bool isPlaying();
			bool isPaused();
			bool isStopped();

			void setStreamPos(unsigned int );
			void setStreamPosFrame(unsigned int );

			AudioSource* getAudioSource();
			void setAudioSource(const AudioSource& );

			const float getNumLoops();

			AudioPlayMode audioPlayMode;
			AudioInfo audioInfo;

			float volume = 1;
			float pitch = 1;

		private:
			SNDFILE *_soundFile = nullptr;
			PaStream *_paStream;

			AudioStatus _audioStatus = AudioStatus::Stop;
			unsigned int _streamPosFrame = 0;
			uint8_t _loops = 0;
			
			AudioSource *_audioSource;
		};

		struct AFW_EXPORT AudioIStream {
			friend int audioInputCallback(const void* , void* , unsigned long , const PaStreamCallbackTimeInfo* , PaStreamCallbackFlags, void* );

			AudioIStream(const char* , unsigned int, unsigned int, uint8_t);
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
			float* buffer;
		private:
			SNDFILE *_soundFile = nullptr;
			SF_INFO _sndInfo;
		};

		int audioCallback(const void* , void* , unsigned long , const PaStreamCallbackTimeInfo* , PaStreamCallbackFlags , void* );

		int debugCallback(const void* , void* , unsigned long , const PaStreamCallbackTimeInfo* , PaStreamCallbackFlags , void* );

		// Inline definitions
		inline const float AudioOStream::getNumLoops()
		{
			return _loops;
		}
	}
}

#endif	// AURORAFW_AUDIO_AUDIO_H