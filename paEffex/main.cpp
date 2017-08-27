#include "portaudio.h"
#include "pa_linux_alsa.h"
#include "EffectManager.h"
#include <iostream>

//#include "CleanEffect.h"
//#include "DelayEffect.h"
//#include "MsgStruct.h"

using namespace std;

// Using a global object to hold the effects for simplicity.
EffectManager effectManager;
//MsgFromUser UDPmsg;

#define SAMPLE_RATE (44100)
#define FRAMES_PER_BUFFER (64)

int processAudioStreamCallback(const void *input, void *output, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void * userData) {

	effectManager.applyEffect((double*)input, (double*)output, framesPerBuffer);
	return 0;
}
int main(void);
int main(void) {
	
	/* Stream parameters */
	PaStream *stream = NULL;
	PaError err;
	PaStreamParameters inputParameters, outputParameters;
	PaStreamFlags streamFlags = paNoFlag;
	PaHostApiInfo APIInfo;

	/*ALSA specific parameters*/
	PaAlsaStreamInfo * AlsaStreamInfo;
	int *AlsaInputCard, *AlsaOutputCard;
	
	/* PA info */
	const PaDeviceInfo* inputInfo;
	const PaDeviceInfo* outputInfo;
	PaHostApiIndex ApiIndex;
	const PaHostApiInfo * HostApiInfo;
	
	/* Internal variables*/
	string waitForUserInput;


	/* Initialize PortAudio */
	err = Pa_Initialize();
	if (err != paNoError) {
		cout << "PA Initialize FAILED" << endl;
		return 0;
	}
	
	/*PaAlsa_InitializeStreamInfo(AlsaStreamInfo);*/
	AlsaStreamInfo->size = sizeof(PaAlsaStreamInfo);
	cout << endl << "Here" << endl;
	AlsaStreamInfo->hostApiType = paALSA;
	AlsaStreamInfo->version = 1;
	AlsaStreamInfo->deviceString = "Set";


	//cout << "Default input device: " << APIInfo.defaultInputDevice << endl;
	//cout << "Default ouput device: " << APIInfo.defaultOutputDevice << endl;
	//cout << "Device count: " << APIInfo.deviceCount << endl;
	//cout << "Name: " << APIInfo.name << endl;
	//cout << "Type: " << APIInfo.type << endl;
		
	/* Initialize Stream Parameters */
	err = PaAlsa_GetStreamInputCard(stream, AlsaInputCard);
	if (err != paNoError)
	{
		cout << "Could not get ALSA input card" << endl;
		goto error;
	}
	err = PaAlsa_GetStreamOutputCard(stream, AlsaOutputCard);
	if (err != paNoError)
	{
		cout << "Could not get ALSA input card" << endl;
		goto error;
	}

	inputParameters.device  = *AlsaInputCard; /*Pa_GetDefaultInputDevice();*/
	outputParameters.device = *AlsaOutputCard;  /*Pa_GetDefaultOutputDevice();*/
	cout << "input DevID: " << inputParameters.device << endl;
	cout << "output DevID: " << outputParameters.device << endl;
	if (inputParameters.device == paNoDevice) {
		fprintf(stderr, "Error: No default input device.\n");
		goto error;		
	}
	if (outputParameters.device == paNoDevice) {
		fprintf(stderr, "Error: No default input device.\n");
		goto error;
	}
	
	inputInfo = Pa_GetDeviceInfo(inputParameters.device);
	outputInfo = Pa_GetDeviceInfo(outputParameters.device);
	ApiIndex = Pa_GetDefaultHostApi();
	HostApiInfo = Pa_GetHostApiInfo( ApiIndex);

	//printf("   Name: %s\n", inputInfo->name);
	//printf("     LL: %g s\n", inputInfo->defaultLowInputLatency);
	//printf("     HL: %g s\n", inputInfo->defaultHighInputLatency);
	//cout <<"     max in ch: " << inputInfo->maxInputChannels << endl;
	//	
	//printf("   Name: %s\n", outputInfo->name);
	//printf("     LL: %g s\n", outputInfo->defaultLowOutputLatency);
	//printf("     HL: %g s\n", outputInfo->defaultHighOutputLatency);
	//cout <<"     max out ch: " << outputInfo->maxOutputChannels << endl;
	//cout <<"     default samplerate: " << outputInfo->defaultSampleRate << endl;

	//cout << "Default API index: " << ApiIndex << endl;

	cout << "   Default host API info" << endl;
	cout << "      Input device: " << HostApiInfo->defaultInputDevice << endl;
	cout << "      Output device:" << HostApiInfo->defaultOutputDevice << endl;
	cout << "      Device count: " << HostApiInfo->deviceCount << endl;
	cout << "      Name: " << HostApiInfo->name << endl;
	cout << "      Struct ver: " << HostApiInfo->structVersion << endl;
	cout << "      Type: " << HostApiInfo->type << endl;
	
	inputParameters.channelCount = inputInfo->maxInputChannels;
	outputParameters.channelCount = outputInfo->maxOutputChannels;
	inputParameters.sampleFormat = paFloat32; /*+paNonInterleaved*/
	outputParameters.sampleFormat = paFloat32; /*+paNonInterleaved*/
	inputParameters.suggestedLatency = inputInfo->defaultLowInputLatency;
	outputParameters.suggestedLatency = outputInfo->defaultLowOutputLatency;
	inputParameters.hostApiSpecificStreamInfo  = (PaAlsaStreamInfo*)AlsaStreamInfo;
	outputParameters.hostApiSpecificStreamInfo = (PaAlsaStreamInfo*)AlsaStreamInfo;
					
	/* Open an audio I/O stream. */
	err = Pa_OpenStream(&stream, &inputParameters, &outputParameters, SAMPLE_RATE, FRAMES_PER_BUFFER,
		streamFlags, processAudioStreamCallback, NULL);
	if (err != paNoError); cout << "OPEN STREAM FAILED " <<  endl; goto error;

	/* Start an audio I/O stream. */
	err = Pa_StartStream(stream);
	if (err != paNoError); goto error;

	/*  -- Block thread until user hits enter -- */
	cout << "Play ... " << endl;
	
	while(1)
	{
		cin >> waitForUserInput;
		break;
	}

	/* Stop an audio I/O stream. */
	err = Pa_StopStream(stream);
	if (err != paNoError); goto error;

	/* Close an audio I/O stream. */
	err = Pa_CloseStream(stream);
	if (err != paNoError); goto error;

	/* Terminate PortAudio */
	err = Pa_Terminate();
	if (err != paNoError); goto error;
		
	cout << "Stream terminated" << endl;
	return err;

error:
	Pa_Terminate();
	fprintf(stderr, "An error occured while using the portaudio stream\n");
	fprintf(stderr, "Error number: %d\n", err);
	fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
	return err;
	
}