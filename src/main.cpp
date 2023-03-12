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
    // convert images to grayscale

    //vector<Point> corners1, corners2;
    Mat dst1, dst2;

    // find the corners in the images
    tuple<vector<Point>, Mat> corners1 = cvf.find_corners(imgs[0]);
    // take the corner vector from the tuple
    vector<Point> corners1_vec = get<0>(corners1);
    dst1 = get<1>(corners1);
    tuple<vector<Point>, Mat> corners2 = cvf.find_corners(imgs[1]);
    //cout<<corners1_vec.size()<<endl;
    vector<Point> corners2_vec = get<0>(corners2);
    dst2 = get<1>(corners2);

    vector<pair<Point, Point>> corres = cvf.find_correspondences(imgs[0], imgs[1], corners1_vec, corners2_vec);

    //print size of corres
    cout << "Size of corres: " << corres.size() << endl;

    // draw the correspondences
    // Mat outputImage = cvf.draw_lines(imgs[0], imgs[1], corres);

    // RANSAC to find the homography
    vector<pair<Point, Point>> BestCorres;
    Mat BestH = cvf.RANSAC(corres, BestCorres);


    Mat outputImage = cvf.draw_lines(imgs[0], imgs[1], BestCorres);

    // warp the image
    Mat warpedImage = cvf.warpImage(imgs[0], imgs[1], BestH);

    // display the images
    imshow("Image 1", imgs[0]);
    imshow("Image 2", imgs[1]);
    imshow("Image 3", dst1);
    imshow("Image 4", dst2);
    imshow("Image 5", outputImage);
    imshow("Image 6", warpedImage);
    waitKey(0);
}
