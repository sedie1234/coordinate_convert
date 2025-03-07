## requirements
1. cmake
2. cuda
3. eigen library

```
$ sudo apt install cmake
$ sudo apt install nvidia-jetpack
$ export PATH=/usr/local/cuda/bin/${PATH:+:${PATH}}
$ export LD_LIBRARY_PATH=/usr/local/cuda/lib64${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}
$ sudo apt install libeigen3-dev
```

## build

```
$ mkdir build && cd build
$ cmake ..
$ make
```

## usage
```
$ ./gpu_test <# of data>
```