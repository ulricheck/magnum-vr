//
// Created by netlabs on 7/27/19.
//

#ifndef MAGNUM_OPENVR_VRCAMERA_H
#define MAGNUM_OPENVR_VRCAMERA_H

#include <Magnum/Math/Matrix.h>
#include "HmdBase.h"
#include "Magnum/Image.h"

namespace artekmed
{
    struct camera_intrinsics
    {
        int width;  // width of the image in pixels
        int height; // height of the image in pixels
        float ppx;  // horizontal coordinate of the principal point of the image, as a pixel offset from the left edge
        float ppy;  // vertical coordinate of the principal point of the image, as a pixel offset from the top edge
        float fx;   // focal length of the image plane, as a multiple of pixel width
        float fy;   // focal length of the image plane, as a multiple of pixel height
    };

    struct tracked_camera_frame
    {
        Magnum::Matrix4x4 render_pose;
        Magnum::GL::Texture2D texture{Magnum::NoCreate};
        Magnum::Image2D rawBytes;
    };

    class VRCamera
    {
        vr::IVRSystem * hmd{ nullptr };
        vr::IVRTrackedCamera * trackedCamera{ nullptr };
        vr::TrackedCameraHandle_t trackedCameraHandle{ INVALID_TRACKED_CAMERA_HANDLE };

        uint32_t lastFrameSequence{ 0 };
        uint32_t cameraFrameBufferSize{ 0 };

        Magnum::Matrix4x4 projectionMatrix;
        camera_intrinsics intrin;
        tracked_camera_frame frame;

    public:

        bool initialize(vr::IVRSystem * vr_system);
        bool start();
        void stop();
        void capture();

        camera_intrinsics get_intrinsics() const { return intrin; }
        Magnum::Matrix4x4 const get_projection_matrix() { return projectionMatrix; }
        tracked_camera_frame & get_frame() { return frame; }
    };

} // end namespace artekmed


#endif //MAGNUM_OPENVR_VRCAMERA_H
