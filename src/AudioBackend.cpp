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

#include <AuroraFW/Audio/AudioBackend.h>

namespace AuroraFW {
	namespace AudioManager {
		AudioDeviceNotFoundException::AudioDeviceNotFoundException(const char *deviceName)
			: _deviceName(deviceName
				? std::string("The device \"" + std::string(deviceName) + "\" does not exist!")
				: std::string("OpenAL couldn't find an audio device. "
				"Make sure you have a sound card and that is compatible with OpenAL."))
		{}

		const char* AudioDeviceNotFoundException::what() const throw()
		{
			return _deviceName.c_str();
		}

		PAErrorException::PAErrorException(const PaError& paError)
			: _paError(std::string("PortAudio error: " + std::string(Pa_GetErrorText(paError))))
		{}
		
		const char* PAErrorException::what() const throw()
		{
			return _paError.c_str();
		}

		void AudioBackend::getPAError(const PaError& error)
		{
			if(error != paNoError)
				throw PAErrorException(error);
			return;
		}

		AudioBackend::AudioBackend(const char *deviceName)
		{
			// Start PortAudio
			getPAError(Pa_Initialize());
		}

		AudioBackend::~AudioBackend()
		{
			// Stops PortAudio
			getPAError(Pa_Terminate());
		}

		int AudioBackend::audioCallback(const void *inputBuffer, void *outputBuffer,
										unsigned long framesPerBuffer,
										const PaStreamCallbackTimeInfo *timeInfo,
										PaStreamCallbackFlags statusFlags,
										void *userData)
		{}

		AudioBackend* AudioBackend::_instance = nullptr;

		AudioBackend& AudioBackend::getInstance()
		{
			if(_instance == nullptr)
				_instance = new AudioBackend();
			return *_instance;
		}

		/*const AudioDevice * AudioBackend::getAllDevices()
		{
			int numDevices = getNumDevices();
			AudioDevice *audioDevices = new AudioDevice [numDevices];
			for(int i = 0; i < numDevices; i++) {
				audioDevices[i] = AudioDevice(Pa_GetDeviceInfo(i));
			}
		}*/

		AudioDevice* AudioBackend::getAllDevices()
		{
			int numDevices = getNumDevices();
			AudioDevice *audioDevices = new AudioDevice[numDevices];
			for(int i = 0; i < numDevices; i++) {
				audioDevices[i] = AudioDevice(Pa_GetDeviceInfo(i));
			}

			return audioDevices;
		}

		const AudioDevice * AudioBackend::getOutputDevices()
		{
			// TODO
		}

		const AudioDevice * AudioBackend::getInputDevices()
		{
			// TODO
		}

		AudioDevice::AudioDevice()
			: _deviceInfo(Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice())) {}

		AudioDevice::AudioDevice(const PaDeviceInfo *deviceInfo)
			: _deviceInfo(deviceInfo) {}

		const char* AudioDevice::getName() const
		{
			return _deviceInfo->name;
		}

		PaHostApiIndex AudioDevice::getHostAPI() const
		{
			return _deviceInfo->hostApi;
		}

		int AudioDevice::getMaxInputChannels() const
		{
			return _deviceInfo->maxInputChannels;
		}

		int AudioDevice::getMaxOutputChannels() const
		{
			return _deviceInfo->maxOutputChannels;
		}

		PaTime AudioDevice::getDefaultLowInputLatency() const
		{
			return _deviceInfo->defaultLowInputLatency;
		}

		PaTime AudioDevice::getDefaultLowOutputLatency() const
		{
			return _deviceInfo->defaultLowOutputLatency;
		}

		PaTime AudioDevice::getDefaultHighInputLatency() const
		{
			return _deviceInfo->defaultHighInputLatency;
		}

		PaTime AudioDevice::getDefaultHighOutputLatency() const
		{
			return _deviceInfo->defaultHighOutputLatency;
		}

		double AudioDevice::getDefaultSampleRate() const
		{
			return _deviceInfo->defaultSampleRate;
		}

		bool AudioDevice::isInputDevice() const
		{
			return getMaxInputChannels() > 0;
		}

		bool AudioDevice::isOutputDevice() const
		{
			return getMaxOutputChannels() > 0;
		}
	}
}