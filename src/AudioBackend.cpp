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
		// PAErrorException
		PAErrorException::PAErrorException(const PaError& paError)
			: _paError(std::string("PortAudio error: " + std::string(Pa_GetErrorText(paError))))
		{}
		
		const char* PAErrorException::what() const throw()
		{
			return _paError.c_str();
		}

		// audioCallBack
		int audioCallback(const void *inputBuffer, void *outputBuffer,
										unsigned long framesPerBuffer,
										const PaStreamCallbackTimeInfo *timeInfo,
										PaStreamCallbackFlags statusFlags,
										void *userData)
		{
			// TODO
		}

		// debugCallBack
		int debugCallback(const void *inputBuffer, void *outputBuffer,
						unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo,
						PaStreamCallbackFlags statusFlags, void *userData)
		{
			float left_ear = -1;
			float right_ear = -1;

			float *output = (float*)outputBuffer;

			for(unsigned int i = 0; i < framesPerBuffer; i++) {
				*output++ = left_ear;
				*output++ = right_ear;

				left_ear += 0.01f;
				if(left_ear >= 1.0f)
					left_ear -= 2.0f;

				right_ear += 0.05f;
				if(right_ear >= 1.0f)
					right_ear -= 2.0f;
			}

			return 0;
		}

		// AudioBackend
		const AudioDevice* AudioBackend::getDevices()
		{
			int numDevices = getNumDevices();
			AudioDevice *audioDevices = new AudioDevice[numDevices];
			for(int i = 0; i < numDevices; i++) {
				audioDevices[i] = AudioDevice(Pa_GetDeviceInfo(i));
			}
			
			return const_cast<const AudioDevice*>(audioDevices);
		}

		int AudioBackend::calcNumOutputDevices()
		{
			int numDevices = getNumDevices();
			const AudioDevice *audioDevices = getDevices();
			for(int i = 0; i < getNumDevices(); i++) {
				if(!audioDevices[i].isOutputDevice())
					numDevices--;
			}

			delete[] audioDevices;

			return numDevices;
		}

		int AudioBackend::calcNumInputDevices()
		{
			int numDevices = getNumDevices();
			const AudioDevice *audioDevices = getDevices();
			for(int i = 0; i < getNumDevices(); i++) {
				if(!audioDevices[i].isInputDevice())
					numDevices--;
			}

			delete[] audioDevices;

			return numDevices;
		}

		AudioBackend::AudioBackend()
		{
			// Starts PortAudio
			getPAError(Pa_Initialize());

			// Gets number of devices
			numDevices = calcNumDevices();
			numOutputDevices = calcNumOutputDevices();
			numInputDevices = calcNumInputDevices();

			// Prints verbose
			AuroraFW::Debug::Log("AudioBackend initialized. Num. of available audio devices: ", numDevices,
								"(", numOutputDevices, " output devices, ",
								numInputDevices, " input devices.)");
		}

		AudioBackend::~AudioBackend()
		{
			// Stops PortAudio
			getPAError(Pa_Terminate());

			// Prints verbose
			AuroraFW::Debug::Log("AudioBackend was terminated.");
		}

		AudioBackend* AudioBackend::_instance = nullptr;

		AudioBackend& AudioBackend::getInstance()
		{
			if(_instance == nullptr)
				_instance = new AudioBackend();
			return *_instance;
		}

		const AudioDevice* AudioBackend::getAllDevices()
		{
			return getDevices();
		}

		const AudioDevice* AudioBackend::getOutputDevices()
		{
			AudioDevice *audioDevices = new AudioDevice[numOutputDevices];
			int trueIndex = 0;
			for(int i = 0; i < numDevices; i++) {
				AudioDevice audioDevice(Pa_GetDeviceInfo(i));
				if(audioDevice.getMaxOutputChannels() > 0) {
					audioDevices[trueIndex] = audioDevice;
					trueIndex++;
				}
			}

			return audioDevices;
		}

		const AudioDevice* AudioBackend::getInputDevices()
		{
			AudioDevice *audioDevices = new AudioDevice[numInputDevices];
			int trueIndex = 0;
			for(int i = 0; i < numDevices; i++) {
				AudioDevice audioDevice(Pa_GetDeviceInfo(i));
				if(audioDevice.getMaxInputChannels() > 0) {
					audioDevices[trueIndex] = audioDevice;
					trueIndex++;
				}
			}

			return audioDevices;
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

		bool AudioDevice::isDefaultOutputDevice() const
		{
			return Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice()) == _deviceInfo;
		}

		bool AudioDevice::isDefaultInputDevice() const
		{
			return Pa_GetDeviceInfo(Pa_GetDefaultInputDevice()) == _deviceInfo;
		}
	}
}