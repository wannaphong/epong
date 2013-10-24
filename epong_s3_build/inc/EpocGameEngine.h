/*
 * Copyright (c) 2011 Nokia Corporation.
 */

/*
 ============================================================================
 Name		: EpocGameEngine.h
 Author	  : 
 Description : Declares view class for application.
 ============================================================================
 */

#ifndef __EPOCGAMEENGINE_H__
#define __EPOCGAMEENGINE_H__

// INCLUDES
#include <coecntrl.h>
#include <EGL/egl.h>
#include <gles2/gl2.h>
#include <akndef.h>
#include <aknappui.h> 
#include "GameEngine.h"
#include "GameApp.h"

// volume keys
#include <remconcoreapitargetobserver.h>
#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h>

// profile
#include <centralrepository.h>

// audio
#include <mmf/server/sounddevice.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmffourcc.h>

// goom
#ifdef USE_GOOM_MONITOR
#include <goommonitorsession.h>
#include <goommonitorplugin.hrh>
#endif

// sensor
#include <sensrvchannelfinder.h> 
#include <sensrvchannel.h> 
#include <sensrvorientationsensor.h>
#include <sensrvdatalistener.h>
#include <sensrvaccelerometersensor.h>
#include <sensrvorientationsensor.h>
#include <sensrvchannelinfo.h>

#include "HdmiOutput.h"

// frame count window used to compute average framerate in debug builds
#define FRAMERATE_WINDOW 64

// namespace GF {


#ifdef IAP
#include "../../epong_iap_impl/miniiapsymbianclient.h"
#endif

enum EState {
	ENotRunning=0,
	ERunning=1,
	EDelayedStop=2,
};

// CLASS DECLARATION
class EpocGameEngine : public CCoeControl, public GF::GameEngine, public MCoeControlObserver, public MRemConCoreApiTargetObserver,
	public MDevSoundObserver,
	public MSensrvDataListener,
	public MHdmiOutputCallback
	{
public:
	// New methods

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Create a CEPongAppView object, which will draw itself to aRect.
	 * @param aRect The rectangle this view will be drawn to.
	 * @return a pointer to the created instance of CEPongAppView.
	 */
	static EpocGameEngine* NewL(const TRect& aRect);

	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Create a CEPongAppView object, which will draw itself
	 * to aRect.
	 * @param aRect Rectangle this view will be drawn to.
	 * @return A pointer to the created instance of CEPongAppView.
	 */
	static EpocGameEngine* NewLC(const TRect& aRect);

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 * Perform the second phase construction of a
	 * CEPongAppView object.
	 * @param aRect The rectangle this view will be drawn to.
	 */
	void ConstructL(const TRect& aRect);

private:
	/**
	 * CEPongAppView.
	 * C++ default constructor.
	 */
	EpocGameEngine();

public:
	/**
	 * ~CEPongAppView
	 * Virtual Destructor.
	 */
	virtual ~EpocGameEngine();

	// Functions from base classes

	/**
	 * From CCoeControl, Draw
	 * Draw this CEPongAppView to the screen.
	 * @param aRect the rectangle of this view that needs updating
	 */
	void Draw(const TRect& aRect) const;

	/**
	 * From CoeControl, SizeChanged.
	 * Called by framework when the view size is changed.
	 */
	virtual void SizeChanged();

	/**
	 * From CoeControl, HandlePointerEventL.
	 * Called by framework when a pointer touch event occurs.
	 * Note: although this method is compatible with earlier SDKs, 
	 * it will not be called in SDKs without Touch support.
	 * @param aPointerEvent the information about this event
	 */
	virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);

	virtual void HandleResourceChange(TInt aType);
	virtual void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);
	virtual TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType);

    void MrccatoCommand(TRemConCoreApiOperationId aOperationId, TRemConCoreApiButtonAction aButtonAct);

private:
    virtual void OutputChanged(CHdmiOutput *aHdmi);
    

private:
	// MSensrvDataListener
    void DataReceived(CSensrvChannel& aChannel, TInt aCount, TInt aDataLost);
	void DataError(CSensrvChannel& aChannel, TSensrvErrorSeverity aError);
	void GetDataListenerInterfaceL(TUid aInterfaceUid, TAny*& aInterface);
    
private:
	virtual void InitializeComplete(TInt aError);
   	virtual void BufferToBeFilled(CMMFBuffer* aBuffer);
	virtual void PlayError(TInt aError) {}
	virtual void ToneFinished(TInt /*aError*/) {} 
	virtual void BufferToBeEmptied(CMMFBuffer* /*aBuffer*/) {}
	virtual void RecordError(TInt /*aError*/) {}
	virtual void ConvertError(TInt /*aError*/) {}
	virtual void DeviceMessage (TUid /*aMessageType*/,const TDesC8& /*aMsg*/) {}
	virtual void SendEventToClient(const TMMFEvent& /*aEvent*/) {} 
public:
	virtual int openUrlInBrowser(const char *path);   
	virtual void render();
	
	virtual void exit();
	virtual bool startAudio( int frequency, int channels );
	virtual void stopAudio();

    virtual int loadGLTexture( const char *fileName );
    virtual void releaseTexture(int textureHandle);
    
    virtual void sendEvent(GF::ENGINE_EVENT_TYPE type, int flags);
    virtual bool queryTextInput(const char *prompt, char *target, int targetLength );
	
    void startEngine();
    void stopEngine(TBool aDelayedStop=EFalse);
    void setVolume(int volume);
    virtual const char *adjustPath(const char *inputPath, char *outputPath, int outputPathLength, bool *isOk = 0);
    virtual void setUpdateAndPauseState(bool iUpdatesDisabled, bool aPauseResumeDisabled);
#ifdef IAP
    // gets (and if not existing, creates) "global" MiniIAPClient.
    virtual MiniIAPClientApi *getMiniIAPClient();
#endif
private:
	void initEGL();
	void initEGLSurface();
	void releaseEGLSurface();
	
	static TInt timerCallback( TAny* aInstance);
	static TInt stopEngineCallback( TAny* aInstance);

	virtual void startAudioEngine();
	virtual void stopAudioEngine();
    
#ifdef IAP
	MiniIAPSymbianClient *m_miniIAPClientApi;
#endif
    CRemConInterfaceSelector* iInterfaceSelector;
    CRemConCoreApiTarget*     iCoreTarget;

    RWsSession wsSession;
    CWsScreenDevice *iScreenDevice;
    RWindowGroup *iWindowGroup;
    RWindow *iWindow;

    EGLDisplay  iEglDisplay;
    EGLSurface  iEglSurface;
    EGLContext  iEglContext;

    CPeriodic* iPeriodic;
    CPeriodic* iDelayedStopTimer;
    CAknAppUi* iAppUi;

    EState iRunning;
    TBool iQueryVisible;
    TBool iPauseResumeDisabled;
    TBool iUpdatesDisabled;

    TUint iLastFrameTime;
    TUint iOldFrameTime[FRAMERATE_WINDOW];
    TUint iFrameCount;
    float iAverageDeltaTime;
    
    int iAudioRate;
    int iAudioChannels;

    GF::GameApp *iGame;

    TUint iCurrentVolume;
    
    CMMFDevSound *iDevSound;
    CMMFDescriptorBuffer *iBuffer;
    TUint8 *iMixingBuffer;
    
    CRepository* iProfileRepository;
    CSensrvChannel *iAccelerometerChannel;

#ifdef USE_GOOM_MONITOR
  	RGOomMonitorSession iGoom;
#endif
  	
	CHdmiOutput *iHdmiOutput;
	};

//	}

#endif // __EPOCGAMEENGINE_H__
// End of File
