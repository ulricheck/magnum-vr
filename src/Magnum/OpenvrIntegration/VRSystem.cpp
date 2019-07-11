//
// Created by netlabs on 08.07.19.
//

#include "VRSystem.h"

namespace Magnum{ namespace OpenvrIntegration {

        VRSystem* VRSystem::_instance = nullptr;

        VRSystem::VRSystem() : _pHMD(nullptr) {
            CORRADE_ASSERT(_instance == nullptr, "Another instance of VRSystem already exists.", );

            // Loading the SteamVR Runtime
            vr::EVRInitError eError = vr::VRInitError_None;
            _pHMD = vr::VR_Init( &eError, vr::VRApplication_Scene );

            if ( eError != vr::VRInitError_None )
            {
                _pHMD = nullptr;
                Debug{} << "Unable to init VR runtime: " << vr::VR_GetVRInitErrorAsEnglishDescription( eError );
                return;
            }


            _string_driver = "No Driver";
            _string_display = "No Display";

            _string_driver = getTrackedDeviceString( vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String );
            _string_display = getTrackedDeviceString( vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String );

            Debug{} << "OpenvrIntegration found device: " << _string_driver << " " << _string_display;

            _instance = this;
        }

        VRSystem::~HMD() {
            if( _pHMD )
            {
                vr::VR_Shutdown();
                _pHMD = nullptr;
            }

            _instance = nullptr;
        }

        VRSystem& VRSystem::get() {
            CORRADE_ASSERT(_instance != nullptr, "No instance of VRSystem for ::get() exists.", );
            return *VRSystem::_instance;
        }

        std::string VRSystem::getTrackedDeviceString(vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError) {
            uint32_t unRequiredBufferLen = pHMD->GetStringTrackedDeviceProperty( unDevice, prop, NULL, 0, peError );
            if( unRequiredBufferLen == 0 )
                return "";

            char *pchBuffer = new char[ unRequiredBufferLen ];
            unRequiredBufferLen = _pHMD->GetStringTrackedDeviceProperty( unDevice, prop, pchBuffer, unRequiredBufferLen, peError );
            std::string sResult = pchBuffer;
            delete [] pchBuffer;
            return sResult;

        }


    }}
