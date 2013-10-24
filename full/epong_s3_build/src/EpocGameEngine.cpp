/*
 * Copyright (c) 2011 Nokia Corporation.
 */

/*
 ============================================================================
 Name		: EpocGameEngine.cpp
 Author	  : 
 Description : Application view implementation
 ============================================================================
 */

// INCLUDE FILES
#include <coemain.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/fcntl.h>
#include "EpocGameEngine.h"

#include "stb_image.h"

// querytextinput
#include <UTF.H>
#include <aknquerydialog.h>
#include <aknnotewrappers.h>
#include <aknmessagequerydialog.h>

// haptic
#include <touchfeedback.h>
#include <e32debug.h>

// opening browser with html
#include <DocumentHandler.h>

// memory profile helper
#include "memprofile.h"

#ifdef IAP
#include "../../epong_iap_impl/miniiapsymbianclient.h"
#endif

#ifdef USE_GOOM_MONITOR
// engine requests 18 MB of graphics memory from goom
const TInt KRequestGraphicsMemoryAmount = 18*1024*1024;
#endif

// number of volume steps, 0=mute, MAX_VOLUME_STEPS=100%
const int MAX_VOLUME_STEPS = 8;

// timer interval in microseconds
const TInt KTimerInterval = 10000;

// stop delay
const TInt KStopDelay = 10000000; // 10 seconds

// buffer size in bytes
const TInt KBufferSize = 1024 * 8;
// N8 with Symbian Belle seems to always require this amount of buffer.

// Uid for Profile repository (Not included in the standard headers).
const TUid KCRUidProfileEngine =
	{
	0x101F8798
	};

// Key for warning and game tones enabled in active profile
const TUint32 KProEngActiveWarningTones = 0x7E000020;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// EpocGameEngine::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EpocGameEngine* EpocGameEngine::NewL(const TRect& aRect)
	{
	EpocGameEngine* self = EpocGameEngine::NewLC(aRect);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// EpocGameEngine::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EpocGameEngine* EpocGameEngine::NewLC(const TRect& aRect)
	{
	EpocGameEngine* self = new (ELeave) EpocGameEngine;
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	return self;
	}

TInt EpocGameEngine::timerCallback(TAny* aInstance)
	{
	EpocGameEngine* instance = (EpocGameEngine*) aInstance;
	instance->render();
	return 0;
	}

// -----------------------------------------------------------------------------
// EpocGameEngine::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void EpocGameEngine::ConstructL(const TRect& aRect)
	{
	RDebug::Printf("ConstructL EpocGameEngine");
#ifdef IAP
	m_miniIAPClientApi = 0;
#endif

	// Create a window for this application view
	CreateWindowL();
	// Set the windows size
	SetRect(aRect);

	SetExtentToWholeScreen();

	EnableDragEvents();

#ifndef DISABLE_MULTITOUCH
	Window().EnableAdvancedPointers();
#endif

	// Activate the window, which makes it ready to be drawn
	ActivateL();

#ifdef USE_GOOM_MONITOR
 	User::LeaveIfError(iGoom.Connect());
#endif
 	
	iHdmiOutput = CHdmiOutput::NewL();

	iMixingBuffer = new TUint8[KBufferSize];
	iBuffer = CMMFDescriptorBuffer::NewL(KBufferSize);
	iBuffer->SetStatus(EAvailable);

	// profile
	iProfileRepository = CRepository::NewL(KCRUidProfileEngine);

	// volume keys 
	iInterfaceSelector = CRemConInterfaceSelector::NewL();
	iCoreTarget = CRemConCoreApiTarget::NewL(*iInterfaceSelector, *this);
	iInterfaceSelector->OpenTargetL();

	// symbian has no concept of global volume, use 75% 
	iCurrentVolume = MAX_VOLUME_STEPS * 3 / 4;

	initEGL();
	initEGLSurface();

	iHdmiOutput->iCallback = this;

	iPeriodic = CPeriodic::NewL(CActive::EPriorityIdle);
	iDelayedStopTimer = CPeriodic::NewL(CActive::EPriorityIdle);

	iGame = GF::createGameApplication(this);
	
    iPauseResumeDisabled = EFalse;
    iUpdatesDisabled = EFalse;

	//Sensors
	CSensrvChannelFinder* finder = CSensrvChannelFinder::NewLC();
	RSensrvChannelInfoList channelList;
	CleanupClosePushL(channelList);

	
	TSensrvChannelInfo info;
	info.iChannelType = KSensrvChannelTypeIdAccelerometerXYZAxisData;
	finder->FindChannelsL(channelList, info);

	if (channelList.Count() > 0)
		{
		iAccelerometerChannel = CSensrvChannel::NewL(channelList[0]);
		iAccelerometerChannel->OpenChannelL();
		iAccelerometerChannel->StartDataListeningL(this, 1, 1, 0);
		}

	channelList.Close();
	CleanupStack::Pop(&channelList);
	CleanupStack::PopAndDestroy(finder);
	}

// -----------------------------------------------------------------------------
// EpocGameEngine::EpocGameEngine()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
EpocGameEngine::EpocGameEngine()
	{
	// No implementation required

	}

// -----------------------------------------------------------------------------
// EpocGameEngine::~EpocGameEngine()
// Destructor.
// -----------------------------------------------------------------------------
//
EpocGameEngine::~EpocGameEngine()
	{
	stopEngine();

	if (iAccelerometerChannel)
		{
		iAccelerometerChannel->StopDataListening();
		iAccelerometerChannel->CloseChannel();
		delete iAccelerometerChannel;
		iAccelerometerChannel = NULL;
		}

	iPeriodic->Cancel();
	delete iPeriodic;
	iPeriodic = NULL;

	iDelayedStopTimer->Cancel();
	delete iDelayedStopTimer;
	iDelayedStopTimer = NULL;

	delete[] iMixingBuffer;
	iMixingBuffer = NULL;

	delete iBuffer;
	iBuffer = NULL;

	delete iGame;
	iGame = NULL;

	delete iProfileRepository;
	iProfileRepository = NULL;

	delete iInterfaceSelector;
	iInterfaceSelector = NULL;
	iCoreTarget = NULL; // owned by interfaceselector

	if (iEglDisplay)
		{
		releaseEGLSurface();

		}

	delete iHdmiOutput;
	iHdmiOutput = NULL;
#ifdef IAP
	if (m_miniIAPClientApi) {
        delete m_miniIAPClientApi;
    }
    m_miniIAPClientApi = 0;
#endif

#ifdef USE_GOOM_MONITOR
 	iGoom.Close();
#endif

	}

// -----------------------------------------------------------------------------
// EpocGameEngine::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void EpocGameEngine::Draw(const TRect& /*aRect*/) const
	{
	if (iRunning == ERunning)
		{
		// remove splash screen if using internal screen for playing
		if(!iHdmiOutput->IsHdmiConnected())
			{
			ResetGc();
			}

		// framerate counter  
#ifdef _DEBUG
		CWindowGc& gc = SystemGc();

		gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
		gc.SetBrushColor(KRgbBlack);

		const CFont* f = CEikonEnv::Static()->TitleFont();
		gc.UseFont(f);
		gc.SetPenColor(KRgbWhite);

		TBuf<64> fps;
		_LIT(KFpsString, "fps %2.2f");
		if (iAverageDeltaTime > 0)
			{
			fps.Format(KFpsString, 1000.0f / iAverageDeltaTime);
			}
		gc.DrawText(fps, TPoint(10, 30));
		gc.DiscardFont();
#endif

		}
	else if (iRunning == ENotRunning)
		{
		// draw splash screen
		CWindowGc& gc = SystemGc();

		int w, h, channels;
		TUint32 *data = (TUint32 *) stbi_load("default.png", &w, &h, &channels,
				4);

		if (data)
			{
			CFbsBitmap *bmp = new (ELeave) CFbsBitmap();
			bmp->Create(TSize(w, h), EColor16MU);

			bmp->BeginDataAccess();

			// shuffle pixels to symbian bitmap 
			TUint32 *buf = bmp->DataAddress();
			for (int o = 0; o < w * h; o++)
				{
				unsigned int p = data[o];
				// swap red/blue
				p = (p & 0xff00ff00) | ((p >> 16) & 0xff) | (p & 0xff) << 16;
				buf[o] = p;
				}
			bmp->EndDataAccess();
			stbi_image_free(data);

			int x = (Rect().Width() - w) / 2;
			int y = (Rect().Height() - h) / 2;
			gc.BitBlt(TPoint(x, y), bmp);

			delete bmp;
			}
		}

	}

// -----------------------------------------------------------------------------
// EpocGameEngine::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void EpocGameEngine::SizeChanged()
	{
	if (iGame)
		{
		iGame->resize(Rect().Width(), Rect().Height());
		}
	}

// -----------------------------------------------------------------------------
// EpocGameEngine::HandlePointerEventL()
// Called by framework to handle pointer touch events.
// Note: although this method is compatible with earlier SDKs, 
// it will not be called in SDKs without Touch support.
// -----------------------------------------------------------------------------
//
void EpocGameEngine::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{

	int n = 0;
	int x = aPointerEvent.iPosition.iX;
	int y = aPointerEvent.iPosition.iY;

#ifndef DISABLE_MULTITOUCH
	// TODO
	const TAdvancedPointerEvent* advancedEvent =
			aPointerEvent.AdvancedPointerEvent();
	if (advancedEvent)
		{
		x = advancedEvent->Position3D().iX;
		y = advancedEvent->Position3D().iY;
		n = advancedEvent->PointerNumber();
		}
#endif	

	switch (aPointerEvent.iType)
		{
		case TPointerEvent::EButton1Down:
			iGame->mouseEvent(GF::MOUSE_DOWN, x, y, n);

			break;
		case TPointerEvent::EButton1Up:
			iGame->mouseEvent(GF::MOUSE_UP, x, y, n);
			break;
		case TPointerEvent::EDrag:
			iGame->mouseEvent(GF::MOUSE_MOVE, x, y, n);
			break;
		default:
			break;
		}
	// Call base class HandlePointerEventL()
	CCoeControl::HandlePointerEventL(aPointerEvent);
	}

/** Called by framework on screen orientation change.  Not used unless you enable dynamic orientation. */
void EpocGameEngine::HandleResourceChange(TInt aType)
	{
	if (aType == KEikDynamicLayoutVariantSwitch)
		{
		SetExtentToWholeScreen();
		}
	}

TKeyResponse EpocGameEngine::OfferKeyEventL(const TKeyEvent & aKeyEvent,
		TEventCode aType)
	{

	int button = 0;
	switch (aKeyEvent.iScanCode)
		{
		case EStdKeyLeftArrow:
			button = GF::LEFT;
			break;
		case EStdKeyRightArrow:
			button = GF::RIGHT;
			break;
		case EStdKeyUpArrow:
			button = GF::UP;
			break;
		case EStdKeyDownArrow:
			button = GF::DOWN;
			break;
		case EStdKeyDevice0:
		case EStdKeyDevice1:
		case EStdKeyDevice2:
		case EStdKeyDevice3:
			button = GF::FIRE;
			break;
		default:
			button = 0;
		}

	switch (aType)
		{
		case EEventKeyDown:
			iGame->keyEvent(GF::KEY_DOWN, button);
			return EKeyWasConsumed;
		case EEventKeyUp:
			iGame->keyEvent(GF::KEY_UP, button);
			return EKeyWasConsumed;
		default:
			break;
		}
	return EKeyWasNotConsumed;
	}

void EpocGameEngine::HandleControlEventL(CCoeControl *aControl,
		TCoeEvent aEventType)
	{
	// TODO
	}

void EpocGameEngine::initEGL()
	{
	}

void EpocGameEngine::OutputChanged(CHdmiOutput *aHdmi)
	{

	stopEngine(EFalse);
	if (iEglDisplay)
		{
		releaseEGLSurface();
		}
	initEGLSurface();
	startEngine();

	}

void EpocGameEngine::releaseEGLSurface()
	{
	eglMakeCurrent(iEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroySurface(iEglDisplay, iEglSurface);
	iEglSurface = 0;
	eglDestroyContext(iEglDisplay, iEglContext);
	iEglContext = 0;

	eglTerminate(iEglDisplay);
	eglReleaseThread();
	iEglDisplay = 0;
	}

void EpocGameEngine::initEGLSurface()
	{
	// Get the display for drawing graphics
	iEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (iEglDisplay == NULL)
		{
		_LIT( KGetDisplayFailed, "eglGetDisplay failed" );
		User::Panic(KGetDisplayFailed, 0);
		}

	// Initialize display
	if (eglInitialize(iEglDisplay, NULL, NULL) == EGL_FALSE)
		{
		_LIT( KInitializeFailed, "eglInitialize failed" );
		User::Panic(KInitializeFailed, 0);
		}

	RWindow *outputWindow = &Window();
	if (iHdmiOutput->IsHdmiConnected())
		{
		outputWindow = iHdmiOutput->HdmiWindow();
		}

	// EGL configuration
	EGLConfig config;
	EGLint numOfConfigs = 0;

	// Define properties for the wanted EGLSurface
	TDisplayMode displayMode = outputWindow->DisplayMode();
	TInt bufferSize =
			TDisplayModeUtils::NumDisplayModeBitsPerPixel(displayMode);

	// Define properties for the wanted EGLSurface
	const EGLint attribList[] =
		{
				EGL_BUFFER_SIZE,
				bufferSize,
				EGL_RENDERABLE_TYPE,
				EGL_OPENGL_ES2_BIT,
				EGL_NONE
		};

	// Choose an EGLConfig that best matches to the properties in attribList    
	if (eglChooseConfig(iEglDisplay, attribList, &config, 1, &numOfConfigs)
			== EGL_FALSE)
		{
		_LIT( KChooseConfigFailed, "eglChooseConfig failed" );
		User::Panic(KChooseConfigFailed, 0);
		}

	// Check if no configurations were found
	if (numOfConfigs == 0)
		{
		_LIT( KNoConfig, "Can't find the requested config." );
		User::Panic(KNoConfig, 0);
		}

	// Create a window where the graphics are blitted
	iEglSurface = eglCreateWindowSurface(iEglDisplay, config, outputWindow,
			NULL);

	if (iEglSurface == NULL)
		{
		_LIT( KCreateWindowSurfaceFailed, "eglCreateWindowSurface failed" );
		User::Panic(KCreateWindowSurfaceFailed, 0);
		}

	// Create a rendering context
	const EGLint contextAttrs[] =
		{
		EGL_CONTEXT_CLIENT_VERSION, 2, // Request an OpenGL ES 2.0 context
				EGL_NONE
		};

	iEglContext = eglCreateContext(iEglDisplay, config, EGL_NO_CONTEXT,
			contextAttrs);
	if (iEglContext == NULL)
		{
		_LIT( KCreateContextFailed, "eglCreateContext failed" );
		User::Panic(KCreateContextFailed, 0);
		}

	// Make the context current. Binds context to the current rendering thread
	// and surface.
	if (eglMakeCurrent(iEglDisplay, iEglSurface, iEglSurface, iEglContext)
			== EGL_FALSE)
		{
		_LIT( KMakeCurrentFailed, "eglMakeCurrent failed" );
		User::Panic(KMakeCurrentFailed, 0);
		}
	}

TInt EpocGameEngine::stopEngineCallback(TAny* aInstance)
	{
	EpocGameEngine* instance = (EpocGameEngine*) aInstance;
	instance->stopEngine(EFalse);
	return 0;
	}

void EpocGameEngine::stopEngine(TBool aDelayed)
	{
	RDebug::Printf("EpocGameEngine::stopEngine, %s, %d",
			(aDelayed?"delayed":"direct"),
			iPauseResumeDisabled);
	if (iPauseResumeDisabled) {
		return;
	}
	iPeriodic->Cancel();
	iDelayedStopTimer->Cancel();
	stopAudioEngine();

	if (aDelayed)
		{
		iRunning = EDelayedStop;
		iDelayedStopTimer->Start(KStopDelay, KStopDelay, TCallBack(
				stopEngineCallback, this));
		}
	else
		{
		iRunning = ENotRunning;
		DrawNow(); // issue repaint to show splash screen
		iGame->notifyEvent(GF::NE_PAUSED, 0);
		RDebug::Printf("GameApp: release!");
		iGame->release();
		}
	}

void EpocGameEngine::startEngine()
	{
	RDebug::Printf("EpocGameEngine::startEngine, %d", iPauseResumeDisabled);
	if (iPauseResumeDisabled) {
		return;
	}
	iDelayedStopTimer->Cancel();

	if (!iPauseResumeDisabled && iRunning != ERunning)
		{
		RDebug::Printf("EpocGameEngine::startEngine, Really!");
		DrawNow(); // issue repaint to show splash screen
#ifdef USE_GOOM_MONITOR
		iGoom.RequestFreeMemory(KRequestGraphicsMemoryAmount);
#endif		
		startAudioEngine();
		iPeriodic->Start(0, KTimerInterval, TCallBack(timerCallback, this));
		}
	}

void EpocGameEngine::stopAudio()
	{
	iAudioRate = 0;
	iAudioChannels = 0;
	stopAudioEngine();
	}

void EpocGameEngine::stopAudioEngine()
	{
	if (iDevSound)
		{
		iDevSound->Stop();
		delete iDevSound;
		iDevSound = NULL;
		}
	}

bool EpocGameEngine::startAudio(int frequency, int channels)
	{
	iAudioRate = frequency;
	iAudioChannels = channels;
	startAudioEngine();
	return true;
	}

static TMMFSampleRate convertRateToTMMFSampleRate(int rate)
	{
	switch (rate)
		{
		case 8000:
			return EMMFSampleRate8000Hz;
		case 11025:
			return EMMFSampleRate11025Hz;
		case 16000:
			return EMMFSampleRate16000Hz;
		case 22050:
			return EMMFSampleRate22050Hz;
		case 32000:
			return EMMFSampleRate32000Hz;
		case 44100:
		default:
			return EMMFSampleRate44100Hz;
		case 48000:
			return EMMFSampleRate48000Hz;
		}
	}

void EpocGameEngine::startAudioEngine()
	{
	// if warning tones are disabled, mute audio
	TInt warningTonesEnabled = 1;
	if (iProfileRepository->Get(KProEngActiveWarningTones, warningTonesEnabled)
			== KErrNone && warningTonesEnabled == 0)
		{
		iCurrentVolume = 0;
		}

	// init devsound if audio is wanted
	if (!iDevSound && iAudioRate != 0)
		{
		TRAPD(err,
					{
					iBuffer->SetStatus(EAvailable);

					iDevSound = CMMFDevSound::NewL();
					iDevSound->InitializeL(*this, KMMFFourCCCodePCM16, EMMFStatePlaying);

					});
		}
	}

void EpocGameEngine::InitializeComplete(TInt aError)
	{
	if (aError == KErrNone)
		{
		TRAPD(err,
					{
					TMMFPrioritySettings priority;
					priority.iPref = EMdaPriorityPreferenceQuality;
					priority.iPriority = EMdaPriorityPreferenceNone;
					iDevSound->SetPrioritySettings(priority);

					TMMFCapabilities settings;
					settings = iDevSound->Config();
					settings.iChannels = iAudioChannels < 2 ? EMMFMono : EMMFStereo;
					settings.iRate = convertRateToTMMFSampleRate(iAudioRate);
					settings.iBufferSize = KBufferSize;
					iDevSound->SetConfigL(settings);

					setVolume(iCurrentVolume);
					iDevSound->PlayInitL();
					});
		if (err != KErrNone)
			{
			// if config or playinit failed with leave, stop audio engine
			stopAudio();
			}
		}
	}

void EpocGameEngine::exit()
	{
	RDebug::Printf("exit");
	// stop engine
	stopEngine();
	// send exit command to framework
	CEikonEnv::Static()->EikAppUi()->HandleCommandL(EEikCmdExit);
	}

int EpocGameEngine::loadGLTexture(const char *fileName)
	{
	GLuint texName = 0;

	int width, height, channels;

	unsigned char *data = stbi_load(fileName, &width, &height, &channels, 0);

	if (data)
		{
		glGenTextures(1, &texName);
		glBindTexture(GL_TEXTURE_2D, texName);

		switch (channels)
			{
			case 1:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0,
						GL_ALPHA, GL_UNSIGNED_BYTE, data);
				break;
			case 2:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, width,
						height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
				break;
			case 3:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
						GL_RGB, GL_UNSIGNED_BYTE, data);
				break;
			case 4:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
						GL_RGBA, GL_UNSIGNED_BYTE, data);
				break;
			}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
		}

	return texName;
	}


void EpocGameEngine::releaseTexture(int textureHandle) {
	if(textureHandle) glDeleteTextures(1, &textureHandle);
}


void EpocGameEngine::sendEvent(GF::ENGINE_EVENT_TYPE type, int flags)
	{
	if (type == GF::HAPTIC_FEEDBACK)
		{
		MTouchFeedback *feedback = MTouchFeedback::Instance();
		feedback->SetFeedbackEnabledForThisApp(ETrue);
		feedback->InstantFeedback(ETouchFeedbackBasic);
		}
	}

void EpocGameEngine::render()
	{
	if (iRunning == ENotRunning)
		{
		// if previously was not running, load resources
		iRunning = ERunning;
		RDebug::Printf("GameApp: prepare!");
		iGame->prepare();
		iGame->resize(Rect().Width(), Rect().Height());
		DrawNow(); // repaint to remove splash
		return;
		}

	TUint delta = 0;

	TUint currentFrameTime = User::NTickCount();
#ifdef __WINS__
	// emulator default resolution is 200Hz, hardware is 1000Hz
	// (in GLES2, this doesnt really matter since emulator does not support GLES2 anyway)
	currentFrameTime *= 5;
#endif
	if (iLastFrameTime)
		{
		delta = currentFrameTime - iLastFrameTime;
		}
	iLastFrameTime = currentFrameTime;

	if (delta > 1000)
		{
		// delta can get too big when resuming from background or suspended by breakpoint.
		delta = 1000;
		}

	if (iRunning == EDelayedStop)
		{
		iRunning = ERunning;
		delta = 0;
		}

	// TODO we could stop timer when query is visible
	if (!iUpdatesDisabled)
		{
		iGame->update(delta / 1000.0f);

		if (iRunning != ERunning)
			{ 
			// engine is not yet running, maybe because queryTextInput was blocking the game
			// and focus was lost because of another application.
			// timer will handle loading and continuing the game
			return;
			}

		glClearColor(0,0,0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		iGame->render();
		}

#ifdef _DEBUG

	// in debug buids, output possible glGetError states to symbian debug console
	GLenum error = glGetError();
	if (error)
		{
		//
		RDebug::Printf("Frame %d", iFrameCount);
		RDebug::Printf("glGetError %x", error);
		}

	static int lastUsedPrivate = 0;

	EGLDisplay aDisp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	int aTotalGRAM = 0;
	int aUsedGRAM = 0;
	int aUsedPrivate = 0;
	int aUsedShared = 0;
	int free = 0;

	GetProfilingExtensionData(aDisp, aTotalGRAM, aUsedGRAM, aUsedPrivate,
			aUsedShared);
	free = aTotalGRAM - aUsedGRAM;

	if (lastUsedPrivate != aUsedPrivate)
		{
		RDebug::Printf("Frame %d", iFrameCount);
		RDebug::Printf(
				"MEMORY_STATS_TOTAL: %d USED: %d USEDPRIVATE: %d USEDSHARED: %d FREE: %d DELTA:%d\n",
				aTotalGRAM, aUsedGRAM, aUsedPrivate, aUsedShared, free,
				lastUsedPrivate - aUsedPrivate);
		}
	lastUsedPrivate = aUsedPrivate;

#endif

	if (!iUpdatesDisabled)
		{
		eglSwapBuffers(iEglDisplay, iEglSurface);

		iOldFrameTime[iFrameCount % FRAMERATE_WINDOW] = iLastFrameTime;
		iFrameCount++;

		if ((iFrameCount & 63) == 0)
			{
			// keep backlight on.  
			// disabled. conflicts with TPO_BAT Sleep Function test which requires screensaver to activate 
			// User::ResetInactivityTime();

#ifdef _DEBUG
			// update fps counter
			if (iFrameCount > FRAMERATE_WINDOW)
				{
				iAverageDeltaTime = (iLastFrameTime - iOldFrameTime[iFrameCount
						% FRAMERATE_WINDOW]) / (float) FRAMERATE_WINDOW;
				DrawDeferred();
				}
#endif
			}
		}

	}

/**
 * Callback from Rem Con API.  If volume up or down is clicked, adjust internal volume step accordingly
 * and update engine volume.
 */
void EpocGameEngine::MrccatoCommand(TRemConCoreApiOperationId aOperationId,
		TRemConCoreApiButtonAction aButtonAct)
	{
	if (aOperationId == ERemConCoreApiVolumeUp && aButtonAct
			== ERemConCoreApiButtonClick)
		{
		// vol up
		setVolume(iCurrentVolume + 1);
		}
	else if (aOperationId == ERemConCoreApiVolumeDown && aButtonAct
			== ERemConCoreApiButtonClick)
		{
		// vol down
		setVolume(iCurrentVolume - 1);
		}
	}

/**
 * Sets engine volume.  Symbian audio stream volume is adjusted and game is notified with new volume.
 * @param volume int in range 0..MAX_VOLUME_STEPS
 */
void EpocGameEngine::setVolume(int volume)
	{
	if (volume < 0)
		volume = 0;
	else if (volume > MAX_VOLUME_STEPS)
		volume = MAX_VOLUME_STEPS;

	iCurrentVolume = volume;

	if (iDevSound)
		iDevSound->SetVolume(iCurrentVolume * iDevSound->MaxVolume()
				/ MAX_VOLUME_STEPS);

	iGame->notifyEvent(GF::NE_VOLUME_CHANGED, iCurrentVolume * 100
			/ MAX_VOLUME_STEPS);
	}

/**
 * Launch url in browser.  We create a simple html, which redirects the browser to correct url.
 * Another way to launch browser is by using UID to launch browser application, but it gets 
 * nasty (requires SwEvent capability) when browser is already open.
 */
int EpocGameEngine::openUrlInBrowser(const char *path8)
	{
	_LIT(KDocumentPath, "d:\\system\\temp\\launch.html");
	_LIT8(KTemplate, "<html><meta http-equiv=\"refresh\" content=\"0;url=");
	_LIT8(KTemplate2, "\"><body>Loading...</body></html>");

	// Write simple HTML meta refresh loader page to hardcoded location
	TInt err = 0;
	RFs session;
	err = session.Connect(); // never fails
	RFile f;
	err = f.Replace(session, KDocumentPath, EFileStream | EFileWrite);
	if (err == KErrNone)
		{
		f.Write(KTemplate);
		f.Write(TPtrC8((const TUint8*) path8, strlen(path8)));
		f.Write(KTemplate2);
		f.Close();
		}
	session.Close();
	if (err)
		return err;

	TRAP(err,
				{
				CDocumentHandler* docHandler = CDocumentHandler::NewL(CEikonEnv::Static()->Process());
				CleanupStack::PushL(docHandler);
				TDataType emptyDataType = TDataType();
				// Launch HTML document to open browser
				docHandler->OpenFileL(KDocumentPath, emptyDataType);
				CleanupStack::PopAndDestroy(docHandler);
				}
	);

	return err;
	}

void EpocGameEngine::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	CMMFDataBuffer* buffer = static_cast<CMMFDataBuffer*> (aBuffer);
	if (iGame)
		{
		// TODO: Use buffer->RequestSize() !
		// See http://www.developer.nokia.com/Community/Wiki/KIS001595_-_Increased_CMMFDevSound_playback_buffer_size_in_Nokia_N8
		iGame->readAudioStream((short *)iMixingBuffer, KBufferSize / 2);
		buffer->Data().Zero();
		buffer->Data().Append(iMixingBuffer, KBufferSize);
		
		iDevSound->PlayData();
		}

	}

// Sensor callbacks

void EpocGameEngine::DataReceived(CSensrvChannel& aChannel, TInt aCount,
		TInt aDataLost)
	{
	TSensrvChannelInfo info = aChannel.GetChannelInfo();

	if (info.iChannelType == KSensrvChannelTypeIdAccelerometerXYZAxisData)
		{
		TSensrvAccelerometerAxisData data;
		TPckg<TSensrvAccelerometerAxisData> pkg(data);

		TInt err = aChannel.GetData(pkg);
		if (err == KErrNone)
			{
			iGame->sensorEvent(GF::ACCELEROMETER, data.iAxisX, data.iAxisY,
					data.iAxisZ);
			}
		}
	// TODO add more sensors

	}

void EpocGameEngine::DataError(CSensrvChannel& aChannel,
		TSensrvErrorSeverity aError)
	{
	// sensor error, ignored
	}

void EpocGameEngine::GetDataListenerInterfaceL(TUid aInterfaceUid,
		TAny*& aInterface)
	{
	// sensor future extensions, no implementation
	}

bool EpocGameEngine::queryTextInput(const char *prompt, char *target,
		int targetLength)
	{
	memset(target, 0, targetLength);
	TRAPD(err,
				{

				HBufC16 *buf = HBufC16::NewLC(targetLength);

				TPtr16 msg(buf->Des());
				CAknTextQueryDialog* dlg = CAknTextQueryDialog::NewL(msg);
				CleanupStack::PushL(dlg);
				if(prompt)
					{
					TPtrC8 promptDes((const TUint8*)prompt);
					HBufC *promptBuf = CnvUtfConverter::ConvertToUnicodeFromUtf8L(promptDes);
					CleanupStack::PushL(promptBuf);
					dlg->SetPromptL(promptBuf->Des());
					CleanupStack::PopAndDestroy(promptBuf);
					}

				setUpdateAndPauseState(ETrue, EFalse);
				TInt result = dlg->ExecuteLD(R_AVKON_DIALOG_QUERY_VALUE_TEXT);
				CleanupStack::Pop(dlg);

				setUpdateAndPauseState(EFalse, EFalse);

				// just in case application was suspended.
				startEngine();

				TPtr8 targetDes((TUint8*)target, targetLength-1);
				CnvUtfConverter::ConvertFromUnicodeToUtf8(targetDes, msg);

				CleanupStack::PopAndDestroy(buf);

				return result;
				});

	setUpdateAndPauseState(EFalse, EFalse);
	startEngine();
	return err != KErrNone;
	}

const char* EpocGameEngine::adjustPath(const char *inputPath, char *outputPath, int outputPathLength, bool *isOk) {
    if (strlen(inputPath) > (unsigned int) outputPathLength) {
        if (isOk) (*isOk) = false;
        strcpy(outputPath, "");
    }
    else {
    	if (strncmp("configdata:", inputPath, 11)  == 0) {
    		strcpy(outputPath, inputPath + 11);
    	}
    	else {
            strcpy(outputPath, inputPath);
    	}
        if (isOk) (*isOk) = true;
    }
    return outputPath;
}

void EpocGameEngine::setUpdateAndPauseState(bool aUpdatesDisabled, bool aPauseResumeDisabled) {
	RDebug::Printf("EpocGameEngine::setPauseResumeDisabled: %d, %d", aUpdatesDisabled, aPauseResumeDisabled);
	if (aUpdatesDisabled) {
		iUpdatesDisabled = ETrue;
	}
	else {
		iUpdatesDisabled = EFalse; 
	}

	if (aPauseResumeDisabled) {
		iPauseResumeDisabled = ETrue;
	}
	else {
		iPauseResumeDisabled = EFalse; 
	}
}

#ifdef IAP
// gets (and if not existing, creates) "global" MiniIAPClient.
MiniIAPClientApi *EpocGameEngine::getMiniIAPClient() {
	if (!m_miniIAPClientApi) {
		m_miniIAPClientApi = new MiniIAPSymbianClient();
	}
	return m_miniIAPClientApi;
}

#endif // IAP


// End of File
