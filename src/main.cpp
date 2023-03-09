//
//  main.cpp
//  temporalgradient
//
//  Created by Anuj Shrivastav and Anush sriram Ramesh on 3/9/23.
//
// apply a Harris corner detector to find corners in two images, auto- matically find corresponding features, estimate a homography between the two images, and warp one image into the coordinate system of the second one to produce a mosaic containing the union of all pixels in the two images.

#include <iostream>
#include "../inc/cv_factory.hpp"
#include <string>

using namespace std;
using namespace cv;

int main(int argc, const char * argv[])
{
    // create a cv_factory object
    cv_factory cvf;
    // command line arguments to get path of 2 images
    string path1 = argv[1];
    string path2 = argv[2];
    // create a vector to store the directory of the images
    vector<string> directory;
    // add the directory of the images to
    directory.push_back(path1);
    directory.push_back(path2);
    // read the images from the directory
    vector<Mat> imgs = cvf.read_images(directory);

    // find the corners in the images
    Mat corners1 = cvf.find_corners(imgs[0]);
    Mat corners2 = cvf.find_corners(imgs[1]);

    // display the images
    imshow("Image 1", imgs[0]);
    imshow("Image 2", imgs[1]);
    imshow("Image 3", corners1);
    imshow("Image 4", corners2);
    waitKey(0);
}
