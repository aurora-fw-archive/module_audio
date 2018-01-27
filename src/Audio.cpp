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

#include <AuroraFW/Audio/Audio.h>

namespace AuroraFW {
	namespace AudioManager {
		// AudioFileNotFound
		AudioFileNotFound::AudioFileNotFound(const char *fileName)
			: _errorMessage(std::string("The specified audio file \"" + std::string(fileName)
				+ "\" couldn't be found/read!")) {}

		const char* AudioFileNotFound::what() const throw()
		{
			return _errorMessage.c_str();
		}

		// audioOutputCallback
		int audioOutputCallback(const void* inputBuffer, void* outputBuffer,
						unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
						PaStreamCallbackFlags statusFlags, void* userData)
		{
			// Gets the output buffer (it's of type paInt32)
			int* output = (int*)outputBuffer;
			AudioOStream *audioOStream = (AudioOStream*)userData;

			// In case the audio stream is paused, saves the current position and stops the stream
			if(audioOStream->_audioStatus == AudioStatus::Pause) {
				audioOStream->_streamPosFrame = sf_seek(audioOStream->_soundFile, 0, SF_SEEK_CUR);
				return paComplete;
			}

			int readFrames = sf_readf_int(audioOStream->_soundFile, output, framesPerBuffer);
			audioOStream->_streamPosFrame += readFrames;

			// Adjusts the volume of each frame
			for(unsigned int i = 0; i < readFrames; i++) {
				// Applies the volume to all channels the sound might have
				for(uint8_t channels = 0; channels < audioOStream->_sndInfo.channels; channels++) {
					float frame = *output;
					
					// In case there's 3D audio, calculates 3D audio
					if(audioOStream->_audioSource != nullptr) {
						AudioSource* source = audioOStream->_audioSource;
						const float panning = source->getPanning();
						if(channels == 0)
							frame *= (-0.5f * panning + 0.5f);
						else if(channels == 1)
							frame *= (0.5f * panning + 0.5f);
					}

					frame *= audioOStream->volume;

					*output++ = frame;
				}
			}

			// If the read frames filled the buffer to read, continues playing
			if(readFrames == framesPerBuffer) {
				return paContinue;
			// Else it means it reached end of file.
			} else {
				// If the song should be played once, stop the callback
				if(audioOStream->audioPlayMode == AudioPlayMode::Once) {
					audioOStream->_audioStatus = AudioStatus::CallbackStop;
					return paComplete;
				// Else if the song should be repeated, do so
				} else if(audioOStream->audioPlayMode == AudioPlayMode::Loop) {
					// TODO: This leaves a noticeable mark in the buffer that the sound is looping,
					// it should be seamingless
					audioOStream->_streamPosFrame = 0;
					audioOStream->_loops++;
					sf_seek(audioOStream->_soundFile, 0, SEEK_SET);
					return paContinue;
				}
			}
		}

		// audioInputCallback
		int audioInputCallback(const void* inputBuffer, void* outputBuffer,
						unsigned long int framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo,
						PaStreamCallbackFlags statusFlags, void *userData)
		{

		}

		// debugCallBack
		int debugCallback(const void *inputBuffer, void *outputBuffer,
						unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo,
						PaStreamCallbackFlags statusFlags, void *userData)
		{
			uint8_t left_ear = 0;
			uint8_t right_ear = 0;

			uint8_t *output = (uint8_t*)outputBuffer;

			for(unsigned int i = 0; i < framesPerBuffer; i++) {
				*output++ = left_ear;
				*output++ = right_ear;

				left_ear += 1;
				if(left_ear >= 255)
					left_ear -= -255;

				right_ear += 3;
				if(right_ear >= 255)
					right_ear -= -255;
			}

			return 0;
		}

		// AudioSource
		AudioSource::AudioSource()
			: _position(Math::Vector3D())
		{
			calculateValues();
		}

		AudioSource::AudioSource(const Math::Vector3D vec)
			: _position(vec)
		{
			calculateValues();
		}

		AudioSource::AudioSource(const float x, const float y, const float z)
			: _position(Math::Vector3D(x, y, z))
		{
			calculateValues();
		}

		AudioSource::AudioSource(const AudioSource& audioSource)
			: falloutType(audioSource.falloutType), _position(audioSource._position),
				_medDistance(audioSource._medDistance), _maxDistance(audioSource._maxDistance)
		{
			calculateValues();
		}

		void AudioSource::setPosition(Math::Vector3D position)
		{
			_position = position;
			_calculatePan();
		}

		void AudioSource::setMedDistance(float medDistance)
		{
			_medDistance = medDistance;
			_calculateStrength();
		}

		void AudioSource::setMaxDistance(float maxDistance)
		{
			_maxDistance = maxDistance;
			_calculateStrength();
		}

		const float AudioSource::getPanning()
		{
			return _pan;
		}

		const float AudioSource::getStrength()
		{
			return _strength;
		}

		const Math::Vector3D AudioSource::getPosition()
		{
			return _position;
		}

		const float AudioSource::getMedDistance()
		{
			return _medDistance;
		}

		const float AudioSource::getMaxDistance()
		{
			return _maxDistance;
		}

		void AudioSource::calculateValues()
		{
			_calculatePan();
			_calculateStrength();
		}

		void AudioSource::_calculatePan()
		{
			Math::Vector3D listenerPos = AudioListener::getInstance().position;
			Math::Vector3D listenerDir = AudioListener::getInstance().direction;
			// TODO: Right now it's hardcoded because the camera, under normal circunstances,
			//	doesn't "tilt". Make less dirty later
			Math::Vector3D listenerUp = Math::Vector3D(0, 1, 0);
			// FIXME: The method to obtain the perpendicular to both dir and lookUp didn't use matrixes
			// , because I am inexperienced on it. This is where I got the formula:
			// https://math.stackexchange.com/questions/501949/determining-a-perpendicular-vector-to-two-given-vectors
			// however, this is dirty and should be cleaned up
			float x = listenerDir.y * listenerUp.z - listenerDir.z * listenerUp.y;
			float y = listenerDir.z * listenerUp.x - listenerDir.x * listenerUp.z;
			float z = listenerDir.x * listenerUp.y - listenerDir.y * listenerUp.x;

			Math::Vector3D cross = Math::Vector3D(x, y, z);
			cross.normalize();

			// Makes a copy of position, since it needs to be manipulated to calculate the panning
			Math::Vector3D sourcePos = _position;
			sourcePos -= listenerPos;

			_pan = cross.dot(sourcePos.normalized());
		}

		void AudioSource::_calculateStrength()
		{
			// TODO: Implement
			float distance = Math::abs(_position.distanceToPoint(AudioListener::getInstance().position));
		}

		// AudioOStream
		AudioOStream::AudioOStream()
			: _audioSource(nullptr)
		{
			AuroraFW::Debug::Log("Debug mode activated for AudioOStream instance");

			AudioDevice device;

			getPAError(Pa_OpenDefaultStream(&_paStream, 0, 2, paUInt8,
				device.getDefaultSampleRate(), 256, debugCallback, NULL));
		}

		AudioOStream::AudioOStream(const char *path, AudioSource *audioSource)
			: _audioSource(audioSource)
		{
			// Gets the SNDFILE* from libsndfile
			_sndInfo.format = 0;

			_soundFile = sf_open(path, SFM_READ, &_sndInfo);

			// If the soundFile is null, it means there was no audio file
			if(_soundFile == nullptr)
				throw AudioFileNotFound(path);

			AudioDevice device;

			// Opens the audio stream
			getPAError(Pa_OpenDefaultStream(&_paStream, 0, _sndInfo.channels, paInt32,
				device.getDefaultSampleRate(), paFramesPerBufferUnspecified, audioOutputCallback, this));
		}

		AudioOStream::~AudioOStream()
		{
			// Closes the soundFile
			if(_soundFile != nullptr)
				sf_close(_soundFile);

			// Deletes audioSource
			delete _audioSource;
		}

		void AudioOStream::play()
		{
			if(_audioStatus == AudioStatus::CallbackStop)
				stop();
			else if(_audioStatus == AudioStatus::Pause)
				getPAError(Pa_StopStream(_paStream));
			
			_audioStatus = AudioStatus::Play;
			sf_seek(_soundFile, _streamPosFrame, SF_SEEK_SET);
			getPAError(Pa_StartStream(_paStream));
		}

		void AudioOStream::pause()
		{
			// No PortAudio method is called. During the callback,
			// the stream is stopped and the current frame stored.
			_audioStatus = AudioStatus::Pause;
		}

		void AudioOStream::stop()
		{
			_streamPosFrame = 0;
			
			_audioStatus = AudioStatus::Stop;
			getPAError(Pa_StopStream(_paStream));
		}

		bool AudioOStream::isPlaying()
		{
			return _audioStatus == AudioStatus::Play;
		}

		bool AudioOStream::isPaused()
		{
			return _audioStatus == AudioStatus::Pause;
		}

		bool AudioOStream::isStopped()
		{
			return _audioStatus == AudioStatus::Stop;
		}

		void AudioOStream::setStreamPos(unsigned int pos)
		{
			// TODO: Implement
		}

		void AudioOStream::setStreamPosFrame(unsigned int pos)
		{
			_streamPosFrame = pos;
		}

		AudioSource* AudioOStream::getAudioSource()
		{
			return _audioSource;
		}

		void AudioOStream::setAudioSource(const AudioSource& audioSource)
		{
			delete _audioSource;
			_audioSource = new AudioSource(audioSource);
		}

		// AudioIStream
		AudioIStream::AudioIStream(const char* path, unsigned int samplerate, unsigned int frames, uint8_t channels)
		{

		}
	}
}