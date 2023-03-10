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
        vector<Mat> read_images(vector<string> directory) {
            // create a vector to store the images
            vector<Mat> imgs;
            //read images from the directory vector and store them in the imgs vector
            Mat img = imread(directory[0]);
            //scale down the image to 1/4th of its original size
            resize(img, img, Size(), 0.75, 0.75);
            imgs.push_back(img);
            img = imread(directory[1]);
            resize(img, img, Size(), 0.75, 0.75);
            imgs.push_back(img);
            return imgs;
        }

        // find corners in the image using the harris corner detector
        Mat find_corners(Mat img) {
            // convert the image to grayscale
            Mat gray;
            cvtColor(img, gray, COLOR_BGR2GRAY);
            // apply the harris corner detector
            Mat dst, dst_norm, dst_norm_scaled;
            dst = Mat::zeros(gray.size(), CV_32FC1);
            // compute the harris R matrix over the image
            cornerHarris(gray, dst, 5, 5, 0.04, BORDER_DEFAULT);
            // normalize the R matrix
            normalize(dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
            convertScaleAbs(dst_norm, dst_norm_scaled);
            // threshold the R matrix to find the corners
            // push corner points into a vector
            vector<Point> corners;
            for(int i = 0; i < dst_norm.rows; i++) {
                for(int j = 0; j < dst_norm.cols; j++) {
                    if((int)dst_norm.at<float>(i,j) > 120) {
                        circle(dst_norm_scaled, Point(j,i), 5, Scalar(0), 2, 8, 0);
                        corners.push_back(Point(j,i));
                    }
                }
            }
            return (corners, dst_norm_scaled);
        }

        // function that does the normalized cross-correlation
        Scalar NCC(Mat t1, Mat t2)
        {   // mean((t1 - mean(t1))*(t2 - mean(t2)))
            Scalar mean1, mean2;
            Scalar stds1, stds2;
            meanStdDev(t1, mean1, stds1);
            meanStdDev(t2, mean2, stds2);
            Scalar value = mean((t1 - mean1)*(t2 - mean2))/(stds1*stds2);
            return value;
        }

        // find correspondences in the image using the corner points
        vector<Point> find_correspondences(Mat img1, Mat img2, vector<Point> corners1, vector<Point> corners2)
        {   Mat template1, template2;
            Scalar Threshold = 0;
            int p2;
            vector<Point> corres;
            for(int i=0; i<corners1.size(); i++)
            {   template1 = img1(cv::Rect(i,i,i+3,i+3));
                for(int j=0; j<corners2.size(); j++)
                {   template2 = img2(cv::Rect(i,i,i+3,i+3));
                    Scalar value = NCC(template1, template2);
                    if(value.val[0] > Threshold.val[0])
                        {Threshold = value;
                        p2 = j;}
                }
                corres.push_back(Point(i, p2));
            }
            return(corres);
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
