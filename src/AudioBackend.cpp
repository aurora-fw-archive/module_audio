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
		const char* AudioDeviceNotFoundException::what() const throw()
		{
			return "OpenAL couldn't find an audio device. Make sure you have a sound card and that is compatible with OpenAL.";
		}

		AudioBackend::AudioBackend()
		{
			// Starts OpenAL
			alGetError();	// This resets the error buffer
			_device = alcOpenDevice(NULL);
			if(!_device) {
				throw AudioDeviceNotFoundException();
			}
		}

		AudioBackend::~AudioBackend()
		{
			// DEBUG: Commented for now to prove OpenAL is working
			//alcCloseDevice(_device);
		}

		AudioBackend* AudioBackend::_instance = nullptr;

		AudioBackend& AudioBackend::getInstance()
		{
			if(_instance == nullptr)
				_instance = new AudioBackend();

			return *_instance;
		}
	}
}