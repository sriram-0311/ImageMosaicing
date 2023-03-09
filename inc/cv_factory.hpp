//
//  cv_factory.hpp
//  temporalgradient
//
//  Created by Anuj Shrivatsav and Anush sriram Ramesh on 3/9/23.
//
// include the header files needed to perform cv operations and read and writing file directories

#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <numeric>
#include <filesystem>
#include <math.h>

using namespace std;
using namespace cv;

// define class to perform cv operations
class cv_factory {
    public:
        // function to read all image files in the directory and return a vector
        vector<Mat> read_images(string directory) {
            // create a vector to store the images
            vector<Mat> imgs;
            //generate the files names from 1 to 1070 and read the images in grayscale
            for (int i = 1; i<=1070;i++) {
                int NumOfZeroes = 4 - to_string(i).length();
                string ZeroString = "";
                for (int j = 0; j < NumOfZeroes; j++) {
                    ZeroString += "0";
                }
                string filename = directory + "img01_" + ZeroString + to_string(i) + ".jpg";
                Mat photos = imread(filename, IMREAD_GRAYSCALE);
                imgs.push_back(photos);
            }
            return imgs;
        }

        // function to threshold the temporal gradient image to create a mask of the moving objects
        Mat thresholding(Mat temporal_gradient, int threshold_value) {
            // threshold the result to create a mask of the moving objects
            Mat mask;
            threshold(temporal_gradient, mask, threshold_value, 255, THRESH_BINARY);
            return mask;
        }

        // Strategy to threshold the images after passing through a prewitt mask and using those images to find
        // the std deviation that will be our new threshold to be used for final thresholding and getting the mask
        double ThreshStrategy(vector<Mat> imgs)
        {
            vector<double> stdevs;
            Scalar mean, std;
            vector<Mat> processed_imgs;
            for(int i=0; i<imgs.size()-1; i++)
            {
                Mat temp;
                temp = abs((-imgs[i] + imgs[i + 1]));
                meanStdDev(temp, mean, std);
                stdevs.push_back(std[0]);
            }
            double deviation  = std::accumulate(stdevs.begin(), stdevs.end(), 0) / stdevs.size();
            cout << deviation;
            return(deviation);
        }

};
