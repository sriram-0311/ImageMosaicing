# ImageMosaicing
Apply a Harris corner detector to find corners in two images, automatically find corresponding features, estimate a homography between the two images, and warp one image into the coordinate system of the second one to produce a mosaic containing the union of all pixels in the two images.

## Usage
Clone the repository and run the following command in the root directory:
```
mkdir build && cd build && cmake .. && make
```

Place the two images you want to mosaic in respective folders named "DanaHallway1" and "DanaOffice" in the buil directory.

Run the binary with the following command:
```
./ImageMosaicing
```