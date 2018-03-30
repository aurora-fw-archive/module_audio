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
#include <AuroraFW/Core/DebugManager.h>
#include <AuroraFW/CLI/Log.h>
#include <AuroraFW/Math/Vector3D.h>

// PortAudio
#include <portaudio.h>

// LibSNDFile
#include <sndfile.h>

// STD
#include <exception>

namespace AuroraFW {
	namespace AudioManager {
		class AFW_API PAErrorException : public std::exception
		{
		private:
			const std::string _paError;
		public:
			PAErrorException(const PaError& );
			virtual const char* what() const throw();
		};

		class AFW_API SNDFILEErrorException : public std::exception
		{
		private:
			const std::string _sndFileError;
		public:
			SNDFILEErrorException(const int& );
			virtual const char* what() const throw();
		};

		class AFW_API AudioNotInitializedException : public std::exception
		{
		public:
			AudioNotInitializedException() {};
			virtual const char* what() const throw();
		};

		struct AFW_API AudioDevice {
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

		class AFW_API AudioListener {
			private:
				static AudioListener *_instance;
				AudioListener() {};

				static void _start();
				static void _stop();
			public:
				friend struct AudioBackend;

				~AudioListener() {};

				static AudioListener& getInstance();

				Math::Vector3D position;
				Math::Vector3D direction = Math::Vector3D(0, 0, -1);
		};

		class AFW_API AudioBackend {
		private:
			static AudioBackend *_instance;
			AudioBackend();

			const AudioDevice* _getDevices();

			int _calcNumOutputDevices();
			int _calcNumDevices();
			int _calcNumInputDevices();

			int _numDevices;
			int _numOutputDevices;
			int _numInputDevices;

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

			float globalVolume = 1;
		};

		// Inline definitions
		inline void catchPAProblem(const PaError& error)
		{
			if(error != paNoError)
				throw PAErrorException(error);
		}

		inline void catchSNDFILEProblem(const int& error)
		{
			if(error != SF_ERR_NO_ERROR)
				throw SNDFILEErrorException(error);
		}

		inline int AudioBackend::getNumDevices()
		{
			return _numDevices;
		}

		inline int AudioBackend::getNumOutputDevices()
		{
			return _numOutputDevices;
		}
		
		inline int AudioBackend::getNumInputDevices()
		{
			return _numInputDevices;
		}
	}
}

#endif // AURORAFW_AUDIO_AUDIOBACKEND_H