#pragma once
#include "raylib.h"

// On Android we expose the platform/graphics specific OpenXR types, since the
// headers involved (jni.h / EGL) do not collide with anything raylib uses.
// On Win32 we must NOT define XR_USE_PLATFORM_WIN32 here, because
// openxr_platform.h then needs <windows.h>, which collides with raylib.h.
// The Win32 specific parts are isolated in platform/rlOpenXRWin32Wrapper.*
#if defined(__ANDROID__)
#define XR_USE_PLATFORM_ANDROID
#define XR_USE_GRAPHICS_API_OPENGL_ES
#define XR_USE_TIMESPEC
#include <jni.h>
#include "EGL/egl.h"
#endif

#include "openxr/openxr.h"

#if defined(__ANDROID__)
#include "openxr/openxr_platform.h"
#endif

// TODO: DLL export stuff

//----------------------------------------------------------------------------------
// Type Definitions
//----------------------------------------------------------------------------------

typedef enum { RLOPENXR_EYE_LEFT = 0, RLOPENXR_EYE_RIGHT = 1, RLOPENXR_EYE_BOTH = 2 } RLOpenXREye;

typedef enum { RLOPENXR_HAND_LEFT, RLOPENXR_HAND_RIGHT, RLOPENXR_HAND_COUNT } RLOpenXRHandEnum;

typedef struct
{
	XrInstance instance; // the instance handle can be thought of as the basic connection to the OpenXR runtime
	XrSystemId system_id; // the system represents an (opaque) set of XR devices in use, managed by the runtime
	XrSession session; // the session deals with the renderloop submitting frames to the runtime

	XrSessionState session_state;

	XrSpace play_space;
	XrSpace view_space;

	// Constants
	XrViewConfigurationType view_type /*= XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO*/;
	XrFormFactor form_factor /*= XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY*/;
	XrReferenceSpaceType play_space_type /*= XR_REFERENCE_SPACE_TYPE_STAGE*/;
} RLOpenXRData;

typedef struct
{
	// OpenXR Ouput Data
	bool valid;
	Vector3 position;
	Quaternion orientation;

	// Input Config
	RLOpenXRHandEnum handedness;

	XrAction hand_pose_action;
	XrPath hand_pose_subpath;
	XrSpace hand_pose_space;
} RLHand;


//----------------------------------------------------------------------------------
// Function Definitions
//----------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

// Setup
bool rlOpenXRSetup();
void rlOpenXRShutdown();

// Update
void rlOpenXRUpdate();

void rlOpenXRUpdateCamera(Camera3D* camera);
void rlOpenXRUpdateCameraTransform(Transform* transform);

// Drawing
bool rlOpenXRBegin();
bool rlOpenXRBeginMockHMD();
void rlOpenXREnd();

void rlOpenXRBlitToWindow(RLOpenXREye eye, bool keep_aspect_ratio);

// State
const RLOpenXRData* rlOpenXRData();

// Input / Hands
void rlOpenXRUpdateHands(RLHand* left, RLHand* right);

void rlOpenXRSyncSingleActionSet(XrActionSet action_set); // Utility function for xrSyncAction with a single action set.

// Misc
XrTime rlOpenXRGetTime();

XrInstance rlGetXRInstance();

XrSession rlGetXRSession();

XrSpace rlGetPlaySpace();

#ifdef __cplusplus
}
#endif
