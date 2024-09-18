/**
* This file is part of ORB-SLAM3
*
* Copyright (C) 2017-2021 Carlos Campos, Richard Elvira, Juan J. Gómez Rodríguez, José M.M. Montiel and Juan D. Tardós, University of Zaragoza.
* Copyright (C) 2014-2016 Raúl Mur-Artal, José M.M. Montiel and Juan D. Tardós, University of Zaragoza.
*
* ORB-SLAM3 is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-SLAM3 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
* the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with ORB-SLAM3.
* If not, see <http://www.gnu.org/licenses/>.
*/

#include<iostream>
#include<algorithm>
#include<fstream>
#include<iomanip>
#include<chrono>

#include<opencv2/core/core.hpp>

#include<System.h>

using namespace std;

void LoadImages(const string &strPathLeft, const string &strPathRight, const string &strPathTimes,
                vector<string> &vstrImageLeft, vector<string> &vstrImageRight, vector<double> &vTimeStamps);

int LoadBearing(vector<double> &wMPSvTimestamps,wMPS::Bearing* wMPSBearing,const string &strSettingsFile);

int main(int argc, char **argv)
{  
    if(argc < 7)
    {
        cerr << endl << "Usage: ./stereo_euroc path_to_vocabulary path_to_settings path_to_sequence_folder_1 path_to_times_file_1 (path_to_image_folder_2 path_to_times_file_2 ... path_to_image_folder_N path_to_times_file_N) (trajectory_file_name) (wMPSBearing_file_name)" << endl;
        return 1;
    }

    const int num_seq = (argc-4)/2;
    cout << "num_seq = " << num_seq << endl;
    bool bFileName= (((argc-4) % 2) == 1);

    string file_name;
    if (bFileName)
    {
        file_name = string(argv[argc-2]);
        cout << "file name: " << file_name << endl;
    }

    // Load all sequences:
    int seq;
    vector< vector<string> > vstrImageLeft;
    vector< vector<string> > vstrImageRight;
    vector< vector<double> > vTimestampsCam;
    vector<int> nImages;

    vstrImageLeft.resize(num_seq);
    vstrImageRight.resize(num_seq);
    vTimestampsCam.resize(num_seq);
    nImages.resize(num_seq);

    int tot_images = 0;
    for (seq = 0; seq<num_seq; seq++)
    {
        cout << "Loading images for sequence " << seq << "...";

        string pathSeq(argv[(2*seq) + 3]);
        string pathTimeStamps(argv[(2*seq) + 4]);

        string pathCam0 = pathSeq + "/mav0/cam0/data";
        string pathCam1 = pathSeq + "/mav0/cam1/data";

        LoadImages(pathCam0, pathCam1, pathTimeStamps, vstrImageLeft[seq], vstrImageRight[seq], vTimestampsCam[seq]);
        cout << "LOADED!" << endl;

        nImages[seq] = vstrImageLeft[seq].size();
        tot_images += nImages[seq];
    }

    wMPS::Bearing* wMPSBearing = new wMPS::Bearing[200000];
    vector<double> wMPSvTimestamps;
    int ret = LoadBearing(wMPSvTimestamps,wMPSBearing,string(argv[argc-1]));
    if(1 == ret)
    {
        cout << "Read wMPS Bearing Data File fails";
        return 1;
    }
    else if(2 == ret)
    {
        cout<< "Read wMPSDataFile fails";
        return 1;
    }

    // Vector for tracking time statistics
    vector<float> vTimesTrack;
    vTimesTrack.resize(tot_images);

    cout << endl << "-------" << endl;
    cout.precision(17);

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM3::System SLAM(argv[1],argv[2],ORB_SLAM3::System::STEREO, true);
    //注意，已关闭回环检测线程

    if(!wMPSvTimestamps.empty())
        SLAM.LoadwMPSBearing(wMPSvTimestamps,wMPSBearing);

    const string SaveFileName =  string(argv[argc-2]) + ".txt";
    SLAM.SaveTrackTrajectory(SaveFileName);

    cv::Mat imLeft, imRight;
    for (seq = 0; seq<num_seq; seq++)
    {
        // Seq loop
        double t_resize = 0;
        double t_rect = 0;
        double t_track = 0;
        int num_rect = 0;
        int proccIm = 0;
        for(int ni=0; ni<nImages[seq]; ni++, proccIm++)
        {
            // Read left and right images from file
            imLeft = cv::imread(vstrImageLeft[seq][ni],cv::IMREAD_UNCHANGED); //,cv::IMREAD_UNCHANGED);
            imRight = cv::imread(vstrImageRight[seq][ni],cv::IMREAD_UNCHANGED); //,cv::IMREAD_UNCHANGED);
            if(imLeft.empty())
            {
                cerr << endl << "Failed to load image at: "
                     << string(vstrImageLeft[seq][ni]) << endl;
                return 1;
            }

            if(imRight.empty())
            {
                cerr << endl << "Failed to load image at: "
                     << string(vstrImageRight[seq][ni]) << endl;
                return 1;
            }

            double tframe = vTimestampsCam[seq][ni];

    #ifdef COMPILEDWITHC11
            std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    #else
            std::chrono::monotonic_clock::time_point t1 = std::chrono::monotonic_clock::now();
    #endif

            // Pass the images to the SLAM system
            SLAM.TrackStereo(imLeft,imRight,tframe, vector<ORB_SLAM3::IMU::Point>(), vstrImageLeft[seq][ni]);

    #ifdef COMPILEDWITHC11
            std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    #else
            std::chrono::monotonic_clock::time_point t2 = std::chrono::monotonic_clock::now();
    #endif

#ifdef REGISTER_TIMES
            t_track = t_resize + t_rect + std::chrono::duration_cast<std::chrono::duration<double,std::milli> >(t2 - t1).count();
            SLAM.InsertTrackTime(t_track);
#endif

            double ttrack= std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1).count();

            vTimesTrack[ni]=ttrack;

            // Wait to load the next frame
            double T=0;
            if(ni<nImages[seq]-1)
                T = vTimestampsCam[seq][ni+1]-tframe;
            else if(ni>0)
                T = tframe-vTimestampsCam[seq][ni-1];

            if(ttrack<T)
                usleep((T-ttrack)*1e6); // 1e6
        }

        if(seq < num_seq - 1)
        {
            cout << "Changing the dataset" << endl;

            SLAM.ChangeDataset();
        }

    }
    // Stop all threads
    SLAM.Shutdown();

    // Save camera trajectory
    //if (bFileName)
    //{
    //    const string kf_file =  "kf_" + string(argv[argc-2]) + ".txt";
    //    const string f_file =  "f_" + string(argv[argc-2]) + ".txt";
    //    SLAM.SaveTrajectoryEuRoC(f_file);
    //    SLAM.SaveKeyFrameTrajectoryEuRoC(kf_file);
    //}
    //else
    //{
    //    SLAM.SaveTrajectoryEuRoC("CameraTrajectory.txt");
    //    SLAM.SaveKeyFrameTrajectoryEuRoC("KeyFrameTrajectory.txt");
    //}

    return 0;
}

void LoadImages(const string &strPathLeft, const string &strPathRight, const string &strPathTimes,
                vector<string> &vstrImageLeft, vector<string> &vstrImageRight, vector<double> &vTimeStamps)
{
    ifstream fTimes;
    fTimes.open(strPathTimes.c_str());
    vTimeStamps.reserve(5000);
    vstrImageLeft.reserve(5000);
    vstrImageRight.reserve(5000);
    while(!fTimes.eof())
    {
        string s;
        getline(fTimes,s);
        if(!s.empty())
        {
            stringstream ss;
            ss << s;
            vstrImageLeft.push_back(strPathLeft + "/" + ss.str() + ".png");
            vstrImageRight.push_back(strPathRight + "/" + ss.str() + ".png");
            double t;
            ss >> t;
            vTimeStamps.push_back(t/1e9);

        }
    }
}

int LoadBearing(vector<double> &wMPSvTimestamps,wMPS::Bearing* wMPSBearing,const string &strSettingsFile)
{
    cout << "wMPS Data FilePath: " << strSettingsFile << endl;
    ifstream fTimes;
    fTimes.open(strSettingsFile);
    if(!fTimes)
        return 1;

    long index = 0;
    while(!fTimes.eof()) {
        string s;
        getline(fTimes, s);
        if(s[0] == '#')
            continue;

        if (!s.empty())
        {
            stringstream ss;
            ss << s;

            double trm, t, d1, d2, d3, a1, a2;  // Frame
            int ch;
            ss >> ch; ss>> trm; ss >> t;  ss>>d1; ss >> d2; ss >> d3; ss>> a1; ss>>a2;
            wMPSBearing[index].channel = ch;
            wMPSBearing[index].transmitter = trm;
            wMPSBearing[index].time = t;
            wMPSBearing[index].Bearing = Eigen::Vector3d(d1,d2,d3);
            wMPSBearing[index].Angle = Eigen::Vector2d(a1,a2);
            wMPSvTimestamps.push_back(t);
        }
        index++;
    }

    return 0;
}