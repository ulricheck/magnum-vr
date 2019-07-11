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
#include "Magnum/OpenvrIntegration/visibility.h"

#include <openvr.h>


namespace Magnum { namespace OpenvrIntegration {

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

            ~Context();

            /** @brief Copying is not allowed */
            VRSystem& operator=(const VRSystem&) = delete;

            /** @brief Moving is not allowed */
            VRSystem& operator=(VRSystem&&) = delete;

        private:
            static VRSystem* _instance;
            vr::IVRSystem *_pHMD;
            std::string _string_driver;
            std::string _string_display;



            std::string getTrackedDeviceString(vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL );



        };



    }}


#endif //MAGNUMVR_OPENVR_HMD_H
