#ifndef Magnum_OpenvrIntegration_Integration_h
#define Magnum_OpenvrIntegration_Integration_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2015, 2016 Jonathan Hale <squareys@googlemail.com>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

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

            template<> struct VectorConverter<2, Float, HmdVector2_t> {
                static Vector<2, Float> from(const HmdVector2_t& other) {
                    return {other.v[0], other.v[1]};
                }

                static HmdVector2_t to(const Vector<2, Float>& other) {
                    HmdVector2_t out;
                    out.v[0] = other[0];
                    out.v[1] = other[1];
                    return out;
                }
            };

            template<> struct VectorConverter<3, Float, HmdVector3_t> {
                static Vector<3, Float> from(const HmdVector3_t& other) {
                    return {other.v[0], other.v[1], other.v[2]};
                }

                static HmdVector3_t to(const Vector<3, Float>& other) {
                    HmdVector3_t out;
                    out.v[0] = other[0];
                    out.v[1] = other[1];
                    out.v[2] = other[2];
                    return out;
                }
            };

            template<> struct VectorConverter<3, Double, HmdVector3d_t> {
                static Vector<3, Double> from(const HmdVector3d_t& other) {
                    return {other.v[0], other.v[1], other.v[2]};
                }

                static HmdVector3d_t to(const Vector<3, Double>& other) {
                    HmdVector3d_t out;
                    out.v[0] = other[0];
                    out.v[1] = other[1];
                    out.v[2] = other[2];
                    return out;
                }
            };

            template<> struct VectorConverter<4, Float, HmdVector4_t> {
                static Vector<4, Float> from(const HmdVector4_t& other) {
                    return {other.v[0], other.v[1], other.v[2], other.v[3]};
                }

                static HmdVector4_t to(const Vector<4, Float>& other) {
                    HmdVector4_t out;
                    out.v[0] = other[0];
                    out.v[1] = other[1];
                    out.v[2] = other[2];
                    out.v[3] = other[3];
                    return out;
                }
            };

            template<> struct RectangularMatrixConverter<3, 3, Float, HmdMatrix33_t> {
                static RectangularMatrix<3, 3, Float> from(const HmdMatrix33_t& other) {
                    return RectangularMatrix<3, 3, Float>::from(reinterpret_cast<const Float*>(other.m));
                }

                static HmdMatrix33_t to(const RectangularMatrix<3, 3, Float>& other) {
                    HmdMatrix33_t out;
                    for (std::size_t col = 0; col != 3; ++col)
                        for (std::size_t row = 0; row != 3; ++row)
                            out.m[col][row] = other[col][row];
                    return out;
                }
            };


            template<> struct RectangularMatrixConverter<3, 4, Float, HmdMatrix34_t> {
                static RectangularMatrix<3, 4, Float> from(const HmdMatrix34_t& other) {
                    return RectangularMatrix<3, 4, Float>::from(reinterpret_cast<const Float*>(other.m));
                }

                static HmdMatrix34_t to(const RectangularMatrix<3, 4, Float>& other) {
                    HmdMatrix34_t out;
                    for (std::size_t col = 0; col != 4; ++col)
                        for (std::size_t row = 0; row != 3; ++row)
                            out.m[col][row] = other[col][row];
                    return out;
                }
            };

            template<> struct RectangularMatrixConverter<4, 4, Float, HmdMatrix44_t> {
                static RectangularMatrix<4, 4, Float> from(const HmdMatrix44_t& other) {
                    return RectangularMatrix<4, 4, Float>::from(reinterpret_cast<const Float*>(other.m));
                }

                static HmdMatrix44_t to(const RectangularMatrix<4, 4, Float>& other) {
                    HmdMatrix44_t out;
                    for (std::size_t col = 0; col != 4; ++col)
                        for (std::size_t row = 0; row != 4; ++row)
                            out.m[col][row] = other[col][row];
                    return out;
                }
            };

            template<> struct QuaternionConverter<Float, HmdQuaternionf_t> {
                static Quaternion<Float> from(const HmdQuaternionf_t& other) {
                    return Quaternion<Float>(Vector3<Float>{other.x, other.y, other.z}, other.w);
                }

                static HmdQuaternionf_t to(const Quaternion<Float>& other) {
                    const Vector3<Float> imaginary = other.vector();
                    return {imaginary.x(), imaginary.y(), imaginary.z(), other.scalar()};
                }
            };

            template<> struct QuaternionConverter<Double, HmdQuaternion_t> {
                static Quaternion<Double> from(const HmdQuaternion_t& other) {
                    return Quaternion<Double>(Vector3<Double>{other.x, other.y, other.z}, other.w);
                }

                static HmdQuaternion_t to(const Quaternion<Double>& other) {
                    const Vector3<Double> imaginary = other.vector();
                    return {imaginary.x(), imaginary.y(), imaginary.z(), other.scalar()};
                }
            };

        }}}
#endif

#endif
