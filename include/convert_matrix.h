#pragma once

#include <vector>

typedef struct{
    float x;
    float y;
    float z;
}Coordinate;

typedef struct{
    float w; //scalar
    float x; //vector x
    float y; //vector y
    float z; //vector z
}Quaternion;

class ConvertMatrix{

public: 
    ConvertMatrix(){
        transition.x = 0;
        transition.y = 0;
        transition.z = 0;
    }

    void setTransition(Coordinate transition);
    void setStartQuaternion(Quaternion quat);
    void setDstQuaternion(Quaternion quat);
    Quaternion cartesianToQuaternion(Coordinate coord);
    void setStartToDstQuaternion();
    // after set transition, start_quat, dst_quat, run the function
    void makeConvertMatrix(Quaternion quat, Coordinate trans);
    void makeConvertMatrix(Coordinate trans);
    void makeConvertMatrix();

    ~ConvertMatrix();

    std::vector<float> convert_matrix;

private:
    Coordinate transition;
    // Matrix S->d : qs^-1 qd = q
    Quaternion start_quat;
    Quaternion dst_quat;
    Quaternion start_to_dst_quat;

};