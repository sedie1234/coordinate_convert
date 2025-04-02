#include <iostream>
#include <cstring>

#include "convert_matrix.h"


void ConvertMatrix::setTransition(Coordinate transition){
    this->transition = transition;
}

void ConvertMatrix::setStartQuaternion(Quaternion quat){
    memcpy(&start_quat, &quat, sizeof(Quaternion));
}

void ConvertMatrix::setDstQuaternion(Quaternion quat){
    memcpy(&dst_quat, &quat, sizeof(Quaternion));
}

Quaternion ConvertMatrix::cartesianToQuaternion(Coordinate coord){
    Quaternion quat;
    quat.w = 0;
    quat.x = coord.x;
    quat.y = coord.y;
    quat.z = coord.z;

    return quat;
}

void ConvertMatrix::setStartToDstQuaternion(){
    //q_ds = q_s x q_d^-1

    // // W = Ws Wd + Xs Xd + Ys Yd + Zs Zd
    // start_to_dst_quat.w = start_quat.w * dst_quat.w + start_quat.x * dst_quat.x
    //                     + start_quat.y * dst_quat.y + start_quat.z * dst_quat.z;
    // // X = Ws Xd - Xs Wd - Yd Zs + Zd Ys
    // start_to_dst_quat.x = start_quat.w * dst_quat.x - start_quat.x * dst_quat.w
    //                      - start_quat.z * dst_quat.y + start_quat.y * dst_quat.z;
    // // Y = Ws Yd - Xs Zd - Ys Wd + Zs Xd
    // start_to_dst_quat.y = start_quat.w * dst_quat.y - start_quat.x * dst_quat.z
    //                     - start_quat.y * dst_quat.w + start_quat.z * dst_quat.x;
    // // Z = Ws Zd + Xs Yd - Ys Xd - Zs Wd
    // start_to_dst_quat.z = start_quat.w * dst_quat.z + start_quat.x * dst_quat.y
    //                     - start_quat.y * dst_quat.x - start_quat.z * dst_quat.w;

    // W = Ws Wd - Xs Xd - Ys Yd - Zs Zd
    start_to_dst_quat.w = start_quat.w * dst_quat.w - start_quat.x * dst_quat.x
                        - start_quat.y * dst_quat.y - start_quat.z * dst_quat.z;
    // X = Ws Xd + Xs Wd + Yd Zs - Zd Ys
    start_to_dst_quat.x = start_quat.w * dst_quat.x + start_quat.x * dst_quat.w
                         + start_quat.z * dst_quat.y - start_quat.y * dst_quat.z;
    // Y = Ws Yd + Xs Zd + Ys Wd - Zs Xd
    start_to_dst_quat.y = start_quat.w * dst_quat.y + start_quat.x * dst_quat.z
                        + start_quat.y * dst_quat.w - start_quat.z * dst_quat.x;
    // Z = Ws Zd - Xs Yd + Ys Xd + Zs Wd
    start_to_dst_quat.z = start_quat.w * dst_quat.z - start_quat.x * dst_quat.y
                        + start_quat.y * dst_quat.x + start_quat.z * dst_quat.w;                        
}

void ConvertMatrix::makeConvertMatrix(Quaternion quat, Coordinate trans){
    
    convert_matrix.resize(16);
    // convert matrix 
    //     R T
    //     0 1

    // R : Rotate matrix with Quaternion (3x3)
    //  1-2(y^2+z^2)    2(xy-wz)    2(xz+wy)
    //    2(xy+wz)    1-2(x^2+z^2)  2(yz-wx)
    //    2(xz-wy)      2(yz+wx)  1-2(x^2+y^2)

    // R^-1 : Rotate matrix of coordinate system with Quaternion (3x3)
    //  1-2(y^2+z^2)    2(xy+wz)    2(xz-wy)
    //    2(xy-wz)    1-2(x^2+z^2)  2(yz+wx)
    //    2(xz+wy)      2(yz-wx)  1-2(x^2+y^2)

    // T : Transition matrix (1x3)
    //     x
    //     y
    //     z

    // (0, 0)
    convert_matrix[0] = 1 - 2*(quat.y * quat.y + quat.z * quat.z);
    // (0, 1)
    convert_matrix[1] = 2*(quat.x*quat.y - quat.w*quat.z);
    // (0, 2)
    convert_matrix[2] = 2*(quat.x*quat.z + quat.w*quat.y);
    // (0, 3)
    convert_matrix[3] = trans.x;
    // (1, 0)
    convert_matrix[4] = 2*(quat.x*quat.y + quat.w*quat.z);
    // (1, 1)
    convert_matrix[5] = 1 - 2*(quat.x * quat.x + quat.z * quat.z);
    // (1, 2)
    convert_matrix[6] = 2*(quat.y*quat.z - quat.w*quat.x);
    // (1, 3)
    convert_matrix[7] = trans.y;
    // (2, 0)
    convert_matrix[8] = 2*(quat.x*quat.z - quat.w*quat.y);
    // (2, 1)
    convert_matrix[9] = 2*(quat.y*quat.z + quat.w*quat.x);
    // (2, 2)
    convert_matrix[10] = 1 - 2*(quat.x*quat.x + quat.y*quat.y);
    // (2, 3)
    convert_matrix[11] = trans.z;
    // (3, 0~3)
    convert_matrix[12] = 0;
    convert_matrix[13] = 0;
    convert_matrix[14] = 0;
    convert_matrix[15] = 1;



}

void ConvertMatrix::makeConvertMatrix(Coordinate trans){
    makeConvertMatrix(start_to_dst_quat, trans);
}

void ConvertMatrix::makeConvertMatrix(){
    makeConvertMatrix(start_to_dst_quat, transition);
}

ConvertMatrix::~ConvertMatrix(){
    // if(convert_matrix != NULL){
    //     delete[] convert_matrix;
    // }
}
