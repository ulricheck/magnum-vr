//
// Created by netlabs on 15.07.19.
//
// inpsired by polymer rendering engine ..

#ifndef MAGNUM_OPENVR_HMDBASE_H
#define MAGNUM_OPENVR_HMDBASE_H

#include <map>
#include <functional>

#include "Magnum/Magnum.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/Trade/MeshData3D.h"


namespace Magnum { namespace VR {

///////////////////
//   Utilities   //
///////////////////

        enum class VRInputVendor
        {
            Unknown,
            ViveWand,
            ValveKnuckles,
            OculusRiftTouch,
            OculusQuestTouch,
            OculusGo,
            LeapMotion
        };

        enum class VRControllerRole
        {
            Invalid = 0,
            LeftHand = 1,
            RightHand = 2
        };

        struct CachedControllerRenderData
        {
            Magnum::Trade::MeshData3D mesh;
            Magnum::GL::Texture2D tex;
            bool loaded{ false };
            VRControllerRole role;
        };

        struct VRButtonState
        {
            bool prev_down{ false }; // do not use directly, for state tracking only
            bool down{ false };      // query if the button is currently down
            bool pressed{ false };   // query if the button was pressed for a single frame
            bool released{ false };  // query if the button was released for a single frame
        };

        void update_button_state(VRButtonState & state, const bool value);

        enum class VREye
        {
            LeftEye = 0,
            RightEye = 1
        };

        enum class VRButton
        {
            System,
            Menu,
            Grip,
            XY,
            Trigger
        };

        struct VRController
        {
            Magnum::Matrix4 t{Magnum::Math::IdentityInit};
            Magnum::Vector2 xy_values{ 0.f, 0.f };
            std::unordered_map<VRButton, VRButtonState> buttons;
        };

        VRButton get_button_id_for_vendor(const uint32_t which_button, const VRInputVendor vendor);

////////////////////////////
//   hmd base interface   //
////////////////////////////

        struct HMDBase
        {
            // The world pose represents an offset that is applied to `get_hmd_pose()`
            // It is most useful for teleportation functionality.
            virtual void set_world_pose(const Magnum::Matrix4 & p) = 0;
            virtual Magnum::Matrix4 get_world_pose() = 0;

            // The pose of the headset, relative to the current world pose.
            // If no world pose is set, then it is relative to the center of the
            // tracking volume. The view matrix is derived from this data.
            virtual Magnum::Matrix4 get_hmd_pose() const = 0;
            virtual void set_hmd_pose(const Magnum::Matrix4 & p) = 0;

            // Returns the per-eye flavor of the view matrix. The eye pose and
            // hmd pose must be multiplied together to derive the per-eye view matrix
            // with correct stereo disparity that must be submitted to the renderer.
            virtual Magnum::Matrix4 get_eye_pose(VREye eye) = 0;

            // Return the recommended render target size, in pixels.
            virtual uint2 get_recommended_render_target_size() = 0;

            // Returns the per-eye projection matrix given a near and far clip value.
            virtual Magnum::Matrix4 get_proj_matrix(VREye eye, float near_clip, float far_clip) = 0;

            // Returns the aspect ratio and vertical field of view in radians for a given eye
            virtual void get_optical_properties(Magnum::Matrix4 eye, float & aspectRatio, float & vfov) = 0;

            // Returns current controller state.
            virtual VRController get_controller(VRControllerRole controller) = 0;
            virtual void controller_render_data_callback(std::function<void(CachedControllerRenderData & data)> callback) = 0;

            // Returns the vendor of the the input controllers
            virtual VRInputVendor get_input_vendor() = 0;

            // Returns the stencil mask for a given eye
            virtual Magnum::Trade get_stencil_mask(VREye eye) = 0;

            // Must be called per-frame in the update loop
            virtual void update() = 0;

            // Submit rendered per-eye OpenGL textures to the compositor
            virtual void submit(const GLuint leftEye, const GLuint rightEye) = 0;
        };

}}

#endif //MAGNUM_OPENVR_HMDBASE_H
