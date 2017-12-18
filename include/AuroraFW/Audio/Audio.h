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
			Stop,
			CallbackStop
		};

		class AudioFileNotFound: public std::exception
		{
		private:
			const std::string _errorMessage;
		public:
			AudioFileNotFound(const char* );
			virtual const char* what() const throw();
		};

		

		struct AFW_EXPORT AudioSource {
			AudioSource(const Math::Vector3D = Math::Vector3D());
			AudioSource(const float = 0 , const float = 0 , const float = 0);
			AudioSource(const AudioSource& );

			AudioFallout falloutType;

			void setPosition(Math::Vector3D );
			const float getPanning();
			const Math::Vector3D getPosition();

			float medDistance;
			float maxDistance;
		private:
			void calculateValues();
			Math::Vector3D _position;
			float _pan;
		};

		struct AFW_EXPORT AudioStream {
			friend struct AudioSource;
			friend int audioCallback(const void* , void* , unsigned long , const PaStreamCallbackTimeInfo* , PaStreamCallbackFlags , void* );

			AudioStream(const char* = nullptr, const AudioDevice& = AudioDevice(), AudioSource* = nullptr);
			~AudioStream();

			void play();
			void pause();
			void stop();

			bool isPlaying();
			bool isPaused();
			bool isStopped();

			void setStreamPos(unsigned int );
			void setStreamPosFrame(unsigned int );

			void setLooping(bool );

			AudioSource* getAudioSource();
			void setAudioSource(const AudioSource& );
			
			AudioLoopMode audioLoopMode;

			float volume = 1;
			float pitch = 1;
		private:
			SNDFILE *_soundFile = nullptr;
			SF_INFO _sndInfo;
			PaStream *_paStream;

			AudioStatus _audioStatus = AudioStatus::Stop;
			unsigned int _streamPosFrame = 0;
			bool _looping = false;
			
			AudioSource *_audioSource;
		};

		int audioCallback(const void* , void* , unsigned long , const PaStreamCallbackTimeInfo* , PaStreamCallbackFlags , void* );

		int debugCallback(const void* , void* , unsigned long , const PaStreamCallbackTimeInfo* , PaStreamCallbackFlags , void* );
	}
}

#endif	// AURORAFW_AUDIO_AUDIO_H