//
// Created by netlabs on 15.07.19.
//
// inpsired by polymer rendering engine ..

#ifndef MAGNUM_OPENVR_HMDBASE_H
#define MAGNUM_OPENVR_HMDBASE_H

#include <map>
#include <unordered_map>
#include <functional>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Matrix4.h"
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
            Magnum::GL::Mesh mesh{Magnum::NoCreate};
            Magnum::GL::Texture2D tex{Magnum::NoCreate};
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


}}

#endif //MAGNUM_OPENVR_HMDBASE_H
