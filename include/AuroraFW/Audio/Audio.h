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

		struct AFW_EXPORT AudioStream {
			AudioStream();
			AudioStream(const char *);
			AudioStream(AudioStream& );

			void setFile(const char *);
			
		private:
			PaStream *paStream;
		};

		struct AFW_EXPORT AudioSource {
			AudioSource();
			AudioSource(float , float , float , AudioStream& );
			AudioSource(AudioSource& );

			void setPosition(float , float , float );
			void setMedDistance(float );
			void setMaxDistance(float );

			void setVolume(float );
			void setPitch(float );

			void setStream(const AudioStream& );
			const AudioStream& getStream();

			void setLooping(bool );
			void setLoopMode(const AudioLoopMode );

			void setPosition(float );
			float getPosition();

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
			AudioStream audioStream;
		};
	}
}

#endif	// AURORAFW_AUDIO_AUDIO_H