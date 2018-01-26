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

#ifndef AURORAFW_AUDIO_AUDIOBACKEND_H
#define AURORAFW_AUDIO_AUDIOBACKEND_H

// AuroraFW
#include <AuroraFW/Global.h>
#include <AuroraFW/STDL/STL/IOStream.h>
#include <AuroraFW/Core/Debug.h>
#include <AuroraFW/CLI/Log.h>
#include <AuroraFW/Math/Vector3D.h>

// PortAudio
#include <portaudio.h>

// STD
#include <exception>

namespace AuroraFW {
	namespace AudioManager {
		class PAErrorException: public std::exception
		{
			private:
				const std::string _paError;
			public:
				PAErrorException(const PaError& );
				virtual const char* what() const throw();
		};

		class AudioNotInitializedException: public std::exception
		{
			public:
				AudioNotInitializedException() {};
				virtual const char* what() const throw();
		};

		struct AFW_EXPORT AudioDevice {
			AudioDevice();
			AudioDevice(const PaDeviceInfo* );

			~AudioDevice() {};

			const char* getName() const;
			PaHostApiIndex getHostAPI() const;
			int getMaxInputChannels() const;
			int getMaxOutputChannels() const;
			PaTime getDefaultLowInputLatency() const;
			PaTime getDefaultLowOutputLatency() const;
			PaTime getDefaultHighInputLatency() const;
			PaTime getDefaultHighOutputLatency() const;
			double getDefaultSampleRate() const;

			bool isInputDevice() const;
			bool isOutputDevice() const;
			bool isDefaultOutputDevice() const;
			bool isDefaultInputDevice() const;

		private:
			const PaDeviceInfo *_deviceInfo;
		};

		class AFW_EXPORT AudioListener {
			private:
				static AudioListener *_instance;
				AudioListener() {};

				static void start();
				static void stop();
			public:
				friend struct AudioBackend;

				~AudioListener() {};

				static AudioListener& getInstance();

				Math::Vector3D position;
				Math::Vector3D direction = Math::Vector3D(0, 0, -1);
		};

		class AFW_EXPORT AudioBackend {
		private:
			static AudioBackend *_instance;
			AudioBackend();

			const AudioDevice* getDevices();

			int calcNumDevices();
			int calcNumOutputDevices();
			int calcNumInputDevices();

			int numDevices;
			int numOutputDevices;
			int numInputDevices;

		public:
			~AudioBackend() {};

			static void start();
			static AudioBackend& getInstance();
			static void terminate();

			const AudioDevice* getAllDevices();
			const AudioDevice* getOutputDevices();
			const AudioDevice* getInputDevices();

			void setInputDevice(AudioDevice );
			void setOutputDevice(AudioDevice );

			int getNumDevices();
			int getNumOutputDevices();
			int getNumInputDevices();
		};

		// Inline definitions
		inline void getPAError(const PaError& error)
		{
			if(error != paNoError)
				throw PAErrorException(error);
		}

		inline int AudioBackend::calcNumDevices()
		{
			return Pa_GetDeviceCount();
		}

		inline int AudioBackend::getNumDevices()
		{
			return numDevices;
		}

		inline int AudioBackend::getNumOutputDevices()
		{
			return numOutputDevices;
		}
		
		inline int AudioBackend::getNumInputDevices()
		{
			return numInputDevices;
		}
	}
}

#endif	// AURORAFW_AUDIO_AUDIOBACKEND_H