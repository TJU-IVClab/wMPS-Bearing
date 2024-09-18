# wMPS-Bearing

### V1.0, September 1st, 2024
Authors: Ze Yang, Yanyan Li, Hongzhi Du, Yanbiao Sun, and Jigui Zhu.

wMPS-Bearing is the real-time SLAM library able to perform Visual SLAM with wMPS bearing measurements. wMPS-Bearing is as robust as the best systems available in the literature, and significantly more accurate. 

We provide examples to run wMPS-Bearing in the [EuRoC dataset](http://projects.asl.ethz.ch/datasets/doku.php?id=kmavvisualinertialdatasets) using stereo, with or without IMU, and in the self-collected dataset using stereo.

This software is based on [ORB-SLAM3](https://github.com/UZ-SLAMLab/ORB_SLAM3) developed by Carlos Campos, Richard Elvira, Juan J. Gómez Rodríguez, [José M. M. Montiel](https://webdiis.unizar.es/~josemari/), and [Juan D. Tardos](https://webdiis.unizar.es/~jdtardos/).

### Related Publications:

[wMPS-Point] Ze Yang, Yanyan Li, Jiarui Lin, Yanbiao Sun, and Jigui Zhu, **wMPS-SLAM: An Online and Accurate Monocular Visual-wMPS SLAM System**, *IEEE TIM 2023*. **[PDF](https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=10304161)**

[wMPS-Pose] Ze Yang, Yanyan Li, Jiarui Lin, Yanbiao Sun, and Jigui Zhu, **Tightly-coupled fusion of iGPS measurements in optimization-based visual SLAM**, *Optics Express 2023*. **[PDF](https://opg.optica.org/directpdfaccess/1d1e91cd-e7d3-435b-8d2dcab762f75ced_525790/oe-31-4-5910.pdf?da=1&id=525790&seq=0&mobile=no)**.

[ORB-SLAM3] Carlos Campos, Richard Elvira, Juan J. Gómez Rodríguez, José M. M. Montiel and Juan D. Tardós, **ORB-SLAM3: An Accurate Open-Source Library for Visual, Visual-Inertial and Multi-Map SLAM**, *IEEE Transactions on Robotics 37(6):1874-1890, Dec. 2021*. **[PDF](https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=9440682)**.

# 1. License

wMPS-Bearing is released under [GPLv3 license](https://github.com/TJU-IVCLab/wMPS_Bearing/LICENSE).

If you use wMPS-Bearing in an academic work, please cite:
  
    @article{wMPSBearing_TIM,
      title={Accurate Visual Odometry by Leveraging 2DoF Bearing-Only Measurements},
      author={Ze Yang, Yanyan Li, Hongzhi Du, Yanbiao Sun, and Jigui Zhu},
      journal={IEEE Transactions on Instrumentation and Measurement}, 
      volume={},
      number={},
      pages={},
      year={}
     }

# 2. Prerequisites
We have tested the library in **18.04**, but it should be easy to compile in other platforms. A powerful computer (e.g. i7) will ensure real-time performance and provide more stable and accurate results.

## C++11 or C++0x Compiler
We use the new thread and chrono functionalities of C++11.

## Pangolin
We use [Pangolin](https://github.com/stevenlovegrove/Pangolin) for visualization and user interface. Dowload and install instructions can be found at: https://github.com/stevenlovegrove/Pangolin.

## OpenCV
We use [OpenCV](http://opencv.org) to manipulate images and features. Dowload and install instructions can be found at: http://opencv.org. **Required at leat 3.0. Tested with OpenCV 3.2.0 and 4.4.0**.

## Eigen3
Required by g2o (see below). Download and install instructions can be found at: http://eigen.tuxfamily.org. **Required at least 3.1.0**.

## DBoW2 and g2o (Included in Thirdparty folder)
We use modified versions of the [DBoW2](https://github.com/dorian3d/DBoW2) library to perform place recognition and [g2o](https://github.com/RainerKuemmerle/g2o) library to perform non-linear optimizations. Both modified libraries (which are BSD) are included in the *Thirdparty* folder.

## EVO
We use [EVO](https://github.com/MichaelGrupp/evo) for Evaluation the RMS ATE of all sequences. Dowload and install instructions can be found at: https://github.com/MichaelGrupp/evo.

# 3. Building wMPS-Bearing library and examples

Clone the repository:
```
git clone https://github.com/TJU-IVCLab/wMPS_Bearing.git wMPS_Bearing
```

We provide a script `build.sh` to build the *Thirdparty* libraries and *wMPS-Bearing*. Please make sure you have installed all required dependencies (see section 2). Execute:
```
cd wMPS_Bearing
chmod +x build.sh
./build.sh
```

# 5. EuRoC Examples
[EuRoC dataset](http://projects.asl.ethz.ch/datasets/doku.php?id=kmavvisualinertialdatasets) was recorded with two pinhole cameras and an inertial sensor. We provide an example script to launch EuRoC sequences in all the sensor configurations.

1. Download a sequence (ASL format) from http://projects.asl.ethz.ch/datasets/doku.php?id=kmavvisualinertialdatasets

2. Run the system
./Examples/Stereo/stereo_euroc ./Vocabulary/ORBvoc.txt ./Examples/Stereo/wMPS.yaml ./Hand_Small_01 ./Examples/Stereo/wMPS_TimeStamps/Hand_Small_01.txt ./Examples/dataset-Hand_Small_01_Stereo ./Hand_Small_01/Bearing.txt

3. Evaluation
Execute the following script to process sequences and compute the RMS ATE:
evo_ape tum Hand_Small_01_GT.txt dataset-Hand_Small_01_Stereo.txt -va

## Acknowledgement
ORB_SLAM3 and the corresponding community.
