//
// Created by netlabs on 7/27/19.
//

#include <openvr.h>

#include "Corrade/Containers/Array.h"

#include "Magnum/PixelFormat.h"


#include "VRCamera.h"
#include "Integration.h"

using namespace artekmed;


//#undef near
//#undef far

bool VRCamera::initialize(vr::IVRSystem * vr_system)
{
    hmd = vr_system;

    trackedCamera = vr::VRTrackedCamera();
    if (!trackedCamera)
    {
        Magnum::Debug{} << "could not acquire VRTrackedCamera";
        return false;
    }

    bool systemHasCamera = false;
    vr::EVRTrackedCameraError error = trackedCamera->HasCamera(vr::k_unTrackedDeviceIndex_Hmd, &systemHasCamera);
    if (error != vr::VRTrackedCameraError_None || !systemHasCamera)
    {
        Magnum::Debug{} << "system has no tracked camera available. did you enable it in steam settings? " << trackedCamera->GetCameraErrorNameFromEnum(error);
        return false;
    }

    vr::ETrackedPropertyError propertyError;
    char buffer[128];
    hmd->GetStringTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_CameraFirmwareDescription_String, buffer, sizeof(buffer), &propertyError);
    if (propertyError != vr::TrackedProp_Success)
    {
        Magnum::Debug{} << "failed to get camera firmware desc";
        return false;
    }

    Magnum::Debug{} << "OpenVR_TrackedCamera::Prop_CameraFirmwareDescription_Stringbuffer - " << buffer;

    // @todo: there are two world cameras now .. currently not implemented

    vr::HmdVector2_t focalLength;
    vr::HmdVector2_t principalPoint;
    error = trackedCamera->GetCameraIntrinsics(vr::k_unTrackedDeviceIndex_Hmd, 0, vr::VRTrackedCameraFrameType_MaximumUndistorted, &focalLength, &principalPoint);

    vr::HmdMatrix44_t trackedCameraProjection;
    float near = .01f;
    float far = 100.f;
    error = trackedCamera->GetCameraProjection(vr::k_unTrackedDeviceIndex_Hmd, 0, vr::VRTrackedCameraFrameType_MaximumUndistorted, near, far, &trackedCameraProjection);

    projectionMatrix = Magnum::Matrix4x4(trackedCameraProjection).transposed(); 

    intrin.fx = focalLength.v[0];
    intrin.fy = focalLength.v[1];
    intrin.ppx = principalPoint.v[0];
    intrin.ppy = principalPoint.v[1];

    return true;
}

bool VRCamera::start()
{
    uint32_t frameWidth = 0;
    uint32_t frameHeight = 0;
    uint32_t framebufferSize = 0;

    intrin.width = frameWidth;
    intrin.height = frameHeight;

    if (trackedCamera->GetCameraFrameSize(vr::k_unTrackedDeviceIndex_Hmd, vr::VRTrackedCameraFrameType_MaximumUndistorted, &frameWidth, &frameHeight, &framebufferSize) != vr::VRTrackedCameraError_None)
    {
        Magnum::Debug{} << "GetCameraFrameSize() failed";
        return false;
    }

    // Generate texture handle
    frame.texture = Magnum::GL::Texture2D(); //(intrin.width, intrin.height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    // Create a persistent buffer for holding the incoming camera data
    Corrade::Containers::Array<char> data;
    frame.rawBytes = Magnum::Image2D{Magnum::PixelFormat::RGBA8Unorm, {512, 256}, std::move(data)}; //image_buffer<uint8_t, 3>(int2(frameWidth, frameHeight));

    lastFrameSequence = 0;

    // Open and cache OpenVR camera handle
    trackedCamera->AcquireVideoStreamingService(vr::k_unTrackedDeviceIndex_Hmd, &trackedCameraHandle);
    if (trackedCameraHandle == INVALID_TRACKED_CAMERA_HANDLE)
    {
        Magnum::Debug{} << "AcquireVideoStreamingService() failed";
        return false;
    }

    return true;
}

void VRCamera::stop()
{
    trackedCamera->ReleaseVideoStreamingService(trackedCameraHandle);
    trackedCameraHandle = INVALID_TRACKED_CAMERA_HANDLE;
}

void VRCamera::capture()
{
    if (!trackedCamera || !trackedCameraHandle) return;

    vr::CameraVideoStreamFrameHeader_t frameHeader;
    vr::EVRTrackedCameraError error = trackedCamera->GetVideoStreamFrameBuffer(trackedCameraHandle, vr::VRTrackedCameraFrameType_MaximumUndistorted, nullptr, 0, &frameHeader, sizeof(frameHeader));
    if (error != vr::VRTrackedCameraError_None) return;

    // Ideally called once every ~16ms but who knows
    if (frameHeader.nFrameSequence == lastFrameSequence) return;

    // Copy
    error = trackedCamera->GetVideoStreamFrameBuffer(trackedCameraHandle, vr::VRTrackedCameraFrameType_MaximumUndistorted, frame.rawBytes.data(), cameraFrameBufferSize, &frameHeader, sizeof(frameHeader));
    if (error != vr::VRTrackedCameraError_None) return;

    frame.render_pose = Magnum::Matrix4x4(frameHeader.standingTrackedDevicePose.mDeviceToAbsoluteTracking);

    lastFrameSequence = frameHeader.nFrameSequence;

    frame.texture.image(0, frame.rawBytes);
//    glTextureImage2DEXT(frame.texture, GL_TEXTURE_2D, 0, GL_RGB, intrin.width, intrin.height, 0, GL_RGB, GL_UNSIGNED_BYTE, frame.rawBytes.data());
}