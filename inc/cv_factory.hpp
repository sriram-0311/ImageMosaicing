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
            cout<<"read_images"<<endl;
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
        tuple<vector<Point>, Mat> find_corners(Mat img) {
            cout<<"find_corners"<<endl;
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
            //cout<<"find_corners"<<endl;
            //cout<<"Number of corners: "<<corners.size()<<endl;
            // return the corners vector and the image with the corners marked
            tuple<vector<Point>, Mat> corners_img;
            corners_img = make_tuple(corners, dst_norm_scaled);
            return corners_img;
        }

        // function that does the normalized cross-correlation
        double NCC(Mat t1, Mat t2)
        {   //cout << ">>NCC" << endl;
            // calculate the mean of the template 1
            double mean1 = 0;
            for(int i=0; i<t1.rows; i++)
            {
                for(int j=0; j<t1.cols; j++)
                {
                    mean1 += t1.at<uchar>(i,j);
                }
            }
            mean1 = mean1/(t1.rows*t1.cols);
            // calculate the mean of the template 2
            double mean2 = 0;
            for(int i=0; i<t2.rows; i++)
            {
                for(int j=0; j<t2.cols; j++)
                {
                    mean2 += t2.at<uchar>(i,j);
                }
            }
            mean2 = mean2/(t2.rows*t2.cols);
            // calculate the standard deviation of the template 1
            double std1 = 0;
            for(int i=0; i<t1.rows; i++)
            {
                for(int j=0; j<t1.cols; j++)
                {
                    std1 += pow(t1.at<uchar>(i,j) - mean1, 2);
                }
            }
            std1 = sqrt(std1/(t1.rows*t1.cols));
            // calculate the standard deviation of the template 2
            double std2 = 0;
            for(int i=0; i<t2.rows; i++)
            {
                for(int j=0; j<t2.cols; j++)
                {
                    std2 += pow(t2.at<uchar>(i,j) - mean2, 2);
                }
            }
            std2 = sqrt(std2/(t2.rows*t2.cols));
            // calculate the normalized cross-correlation
            double ncc = 0;
            for(int i=0; i<t1.rows; i++)
            {
                for(int j=0; j<t1.cols; j++)
                {
                    ncc += (t1.at<uchar>(i,j) - mean1)*(t2.at<uchar>(i,j) - mean2);
                }
            }
            ncc = ncc/(t1.rows*t1.cols*std1*std2);
            return ncc;
        }

        // find correspondences in the image using the corner points
        vector<pair<Point, Point>> find_correspondences(Mat img1, Mat img2, vector<Point> corners1, vector<Point> corners2)
        {   cout<<"find_correspondences"<<endl;
            // convert the images to grayscale
            // cvtColor(img1, img1, COLOR_BGR2GRAY);
            // cvtColor(img2, img2, COLOR_BGR2GRAY);
            Mat template1, template2;
            double Threshold = 0.8;
            Point p2 = Point(0,0);
            // initialize a vector to store the correspondences as a pair of points
            vector<pair<Point,Point>> corres;
            //cout << corners1.size() << endl;
            for(int i=0; i<corners1.size(); i++)
            {
                // check if the template is within the image
                //cout<<"Creating templates"<<endl;
                int WindowSize = 21;
                if(corners1[i].x - WindowSize/2 <= 0 || corners1[i].x + WindowSize/2 >= img1.cols || corners1[i].y - WindowSize/2 <= 0 || corners1[i].y + WindowSize/2 >= img1.rows)
                    continue;
                // create a roi around the corner point
                cv::Rect roi(corners1[i].x - WindowSize/2, corners1[i].y - WindowSize/2, WindowSize, WindowSize);
                template1 = img1(roi);
                //cout<<corners2.size()<<endl;
                p2 = Point(0,0);
                double currentMax = 0;
                for(int j=0; j<corners2.size(); j++)
                {
                    //cout<<"Creating template 2"<<endl;
                    // check if the template is within the image
                    if(corners2[j].x - WindowSize/2 <= 0 || corners2[j].x + WindowSize/2 >= img2.cols || corners2[j].y - WindowSize/2 <= 0 || corners2[j].y + WindowSize/2 >= img2.rows)
                        continue;
                    cv::Rect roi2(corners2[j].x - WindowSize/2, corners2[j].y - WindowSize/2, WindowSize, WindowSize);
                    template2 = img2(roi2);
                    double value = NCC(template1, template2);
                    //cout << value << endl;
                    if(value > Threshold && value > currentMax){
                        currentMax = value;
                        p2 = corners2[j];
                    }
                }
                // push the pair of points into the vector
                if (p2 != Point(0,0))
                {pair<Point, Point> p;
                p.first = corners1[i];
                p.second = p2;
                corres.push_back(p);}
            }
            return corres;
        }

        // find the homography matrix using the correspondences and the corner points
        Mat find_homography(vector<Point> corners1, vector<Point> corners2, vector<Point> corres)
        {   Mat A = Mat::zeros(2*corres.size(), 9, CV_32F);
            for(int i=0; i<corres.size(); i++)
            {   A.at<float>(2*i, 0) = corners1[corres[i].x].x;
                A.at<float>(2*i, 1) = corners1[corres[i].x].y;
                A.at<float>(2*i, 2) = 1;
                A.at<float>(2*i, 6) = -corners1[corres[i].x].x*corners2[corres[i].y].x;
                A.at<float>(2*i, 7) = -corners1[corres[i].x].y*corners2[corres[i].y].x;
                A.at<float>(2*i, 8) = -corners2[corres[i].y].x;
                A.at<float>(2*i+1, 3) = corners1[corres[i].x].x;
                A.at<float>(2*i+1, 4) = corners1[corres[i].x].y;
                A.at<float>(2*i+1, 5) = 1;
                A.at<float>(2*i+1, 6) = -corners1[corres[i].x].x*corners2[corres[i].y].y;
                A.at<float>(2*i+1, 7) = -corners1[corres[i].x].y*corners2[corres[i].y].y;
                A.at<float>(2*i+1, 8) = -corners2[corres[i].y].y;
            }
            Mat u, w, vt;
            SVD::compute(A, w, u, vt);
            Mat H = vt.row(8).reshape(0, 3);
            return H;
        }

        // draw lines between the corresponding points in the two images and return single with two input images one above the other
        Mat draw_lines(Mat img1, Mat img2, vector<pair<Point,Point>> correspondingPoints)
        {
            // create a new image with size of the two input images one top of another
            Mat img3 = Mat::zeros(img1.rows + img2.rows, img1.cols, img1.type());
            // copy the two input images into the new image
            img1.copyTo(img3(Rect(0, 0, img1.cols, img1.rows)));
            img2.copyTo(img3(Rect(0, img1.rows, img2.cols, img2.rows)));
            // mark the corresponding points in the two images
            for(int i=0; i<correspondingPoints.size(); i++)
            {
                // draw a circle at the corresponding points in img1
                circle(img3, correspondingPoints[i].first, 3, Scalar(0, 0, 255), 2, 8, 0);
                // draw a circle at the corresponding points in img2
                circle(img3, Point(correspondingPoints[i].second.x, correspondingPoints[i].second.y + img1.rows), 3, Scalar(0, 0, 255), 2, 8, 0);
            }
            // draw lines between the corresponding points
            for(int i=0; i<correspondingPoints.size(); i++)
            {
                line(img3, correspondingPoints[i].first, Point(correspondingPoints[i].second.x, correspondingPoints[i].second.y + img1.rows), Scalar(0, 255, 0), 1, 8, 0);
            }
            return img3;
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
