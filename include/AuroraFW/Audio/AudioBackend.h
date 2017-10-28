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

// OpenAL
#include <AL/alc.h>
#include <AL/al.h>

// STD
#include <exception>

namespace AuroraFW {
	namespace AudioManager {
		class AudioDeviceNotFoundException: public std::exception
		{
		private:
			const std::string _deviceName;
		public:
			AudioDeviceNotFoundException(const char *);
			virtual const char* what() const throw();
		};

		class AFW_EXPORT AudioBackend {
		private:
			static AudioBackend *_instance;
			ALCdevice *_device;
			AudioBackend();
		public:
			static AudioBackend& getInstance();

			char* getOutputDevices() const;
			char* getInputDevices() const;
			void setDevice(const char *);
			~AudioBackend();
		};
	}
}

#endif	// AURORAFW_AUDIO_AUDIOBACKEND_H