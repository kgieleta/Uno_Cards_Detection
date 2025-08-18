# UNO Card Detection in Images  

This project was carried out as part of the **Digital Image Processing (CPO)** course.  
The goal was to develop an algorithm for detecting and recognizing UNO cards in images affected by various types of noise (blur, gradient, salt-and-pepper), including determining the center of special cards and summing the numerical values of the remaining cards.  

## Features  

- Load an image from a user-provided path  
- Preprocessing: median blur → grayscale conversion → Gaussian blur → adaptive thresholding → morphology (dilation, erosion)  
- Card segmentation based on external contours  
- ROI extraction for shape and color detection  
- Card color detection (red, green, blue, yellow)  
- Shape detection using:  
  - number of circles (HoughCircles)  
  - contour circularity  
  - contour area  
- Symbol classification: `2`, `3`, `4`, `reverse`, `stop`  
- Calculation of the sum of numeric cards by color  
- Printing the coordinates of the card center for special cards (`stop`, `reverse`)  
- Support for 4 scenarios: no noise, blur, gradient, and salt-and-pepper (16 images in total)  

## Technologies  

- C++  
- OpenCV  
