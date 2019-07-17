#ifndef Magnum_OpenvrIntegration_Integration_h
#define Magnum_OpenvrIntegration_Integration_h


/** @file
@brief Conversion of libopen_vr math types

Provides conversion for the following types:

| Magnum type                           | Equivalent libOVR type            |
| ------------------------------------- | --------------------------------- |
| @ref Magnum::Vector<2, Float>         | @cpp HmdVector2_t @ce <b></b>
| @ref Magnum::Vector<3, Float>         | @cpp HmdVector3_t @ce <b></b>
| @ref Magnum::Vector<3, Double>        | @cpp HmdVector3d_t @ce <b></b>
| @ref Magnum::Vector<4, Float>         | @cpp HmdVector4_t @ce <b></b>
| @ref Magnum::Math::RectangularMatrix<3, 3, Float>  | @cpp HmdMatrix33_t @ce <b></b>
| @ref Magnum::Math::RectangularMatrix<3, 4, Float>  | @cpp HmdMatrix34_t @ce <b></b>
| @ref Magnum::Math::RectangularMatrix<4, 4, Float>  | @cpp HmdMatrix44_t @ce <b></b>
| @ref Magnum::Math::Quaternion<Double>  | @cpp HmdQuaternionf_t @ce <b></b>         |
| @ref Magnum::Quaternion "Quaternion"  | @cpp HmdQuaternion_t @ce <b></b>         |

Magnum and Openvr SDK stores matrices in column-major order.

Example usage:

@snippet OpenvrIntegration.cpp Integration

@see @ref types-thirdparty-integration
*/

#include <Magnum/Math/Range.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Quaternion.h>
#include <Magnum/Math/DualQuaternion.h>
#include <openvr.h>

/* Don't list (useless) Magnum and Math namespaces without anything else */
#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Magnum { namespace Math { namespace Implementation {

            template<> struct VectorConverter<2, Float, vr::HmdVector2_t> {
                static Vector<2, Float> from(const vr::HmdVector2_t& other) {
                    return {other.v[0], other.v[1]};
                }

                static vr::HmdVector2_t to(const Vector<2, Float>& other) {
                    vr::HmdVector2_t out{};
                    out.v[0] = other[0];
                    out.v[1] = other[1];
                    return out;
                }
            };

            template<> struct VectorConverter<3, Float, vr::HmdVector3_t> {
                static Vector<3, Float> from(const vr::HmdVector3_t& other) {
                    return {other.v[0], other.v[1], other.v[2]};
                }

                static vr::HmdVector3_t to(const Vector<3, Float>& other) {
                    vr::HmdVector3_t out{};
                    out.v[0] = other[0];
                    out.v[1] = other[1];
                    out.v[2] = other[2];
                    return out;
                }
            };

            template<> struct VectorConverter<3, Double,vr:: HmdVector3d_t> {
                static Vector<3, Double> from(const vr::HmdVector3d_t& other) {
                    return {other.v[0], other.v[1], other.v[2]};
                }

                static vr::HmdVector3d_t to(const Vector<3, Double>& other) {
                    vr::HmdVector3d_t out{};
                    out.v[0] = other[0];
                    out.v[1] = other[1];
                    out.v[2] = other[2];
                    return out;
                }
            };

            template<> struct VectorConverter<4, Float, vr::HmdVector4_t> {
                static Vector<4, Float> from(const vr::HmdVector4_t& other) {
                    return {other.v[0], other.v[1], other.v[2], other.v[3]};
                }

                static vr::HmdVector4_t to(const Vector<4, Float>& other) {
                    vr::HmdVector4_t out{};
                    out.v[0] = other[0];
                    out.v[1] = other[1];
                    out.v[2] = other[2];
                    out.v[3] = other[3];
                    return out;
                }
            };

            template<> struct RectangularMatrixConverter<3, 3, Float, vr::HmdMatrix33_t> {
                static RectangularMatrix<3, 3, Float> from(const vr::HmdMatrix33_t& other) {
                    return RectangularMatrix<3, 3, Float>::from(reinterpret_cast<const Float*>(other.m));
                }

                static vr::HmdMatrix33_t to(const RectangularMatrix<3, 3, Float>& other) {
                    vr::HmdMatrix33_t out{};
                    for (std::size_t col = 0; col != 3; ++col)
                        for (std::size_t row = 0; row != 3; ++row)
                            out.m[col][row] = other[col][row];
                    return out;
                }
            };


            template<> struct RectangularMatrixConverter<3, 4, Float, vr::HmdMatrix34_t> {
                static RectangularMatrix<3, 4, Float> from(const vr::HmdMatrix34_t& other) {
                    return RectangularMatrix<3, 4, Float>::from(reinterpret_cast<const Float*>(other.m));
                }

                static vr::HmdMatrix34_t to(const RectangularMatrix<3, 4, Float>& other) {
                    vr::HmdMatrix34_t out{};
                    for (std::size_t col = 0; col != 4; ++col)
                        for (std::size_t row = 0; row != 3; ++row)
                            out.m[col][row] = other[col][row];
                    return out;
                }
            };

            template<> struct RectangularMatrixConverter<4, 4, Float, vr::HmdMatrix34_t> {
                static RectangularMatrix<4, 4, Float> from(const vr::HmdMatrix34_t& other) {
                    RectangularMatrix<4, 4, Float> out{Magnum::Math::ZeroInit};
                    for (std::size_t col = 0; col != 4; ++col)
                        for (std::size_t row = 0; row != 3; ++row)
                            out[col][row] = other.m[col][row];
                    return out;
                }

                static vr::HmdMatrix34_t to(const RectangularMatrix<4, 4, Float>& other) {
                    vr::HmdMatrix34_t out{};
                    for (std::size_t col = 0; col != 4; ++col)
                        for (std::size_t row = 0; row != 3; ++row)
                            out.m[col][row] = other[col][row];
                    return out;
                }
            };

            template<> struct RectangularMatrixConverter<4, 4, Float, vr::HmdMatrix44_t> {
                static RectangularMatrix<4, 4, Float> from(const vr::HmdMatrix44_t& other) {
                    return RectangularMatrix<4, 4, Float>::from(reinterpret_cast<const Float*>(other.m));
                }

                static vr::HmdMatrix44_t to(const RectangularMatrix<4, 4, Float>& other) {
                    vr::HmdMatrix44_t out{};
                    for (std::size_t col = 0; col != 4; ++col)
                        for (std::size_t row = 0; row != 4; ++row)
                            out.m[col][row] = other[col][row];
                    return out;
                }
            };

            template<> struct QuaternionConverter<Float, vr::HmdQuaternionf_t> {
                static Quaternion<Float> from(const vr::HmdQuaternionf_t& other) {
                    return Quaternion<Float>(Vector3<Float>{other.x, other.y, other.z}, other.w);
                }

                static vr::HmdQuaternionf_t to(const Quaternion<Float>& other) {
                    const Vector3<Float> imaginary = other.vector();
                    return {imaginary.x(), imaginary.y(), imaginary.z(), other.scalar()};
                }
            };

            template<> struct QuaternionConverter<Double, vr::HmdQuaternion_t> {
                static Quaternion<Double> from(const vr::HmdQuaternion_t& other) {
                    return Quaternion<Double>(Vector3<Double>{other.x, other.y, other.z}, other.w);
                }

                static vr::HmdQuaternion_t to(const Quaternion<Double>& other) {
                    const Vector3<Double> imaginary = other.vector();
                    return {imaginary.x(), imaginary.y(), imaginary.z(), other.scalar()};
                }
            };

        }}}
#endif

#endif
