//
// Created by netlabs on 15.07.19.
//

#ifndef MAGNUM_OPENVR_OPENVRHMD_H
#define MAGNUM_OPENVR_OPENVRHMD_H


#include "openvr.h"



#include "Magnum/OpenvrIntegration/HmdBase.h"
#include "Magnum/OpenvrIntegration/Integration.h"

namespace Magnum { namespace VR {


        class OpenvrHMD : public HMDBase
        {
            vr::IVRSystem * _hmd { nullptr };
            vr::IVRRenderModels * _renderModels { nullptr };

            Magnum::Vector2i _renderTargetSize;
            Magnum::Matrix4 _hmdPose, _worldPose;

            CachedControllerRenderData _controllerRenderData[2];
            VRController _controllers[2];
            std::function<void(CachedControllerRenderData & data)> _async_data_cb;
            void load_render_data_impl(vr::VREvent_t event);

        public:

            OpenvrHMD();
            ~OpenvrHMD();

            virtual void set_world_pose(const Magnum::Matrix4 & p) override final;
            virtual Magnum::Matrix4 get_world_pose() override final;

            virtual Magnum::Matrix4 get_hmd_pose() const;
            virtual void set_hmd_pose(const Magnum::Matrix4 & p) override final;

            virtual Magnum::Matrix4 get_eye_pose(VREye eye) override final;
            virtual VRController get_controller(VRControllerRole controller) override final;
            virtual Magnum::Vector2i get_recommended_render_target_size() override final;
            virtual Magnum::Matrix4 get_proj_matrix(VREye eye, float near_clip, float far_clip) override final;
            virtual void get_optical_properties(VREye eye, float & aspectRatio, float & vfov) override final;
            virtual Magnum::Trade::MeshData3D get_stencil_mask(VREye eye) override final;
            virtual VRInputVendor get_input_vendor() override final;

            virtual void controller_render_data_callback(std::function<void(CachedControllerRenderData & data)> callback) override final;

            virtual void update() override final;
            virtual void submit(const GLuint leftEye, const GLuint rightEye) override final;
        };

}}




#endif //MAGNUM_OPENVR_OPENVRHMD_H
