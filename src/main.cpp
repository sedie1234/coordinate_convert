#include <iostream>
#include <cuda_runtime.h>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <vector>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include <thread>
#include <fcntl.h>

#include "convert_matrix.h"

#include "matmul.cuh"

#include <iostream>
#include <vector>
#include <random>

#define CONTINUOUS
#define CPU_MATMUL
#define GPU_MATMUL


#define CM_ROW      4
#define CM_COL      4

void clearLine() {
    std::cout << "\33[2K\r";  // 현재 줄 지우기
}

void moveCursorToBottom() {
    std::cout << "\33[999B";  // 커서를 아래로 이동 (최대값)
}

char getCharNonBlocking();
char getChar();
std::vector<float> generateRandomMatrix(int rows, int cols);
std::vector<float> transposeMatrix(const std::vector<float>& matrix, int rows, int cols);
void printMatrix(const std::vector<float>& matrix, int rows, int cols);

int main(int argc, char* argv[]){

    //temp
    const int ROW_A = 4, COL_A = 4;
    const int ROW_B = 4;
    int COL_B = 100000;

    if(argc == 2){
        COL_B = std::stoi(argv[1]);
    }

    ConvertMatrix CM;

    // get quaternion of start and destination imu
    Quaternion start_quat;
    Quaternion dst_quat;



#ifdef CONTINUOUS

    char input;
    int counter = 0;
    while(1){
        moveCursorToBottom();   // 커서를 콘솔의 가장 아래쪽으로 이동
        clearLine();            // 현재 줄의 내용을 지움
        std::cout << "\n\nCurrent Counter: " << counter++ << std::flush << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 0.5초 대기

        input = getCharNonBlocking();  // 즉시 입력 감지
        if (input == 'q' || input == 'Q') {
            break;
        }
#endif
    // temporarily set quaternion 
    start_quat.w = 0.92388;
    start_quat.x = 0.38268;
    start_quat.y = 0;
    start_quat.z = 0;

    dst_quat.w = 0.70711;
    dst_quat.x = 0;
    dst_quat.y = 0.70711;
    dst_quat.z = 0;

    CM.setStartQuaternion(start_quat);
    CM.setDstQuaternion(dst_quat);
    CM.setStartToDstQuaternion();

    // temporarily set transition
    Coordinate t;
    t.x = 2.2;
    t.y = 1.3;
    t.z = -3.1;

    CM.setTransition(t);

    // make conversion matrix
    CM.makeConvertMatrix();

    // temporarily make dataset
    auto B = generateRandomMatrix(ROW_B, COL_B);
    auto B_T = transposeMatrix(B, ROW_B, COL_B);

#ifdef CPU_MATMUL
    // cpu matmul
    // Eigen 행렬로 변환

    Eigen::Matrix<float, ROW_A, COL_A, Eigen::RowMajor> matA(CM.convert_matrix.data());
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> matB = 
    Eigen::Map<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>(B.data(), ROW_B, COL_B);

    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

    // 행렬 곱 계산
    Eigen::MatrixXf matC;
    matC.resize(ROW_B, COL_B);
    matC = matA * matB;
    std::chrono::duration<double> timeCpuMatmul = std::chrono::system_clock::now() - start;

    printf("cpu matmul elapsed : %lf(ms)\n", timeCpuMatmul * 1000);

    // std::cout << "Matrix matA:\n" << matA << std::endl;
    // std::cout << "Matrix matB:\n" << matB << std::endl;
    // std::cout << "Matrix matC:\n" << matC << std::endl;
#endif

#ifdef GPU_MATMUL
    // gpu matmul
    float* gpuC = new float[COL_B*ROW_B];    

    std::chrono::system_clock::time_point gpu_start = std::chrono::system_clock::now();
    matMul44Wrapper(CM.convert_matrix.data(), B_T.data() , gpuC, COL_B);
    std::chrono::duration<double> timeGpuMatmul = std::chrono::system_clock::now() - gpu_start;

    printf("gpu whole elapsed : %lf(ms)\n", timeGpuMatmul * 1000);

    delete gpuC;
#endif

#ifdef CONTINUOUS

    }
#endif

    // printf("[B_T]\n");
    // for(int i=0; i<COL_B; i++){
    //     for(int j=0; j<4; j++){
    //         printf("%f ", B_T[i*4+j]);
    //     }
    //     printf("\n");
    // }


    // printf("[gpuC]\n");
    // for(int i=0; i<COL_B; i++){
    //     for(int j=0; j<4; j++){
    //         printf("%f ", gpuC[i*4+j]);
    //     }
    //     printf("\n");
    // }


    
    
}


// 랜덤한 float 값을 가지는 1차원 배열 형태의 행렬 생성
std::vector<float> generateRandomMatrix(int rows, int cols) {
    std::vector<float> matrix(rows * cols);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0, 1.0); // 0.0 ~ 1.0 사이 난수

    for (int i = 0; i < rows * cols; ++i) {
        matrix[i] = dis(gen);
    }
    return matrix;
}

// 행렬을 전치(transpose)하는 함수 (1차원 벡터 활용)
std::vector<float> transposeMatrix(const std::vector<float>& matrix, int rows, int cols) {
    std::vector<float> transposed(cols * rows);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            transposed[j * rows + i] = matrix[i * cols + j]; // (i, j) -> (j, i)
        }
    }
    return transposed;
}

// 행렬 출력 함수
void printMatrix(const std::vector<float>& matrix, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            std::cout << matrix[i * cols + j] << " ";
        }
        std::cout << "\n";
    }
}

char getChar() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);  // 현재 터미널 속성 저장
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);  // 즉시 입력 & 입력 문자 숨기기
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);  // 새 설정 적용

    ch = getchar();  // 문자 입력 받기

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  // 원래 설정 복구
    return ch;
}

char getCharNonBlocking() {
    struct termios oldt, newt;
    char ch = 0;
    int old_flags = fcntl(STDIN_FILENO, F_GETFL, 0);  // 현재 입력 설정 저장
    fcntl(STDIN_FILENO, F_SETFL, old_flags | O_NONBLOCK);  // 논블로킹 모드로 변경

    tcgetattr(STDIN_FILENO, &oldt);  // 현재 터미널 속성 저장
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);  // 즉시 입력 & 입력 문자 숨기기
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);  // 새 설정 적용

    if (read(STDIN_FILENO, &ch, 1) <= 0) {  
        ch = 0;  // 입력이 없으면 0 반환
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  // 원래 설정 복구
    fcntl(STDIN_FILENO, F_SETFL, old_flags);  // 원래 설정 복구

    return ch;
}