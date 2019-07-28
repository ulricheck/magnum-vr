//
// Created by netlabs on 08.07.19.
//

#ifndef MAGNUMVR_OPENVR_HMD_H
#define MAGNUMVR_OPENVR_HMD_H
/** @file
 * @brief Class @ref Magnum::OpenvrIntegration::IVRSystem
 * //, struct @ref Magnum::OpenvrIntegration::Error, enum @ref Magnum::OvrIntegration::DetectResult, enum set @ref Magnum::OvrIntegration::DetectResults
 */


#include <memory>
#include <Corrade/Utility/Debug.h>
#include <Corrade/Containers/EnumSet.h>

#include "Magnum/OpenvrIntegration/OpenvrIntegration.h"
#include "Magnum/OpenvrIntegration/HmdBase.h"
#include "Magnum/OpenvrIntegration/visibility.h"

#include <openvr.h>
#include <Magnum/Math/Matrix4.h>


namespace Magnum { namespace OpenvrIntegration {
    using namespace Magnum::VR;

        MAGNUM_OPENVRINTEGRATION_EXPORT void update_button_state(VRButtonState & state, const bool value);
        MAGNUM_OPENVRINTEGRATION_EXPORT VRButton get_button_id_for_vendor(const uint32_t which_button, const VRInputVendor vendor);

        class MAGNUM_OPENVRINTEGRATION_EXPORT VRSystem {


            /**
             * @brief Global hmd instance
             *
             * Expects that the instance is created.
             */
            static VRSystem& get();

            /** @brief Constructor */
            explicit VRSystem();

            /** @brief Copying is not allowed */
            VRSystem(const VRSystem&) = delete;

            /** @brief Moving is not allowed */
            VRSystem(VRSystem&&) = delete;

            ~VRSystem();

            /** @brief Copying is not allowed */
            VRSystem& operator=(const VRSystem&) = delete;

            /** @brief Moving is not allowed */
            VRSystem& operator=(VRSystem&&) = delete;



            void set_world_pose(const Magnum::Matrix4 & p);
            Magnum::Matrix4 get_world_pose();

            Magnum::Matrix4 get_hmd_pose() const;
            void set_hmd_pose(const Magnum::Matrix4 & p);

            Magnum::Matrix4 get_eye_pose(VREye eye);

            Magnum::Vector2i get_recommended_render_target_size();

            Magnum::Matrix4 get_proj_matrix(VREye eye, float near_clip, float far_clip);

            void get_optical_properties(VREye eye, float & aspectRatio, float & vfov);

            /** get the stencil mask mesh for an eye **/
            Magnum::GL::Mesh get_stencil_mask(VREye eye);

            VRInputVendor get_input_vendor();


            VRController get_controller(VRControllerRole controller);

            void update();

            void submit(const Magnum::GL::Texture2D& leftEeye, const Magnum::GL::Texture2D& rightEye);

            void controller_render_data_callback(std::function<void(CachedControllerRenderData & data)> callback);

        private:
            static VRSystem* _instance;
            vr::IVRSystem *_pHMD;
            std::string _string_driver;
            std::string _string_display;

            Magnum::Vector2i _renderTargetSize;
            Magnum::Matrix4 _hmdPose, _worldPose{Magnum::Math::IdentityInit};

            vr::IVRRenderModels * _renderModels;


            CachedControllerRenderData _controllerRenderData[2];
            VRController _controllers[2];
            std::function<void(CachedControllerRenderData & data)> _async_data_cb;
            void load_render_data_impl(vr::VREvent_t event);


            std::string get_tracked_device_string(vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop,
                                                  vr::TrackedPropertyError *peError = NULL);



        };



    }}


#endif //MAGNUMVR_OPENVR_HMD_H
