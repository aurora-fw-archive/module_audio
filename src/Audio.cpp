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
						size_t framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
						PaStreamCallbackFlags statusFlags, void* userData)
		{
			// Gets the output buffer (it's of type paInt32)
			int* output = (int*)outputBuffer;
			AudioOStream *audioStream = (AudioOStream*)userData;
			AudioInfo *audioInfo = &(audioStream->audioInfo);

			// In case the audio stream is paused, saves the current position and stops the stream
			if(audioStream->isPaused()) {
				audioStream->_streamPosFrame = sf_seek(audioInfo->_sndFile, 0, SF_SEEK_CUR);
				return paComplete;
			}

			// Reads the audio
			size_t readFrames;
			if(audioStream->_buffer != nullptr) {	// Buffered
				readFrames = (framesPerBuffer + audioStream->_streamPosFrame) > audioInfo->getFrames()
							? audioInfo->getFrames() - audioStream->_streamPosFrame
							: framesPerBuffer;

				for(size_t i = 0; i < readFrames; i++) {
					for(uint8_t channels = 0; channels < audioInfo->getChannels(); channels++) {
						output[i * audioInfo->getChannels() + channels] = audioStream->_buffer[audioStream->_streamPosFrame * audioInfo->getChannels() + (i * audioInfo->getChannels() + channels)];
					}
				}
				

			} else {	// Streaming
				readFrames = sf_readf_int(audioInfo->_sndFile, output, framesPerBuffer);
			}

			audioStream->_streamPosFrame += readFrames;

			// Adjusts the volume of each frame
			for(size_t i = 0; i < readFrames; i++) {
				// Applies the volume to all channels the sound might have
				for(uint8_t channels = 0; channels < audioInfo->getChannels(); channels++) {
					float frame = *output;

					// In case there's 3D audio, calculates 3D audio
					if(audioStream->_audioSource != nullptr) {
						AudioSource* source = audioStream->_audioSource;
						const float panning = source->getPanning();
						if(channels == 0)
							frame *= (-0.5f * panning + 0.5f);
						else if(channels == 1)
							frame *= (0.5f * panning + 0.5f);
					}

					frame *= audioStream->volume;

					*output++ = frame;
				}
			}

			// If the read frames didn't fill the buffer to read, it reached EOF
			if(readFrames < framesPerBuffer) {
				// If the song should be repeated, do so
				if(audioStream->audioPlayMode == AudioPlayMode::Loop) {
					#pragma message ("TODO: This leaves a noticeable mark in the buffer that the sound is looping, it shoudl be seamingless.")

					audioStream->_streamPosFrame = 0;
					audioStream->_loops++;
					sf_seek(audioInfo->_sndFile, 0, SEEK_SET);

					return paContinue;
				} else {
					return paComplete;
				}
			}

			return paContinue;
		}

		// audioInputCallback
		int audioInputCallback(const void* inputBuffer, void* outputBuffer,
						size_t framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo,
						PaStreamCallbackFlags statusFlags, void *userData)
		{
			#pragma message ("TODO: Need to be implemented")
		}

		// debugCallBack
		int debugCallback(const void *inputBuffer, void *outputBuffer,
						size_t framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo,
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

		float AudioSource::getPanning()
		{
			return _pan;
		}

		float AudioSource::getStrength()
		{
			return _strength;
		}

		Math::Vector3D AudioSource::getPosition()
		{
			return _position;
		}

		float AudioSource::getMedDistance()
		{
			return _medDistance;
		}

		float AudioSource::getMaxDistance()
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
			#pragma message ("TODO: Need to be implemented")
			float distance = Math::abs(_position.distanceToPoint(AudioListener::getInstance().position));
		}

		// AudioOStream
		AudioOStream::AudioOStream()
			: _audioSource(nullptr)
		{
			AuroraFW::DebugManager::Log("Debug mode activated for AudioStream instance");

			AudioDevice device;

			catchPAProblem(Pa_OpenDefaultStream(&_paStream, 0, 2, paUInt8,
				device.getDefaultSampleRate(), 256, debugCallback, NULL));
		}

		AudioOStream::AudioOStream(const char *path, AudioSource *audioSource, bool buffered)
			: audioInfo(nullptr, nullptr), _audioSource(audioSource)
		{
			SF_INFO* sndInfo = new SF_INFO();
			sndInfo->format = 0;

			#pragma message ("FIXME: Two instances of SNDFILE due to inclusion of AudioInfo, optimize that")

			audioInfo._sndInfo = sndInfo;
			audioInfo._sndFile = sf_open(path, SFM_READ, audioInfo._sndInfo);

			// If the audio should be buffered, do so
			if(buffered) {
				AuroraFW::DebugManager::Log("Buffering the audio... (Total frames: ", audioInfo.getFrames() * audioInfo.getChannels(), ")");
				_buffer = AFW_NEW int[audioInfo.getFrames() * audioInfo.getChannels()];
				sf_readf_int(audioInfo._sndFile, _buffer, audioInfo.getFrames());
				AuroraFW::DebugManager::Log("Buffering complete.");
			}

			// If the soundFile is null, it means there was no audio file
			if(audioInfo._sndFile == nullptr)
				throw AudioFileNotFound(path);

			AudioDevice device;

			// Opens the audio stream
			catchPAProblem(Pa_OpenDefaultStream(&_paStream, 0, audioInfo.getChannels(), paInt32,
				device.getDefaultSampleRate(), paFramesPerBufferUnspecified, audioOutputCallback, this));
		}

		AudioOStream::~AudioOStream()
		{
			// Closes the soundFile
			if(audioInfo._sndFile != AFW_NULLPTR)
				catchSNDFILEProblem(sf_close(audioInfo._sndFile));

			// Deletes the buffer
			if(_buffer != AFW_NULLPTR)
				delete[] _buffer;

			// Deletes audioSource
			if(_audioSource != AFW_NULLPTR)
				delete _audioSource;
		}

		void AudioOStream::play()
		{
			sf_seek(audioInfo._sndFile, _streamPosFrame, SF_SEEK_SET);
			catchPAProblem(Pa_StartStream(_paStream));
		}

		void AudioOStream::pause()
		{
			catchPAProblem(Pa_StopStream(_paStream));
		}

		void AudioOStream::stop()
		{
			_streamPosFrame = 0;
			
			catchPAProblem(Pa_StopStream(_paStream));
		}

		bool AudioOStream::isPlaying()
		{
			return Pa_IsStreamActive(_paStream);
		}

		bool AudioOStream::isPaused()
		{
			return isStopped() && _streamPosFrame != 0;
		}

		bool AudioOStream::isStopped()
		{
			return Pa_IsStreamStopped(_paStream);
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
