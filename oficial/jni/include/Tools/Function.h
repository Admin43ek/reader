#pragma once
#ifndef FUNCTION_H
#define FUNCTION_H

#define PI 3.141592653589793238

#include "VecTool.h"

typedef char UTF8;
typedef unsigned short UTF16;

// Utility: extract translation component from a matrix
inline Vec3 MarixToVector(const FMatrix &matrix) {
    return Vec3(matrix.M[3][0], matrix.M[3][1], matrix.M[3][2]);
}

// Utility: multiply two 4x4 matrices
inline FMatrix MatrixMulti(const FMatrix &m1, const FMatrix &m2) {
    FMatrix matrix{};
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                matrix.M[i][j] += m1.M[i][k] * m2.M[k][j];
            }
        }
    }
    return matrix;
}

// Utility: convert FTransform to a matrix
inline FMatrix TransformToMatrix(const FTransform &transform) {
    FMatrix matrix{};
    matrix.M[3][0] = transform.Translation.x;
    matrix.M[3][1] = transform.Translation.y;
    matrix.M[3][2] = transform.Translation.z;

    const float x2 = transform.Rotation.x + transform.Rotation.x;
    const float y2 = transform.Rotation.y + transform.Rotation.y;
    const float z2 = transform.Rotation.z + transform.Rotation.z;
    const float xx2 = transform.Rotation.x * x2;
    const float yy2 = transform.Rotation.y * y2;
    const float zz2 = transform.Rotation.z * z2;

    matrix.M[0][0] = (1.0f - (yy2 + zz2)) * transform.Scale3D.x;
    matrix.M[1][1] = (1.0f - (xx2 + zz2)) * transform.Scale3D.y;
    matrix.M[2][2] = (1.0f - (xx2 + yy2)) * transform.Scale3D.z;

    const float yz2 = transform.Rotation.y * z2;
    const float wx2 = transform.Rotation.w * x2;
    matrix.M[2][1] = (yz2 - wx2) * transform.Scale3D.z;
    matrix.M[1][2] = (yz2 + wx2) * transform.Scale3D.y;

    const float xy2 = transform.Rotation.x * y2;
    const float wz2 = transform.Rotation.w * z2;
    matrix.M[1][0] = (xy2 - wz2) * transform.Scale3D.y;
    matrix.M[0][1] = (xy2 + wz2) * transform.Scale3D.x;

    const float xz2 = transform.Rotation.x * z2;
    const float wy2 = transform.Rotation.w * y2;
    matrix.M[2][0] = (xz2 + wy2) * transform.Scale3D.z;
    matrix.M[0][2] = (xz2 - wy2) * transform.Scale3D.x;

    matrix.M[0][3] = 0.0f;
    matrix.M[1][3] = 0.0f;
    matrix.M[2][3] = 0.0f;
    matrix.M[3][3] = 1.0f;
    return matrix;
}

#endif // FUNCTION_H