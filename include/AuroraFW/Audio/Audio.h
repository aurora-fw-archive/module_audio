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
#include <AuroraFW/Math/Vector3D.h>

// LibSNDFile
#include <sndfile.h>

namespace AuroraFW {
	namespace AudioManager {
		enum class AudioFallout {
			Linear,
			Exponencial
		};

		enum class AudioLoopMode {
			Repeat,
			PingPong
		};

		enum class AudioStatus {
			Play,
			Pause,
			Stop
		};

		class AudioFileNotFound: public std::exception
		{
		private:
			const std::string _errorMessage;
		public:
			AudioFileNotFound(const char* );
			virtual const char* what() const throw();
		};

		struct AFW_EXPORT AudioStream {
			friend struct AudioSource;
			friend int audioCallback(const void* , void* , unsigned long , const PaStreamCallbackTimeInfo* , PaStreamCallbackFlags , void* );

			AudioStream(const char* = nullptr , int = SF_FORMAT_VORBIS, const AudioDevice& = AudioDevice());
			~AudioStream();

			void startStream();
			void stopStream();

			bool isStreamPlaying();
		private:
			SNDFILE *_soundFile = nullptr;
			PaStream *_paStream;
			bool streamPlaying;
		};

		struct AFW_EXPORT AudioSource {
			AudioSource(const AudioStream& , const Math::Vector3D = Math::Vector3D());
			AudioSource(const AudioStream& , const float = 0 , const float = 0 , const float = 0);
			AudioSource(AudioSource& );

			void setStream(const AudioStream& );
			const AudioStream& getStream();

			void setLooping(bool );
			void setLoopMode(const AudioLoopMode );

			void play();
			void pause();
			void stop();

			bool isPlaying();
			bool isPaused();
			bool isStopped();

			void setAudioFalloutType(const AudioFallout );

			AudioFallout falloutType;
			AudioStatus audioStatus;
			AudioLoopMode audioLoopMode;

			Math::Vector3D position;
			float medDistance;
			float maxDistance;
			float volume = 1;
			float pitch = 1;
		private:
			AudioStream _stream;
		};
	}
}

#endif	// AURORAFW_AUDIO_AUDIO_H