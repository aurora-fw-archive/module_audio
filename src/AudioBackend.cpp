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
				"Make sure you have a sound card and that is compatible with OpenAL.")) {}

		const char* AudioDeviceNotFoundException::what() const throw()
		{
			return _deviceName.c_str();
		}

		AudioBackend::AudioBackend()
		{
			// Starts OpenAL
			alGetError();	// This resets the error buffer
			setDevice(NULL);
		}

		AudioBackend::~AudioBackend()
		{
			// DEBUG: Commented for now to prove OpenAL is working
			alcCloseDevice(_device);
		}

		void AudioBackend::setDevice(const char *deviceName)
		{
			if(_device)
				alcCloseDevice(_device);
			
			_device = alcOpenDevice((ALCchar*)deviceName);

			if(!_device)
				throw AudioDeviceNotFoundException(deviceName);
		}

		AudioBackend* AudioBackend::_instance = nullptr;

		AudioBackend& AudioBackend::getInstance()
		{
			if(_instance == nullptr)
				_instance = new AudioBackend();

			return *_instance;
		}

		char* AudioBackend::getOutputDevices() const
		{
			if(alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT") == AL_TRUE) {
				if(alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT") == AL_TRUE) {
					const ALCchar *devices = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
					return (char*)devices;
				} else {
					const ALCchar *devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
					return (char*)devices;
				}
			} else {
				return "";
			}
		}

		char* AudioBackend::getInputDevices() const
		{
			if(alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT") == AL_TRUE) {
				const ALCchar *devices = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
				return (char*)devices;
			}
			return "";
		}
	}
}