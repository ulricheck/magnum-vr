//
// Created by netlabs on 15.07.19.
//

#include <cmath>
#include <vector>
#include <array>

#include "Magnum/Mesh.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/Trade/MeshData3D.h"
#include "Magnum/ImageView.h"

#include "OpenvrHMD.h"

using namespace Magnum::VR;


std::string get_tracked_device_string(vr::IVRSystem * pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL)
{
    uint32_t unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, NULL, 0, peError);
    if (unRequiredBufferLen == 0) return "";
    std::vector<char> pchBuffer(unRequiredBufferLen);
    unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer.data(), unRequiredBufferLen, peError);
    std::string result = { pchBuffer.begin(), pchBuffer.end() };
    return result;
}

///////////////////////////////////
//   OpenVR HMD Implementation   //
///////////////////////////////////

OpenvrHMD::OpenvrHMD()
{
    vr::EVRInitError eError = vr::VRInitError_None;
    _hmd = vr::VR_Init(&eError, vr::VRApplication_Scene);
    if (eError != vr::VRInitError_None) throw std::runtime_error("Unable to init VR runtime: " + std::string(vr::VR_GetVRInitErrorAsEnglishDescription(eError)));

    Magnum::Debug{} << "VR Driver:  " << get_tracked_device_string(_hmd, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String);
    Magnum::Debug{} << "VR Display: " << get_tracked_device_string(_hmd, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String);

    uint32_t w, h;
    _hmd->GetRecommendedRenderTargetSize(&w, &h);
    _renderTargetSize = Vector2i(w, h);

    // Setup the compositor
    if (!vr::VRCompositor())
    {
        throw std::runtime_error("could not initialize VRCompositor");
    }
}

OpenvrHMD::~OpenvrHMD()
{
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_FALSE);
    glDebugMessageCallback(nullptr, nullptr);
    if (_hmd) vr::VR_Shutdown();
}

VRController OpenvrHMD::get_controller(VRControllerRole controller)
{
    auto openvr_controller = static_cast<vr::ETrackedControllerRole>(controller);

    if (openvr_controller == vr::TrackedControllerRole_LeftHand) return _controllers[0];
    else if (openvr_controller == vr::TrackedControllerRole_RightHand) return _controllers[1];
    else throw std::invalid_argument("invalid controller enum");
}

void OpenvrHMD::controller_render_data_callback(std::function<void(CachedControllerRenderData & data)> callback)
{
    _async_data_cb = callback;
}

Magnum::GL::Mesh OpenvrHMD::get_stencil_mask(VREye eye)
{
    auto openVREye = static_cast<vr::Hmd_Eye>(eye);

    Magnum::GL::Mesh mesh;

    auto ham =  _hmd->GetHiddenAreaMesh(openVREye, vr::k_eHiddenAreaMesh_Standard); // k_eHiddenAreaMesh_Standard k_eHiddenAreaMesh_Inverse
    Magnum::Debug{} << "Hidden Area Mesh Triangle Count: " << (ham.unTriangleCount > 0);
    std::vector<Magnum::Vector2> hidden_vertices;

    for (uint32_t i = 0; i < ham.unTriangleCount * 3; ++i)
    {
        auto v = ham.pVertexData[i].v;
        hidden_vertices.emplace_back(Magnum::Vector2(v[0] * 2.f - 1.f, 1.f - 2.f * v[1]));
    }

    // XXX TODO
//    mesh.set_vertices(hidden_vertices.size(), hidden_vertices.data(), GL_STATIC_DRAW);
//    mesh.set_attribute(0, 2, GL_FLOAT, GL_FALSE, sizeof(Magnum::Vector2), ((float*)0));

    return std::move(mesh);
}

void OpenvrHMD::set_world_pose(const Magnum::Matrix4 & p)
{
    _worldPose = p;
}

Magnum::Matrix4 OpenvrHMD::get_world_pose()
{
    return _worldPose;
}

// this naming is not consistent !!
// either rename function to "get_hmd_world_pose()" or change behaviour
Magnum::Matrix4 OpenvrHMD::get_hmd_pose() const
{
    return _worldPose * _hmdPose;
}

void OpenvrHMD::set_hmd_pose(const Magnum::Matrix4 & p)
{
    _hmdPose = p;
}

Magnum::Matrix4 OpenvrHMD::get_eye_pose(VREye eye)
{
    return get_hmd_pose() * convert_openvr_pose(_hmd->GetEyeToHeadTransform(static_cast<vr::Hmd_Eye>(eye)));
}

Magnum::Vector2i OpenvrHMD::get_recommended_render_target_size()
{
    return _renderTargetSize;
}

Magnum::Matrix4 OpenvrHMD::get_proj_matrix(VREye eye, float near_clip, float far_clip)
{
    return Magnum::Matrix4(_hmd->GetProjectionMatrix(static_cast<vr::Hmd_Eye>(eye), near_clip, far_clip)).transposed();
}

VRInputVendor OpenvrHMD::get_input_vendor()
{
    return VRInputVendor::ViveWand;
}

void OpenvrHMD::get_optical_properties(VREye eye, float & aspectRatio, float & vfov)
{
    float l_left = 0.0f, l_right = 0.0f, l_top = 0.0f, l_bottom = 0.0f;
    _hmd->GetProjectionRaw(vr::Hmd_Eye::Eye_Left, &l_left, &l_right, &l_top, &l_bottom);

    float r_left = 0.0f, r_right = 0.0f, r_top = 0.0f, r_bottom = 0.0f;
    _hmd->GetProjectionRaw(vr::Hmd_Eye::Eye_Right, &r_left, &r_right, &r_top, &r_bottom);

    // XXX TODO
//    Magnum::Vector2 tanHalfFov = Magnum::Vector2(max(-l_left, l_right, -r_left, r_right), max(-l_top, l_bottom, -r_top, r_bottom));
//    aspectRatio = tanHalfFov.x() / tanHalfFov.y();
//    vfov = 2.0f * std::atan(tanHalfFov.y());
}

void OpenvrHMD::load_render_data_impl(vr::VREvent_t event)
{
    const vr::ETrackedControllerRole role = _hmd->GetControllerRoleForTrackedDeviceIndex(event.trackedDeviceIndex);

    // Magnum::Debug{} << "Which: " << static_cast<uint32_t>(role) + 1;
    if (role == vr::ETrackedControllerRole::TrackedControllerRole_Invalid) return;

    auto & render_data_for_role = _controllerRenderData[static_cast<uint32_t>(role) - 1];
    render_data_for_role.role = (role == vr::ETrackedControllerRole::TrackedControllerRole_LeftHand) ? VRControllerRole::LeftHand : VRControllerRole::RightHand;

    if (render_data_for_role.loaded)
    {
        _async_data_cb(render_data_for_role);
        return;
    }

    if (_hmd->GetTrackedDeviceClass(event.trackedDeviceIndex) == vr::TrackedDeviceClass_Controller)
    {
        vr::EVRInitError eError = vr::VRInitError_None;
        std::string sRenderModelName = get_tracked_device_string(_hmd, event.trackedDeviceIndex, vr::Prop_RenderModelName_String);

        _renderModels = (vr::IVRRenderModels *)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &eError);
        if (!_renderModels)
        {
            vr::VR_Shutdown();
            throw std::runtime_error("Unable to get render model interface: " + std::string(vr::VR_GetVRInitErrorAsEnglishDescription(eError)));
        }

        {
            vr::RenderModel_t * model = nullptr;
            vr::RenderModel_TextureMap_t * texture = nullptr;

            while (true)
            {
                // see VREvent_TrackedDeviceActivated below for the proper way of doing this
                _renderModels->LoadRenderModel_Async(sRenderModelName.c_str(), &model);
                if (model) _renderModels->LoadTexture_Async(model->diffuseTextureId, &texture);
                if (model && texture) break;
            }

            /* Mesh data */
            std::vector<std::vector<UnsignedInt>> indices({});
            std::vector<std::vector<Vector3>> positions({});
            std::vector<std::vector<Vector2>> textureCoordinates({});
            std::vector<std::vector<Vector3>> normals({});

            // we only have one model:

            indices[0].reserve(model->unTriangleCount*3);
            positions[0].reserve(model->unVertexCount);
            textureCoordinates[0].reserve(model->unVertexCount);
            normals[0].reserve(model->unVertexCount);

            for (uint32_t v = 0; v < model->unVertexCount; v++)
            {
                const vr::RenderModel_Vertex_t vertex = model->rVertexData[v];

                positions[0].emplace_back(Vector3{ vertex.vPosition.v[0], vertex.vPosition.v[1], vertex.vPosition.v[2] });
                normals[0].emplace_back(Vector3{ vertex.vNormal.v[0], vertex.vNormal.v[1], vertex.vNormal.v[2] });
                textureCoordinates[0].emplace_back(Vector2{ vertex.rfTextureCoord[0], vertex.rfTextureCoord[1] });
            }


            for (uint32_t f = 0; f < model->unTriangleCount * 3; f ++)
            {
                indices[0].emplace_back(model->rIndexData[f]);
            }

            render_data_for_role.mesh = Magnum::Trade::MeshData3D{MeshPrimitive::Triangles,
                                                                  std::move(indices), std::move(positions),
                                                                  std::move(normals), std::move(textureCoordinates),
                                                                  {}, nullptr};

            size_t bytes = texture->unWidth * texture->unWidth * 4;

            Magnum::GL::PixelFormat glformat = Magnum::GL::PixelFormat ::RGBA;
            Magnum::GL::PixelType gltype = Magnum::GL::PixelType ::UnsignedByte;
            auto imageData = Corrade::Containers::ArrayView<const char>(reinterpret_cast<const char*>(texture->rubTextureMapData), bytes);

            Magnum::Vector2i size {texture->unWidth, texture->unHeight};

            auto image_view = Magnum::ImageView2D{glformat, gltype, size, imageData};


            render_data_for_role.tex = Magnum::GL::Texture2D();
            render_data_for_role.tex.setWrapping(Magnum::GL::SamplerWrapping::ClampToEdge)
                    .setMagnificationFilter(Magnum::GL::SamplerFilter::Linear)
                    .setMinificationFilter(Magnum::GL::SamplerFilter::Linear)
                    .setStorage(1, Magnum::GL::TextureFormat::RGBA8, Vector2i(texture->unWidth, texture->unHeight))
                    .setSubImage(0, {}, image_view)
                    .generateMipmap();

            _renderModels->FreeTexture(texture);
            _renderModels->FreeRenderModel(model);

            render_data_for_role.loaded = true;

            _async_data_cb(render_data_for_role);
        }
    }
}

void OpenvrHMD::update()
{
    vr::VREvent_t event;
    while (_hmd->PollNextEvent(&event, sizeof(event)))
    {
        switch (event.eventType)
        {
            case vr::VREvent_TrackedDeviceActivated: Magnum::Debug{} << "OpenVR device " << event.trackedDeviceIndex << " attached."; break;
            case vr::VREvent_TrackedDeviceDeactivated: Magnum::Debug{} << "OpenVR device " << event.trackedDeviceIndex << " detached."; break;
            case vr::VREvent_TrackedDeviceUpdated: Magnum::Debug{} << "OpenVR device " << event.trackedDeviceIndex << " updated."; break;
        }

        // Setup render model data if applicable
        load_render_data_impl(event);
    }

    // Get HMD pose
    std::array<vr::TrackedDevicePose_t, 16> poses;
    vr::VRCompositor()->WaitGetPoses(poses.data(), static_cast<uint32_t>(poses.size()), nullptr, 0);
    for (vr::TrackedDeviceIndex_t i = 0; i < poses.size(); ++i)
    {
        if (!poses[i].bPoseIsValid) continue;
        switch (_hmd->GetTrackedDeviceClass(i))
        {
            case vr::TrackedDeviceClass_HMD:
            {
                _hmdPose = Magnum::Math::RectangularMatrix<4, 4, Float>(poses[i].mDeviceToAbsoluteTracking);
                break;
            }
            case vr::TrackedDeviceClass_Controller:
            {
                vr::VRControllerState_t controllerState = vr::VRControllerState_t();
                switch (_hmd->GetControllerRoleForTrackedDeviceIndex(i))
                {
                    case vr::TrackedControllerRole_LeftHand:
                    {
                        // todo - unPacketNum

                        if (_hmd->GetControllerState(i, &controllerState, sizeof(controllerState)))
                        {
                            update_button_state(_controllers[0].buttons[get_button_id_for_vendor(static_cast<uint32_t>(vr::k_EButton_SteamVR_Trigger), get_input_vendor())],
                                                !!(controllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger)));

                            update_button_state(_controllers[0].buttons[get_button_id_for_vendor(static_cast<uint32_t>(vr::k_EButton_SteamVR_Touchpad), get_input_vendor())],
                                                !!(controllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad)));

                            _controllers[0].xy_values = { controllerState.rAxis[vr::k_eControllerAxis_TrackPad].x, controllerState.rAxis[vr::k_eControllerAxis_TrackPad].y };
                            _controllers[0].t = (_worldPose * Magnum::Math::RectangularMatrix<4, 4, Float>(poses[i].mDeviceToAbsoluteTracking));
                        }
                        break;
                    }
                    case vr::TrackedControllerRole_RightHand:
                    {
                        if (_hmd->GetControllerState(i, &controllerState, sizeof(controllerState)))
                        {
                            update_button_state(_controllers[1].buttons[get_button_id_for_vendor(static_cast<uint32_t>(vr::k_EButton_SteamVR_Trigger), get_input_vendor())],
                                                !!(controllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger)));

                            update_button_state(_controllers[1].buttons[get_button_id_for_vendor(static_cast<uint32_t>(vr::k_EButton_SteamVR_Touchpad), get_input_vendor())],
                                                !!(controllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad)));

                            _controllers[1].xy_values = { controllerState.rAxis[vr::k_eControllerAxis_TrackPad].x, controllerState.rAxis[vr::k_eControllerAxis_TrackPad].y };
                            _controllers[1].t = (_worldPose * Magnum::Math::RectangularMatrix<4, 4, Float>(poses[i].mDeviceToAbsoluteTracking));
                        }
                        break;
                    }
                }
                break;
            }
        }
    }
}

void OpenvrHMD::submit(const GLuint leftEye, const GLuint rightEye)
{
    const vr::Texture_t leftTex = { (void*)(intptr_t) leftEye, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
    vr::VRCompositor()->Submit(vr::Eye_Left, &leftTex);

    const vr::Texture_t rightTex = { (void*)(intptr_t) rightEye, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
    vr::VRCompositor()->Submit(vr::Eye_Right, &rightTex);

    glFlush();
}
