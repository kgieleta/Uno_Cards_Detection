# UNO Card Detection in Images  

This project was carried out as part of the **Digital Image Processing** course.  
The goal was to develop an algorithm for detecting and recognizing UNO cards in images affected by various types of noise (blur, gradient, salt-and-pepper), including determining the center of special cards and summing the numerical values of the remaining cards. Additionally, the user can choose whether to process all cards or only those of a specific color. 

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

## Results

### detection goal: all cards
<img width="730" height="587" alt="all" src="https://github.com/user-attachments/assets/e0b88e67-7501-4f59-b1c3-0304272db1ea" />

### detection goal: only yellow cards
<img width="727" height="650" alt="yellow" src="https://github.com/user-attachments/assets/80c29fce-0a2a-47fc-8c6e-749f8bf06c9f" />

### blured image, detection goal: all cards
<img width="712" height="788" alt="all_b" src="https://github.com/user-attachments/assets/827cb47d-7cde-4733-89fa-caa9651add3b" />

### salt and pepper on image, detection goal: all cards
<img width="700" height="634" alt="all_s" src="https://github.com/user-attachments/assets/1629b971-2fd3-494b-9c2b-f69181542ebe" />


