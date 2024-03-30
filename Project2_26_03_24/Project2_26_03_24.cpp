
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <vector>
cv::RNG rng(12345);


void print_Contours(cv::Mat& frame, cv::Mat& contour_frame) {

   cv::Mat  gauss, edges, gray;

   cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
   cv::GaussianBlur(gray, gauss, cv::Size(3, 3), 0);
   cv::Canny(gauss, edges, 40, 200);

   std::vector<std::vector<cv::Point> > contours;

   std::vector<cv::Vec4i> hierarchy;
   findContours(edges, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

   for (size_t i = 0; i < contours.size(); i++) {

      cv::Scalar color = cv::Scalar(0, 225 - (contours.size()-i)*5, 0);

      drawContours(contour_frame, contours, (int)i, color, 1, cv::LINE_8, hierarchy, 0);

   }
}

void print_Contours_and_Text(cv::Mat& frame, cv::Mat& contour_frame) {

   cv::Mat  gauss, edges, gray;

   cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
   cv::GaussianBlur(gray, gauss, cv::Size(3, 3), 0);
   cv::Canny(gauss, edges, 40, 200);

   std::vector<std::vector<cv::Point> > contours;

   std::vector<cv::Vec4i> hierarchy;
   findContours(edges, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

   for (size_t i = 0; i < contours.size(); i++) {
      double epsilon = 0.01 * arcLength(contours[i], true);
      std::vector<cv::Point> approx;
      approxPolyDP(contours[i], approx, epsilon, true);

      cv::Scalar color = cv::Scalar(0, 255, 0);

      drawContours(contour_frame, contours, (int)i, color, 2, cv::LINE_8, hierarchy, 0);


      cv::Moments M = moments(approx);
      cv::Point center(M.m10 / M.m00, M.m01 / M.m00);
      if (approx.size() == 4) {
         putText(contour_frame, "Rectangle", center, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
      }

   }
}

int main()
{
   setlocale(LC_ALL, "Russian");

   std::string A1 = "D:/virandfpc/VID_20240326_103206.mp4";
   std::string A2 = "D:/virandfpc/VID_20240326_103227.mp4";

   cv::VideoCapture cap1(A1);
   //cv::VideoCapture cap1(A2);

   //cv::VideoWriter video("output_second.mp4", cv::VideoWriter::fourcc('a', 'v', 'c', '1'), 30, cv::Size(320, 240));
   cv::VideoWriter video("output_first.mp4", cv::VideoWriter::fourcc('a', 'v', 'c', '1'), 30, cv::Size(320, 240));
   if (!video.isOpened()) {
      std::cout << "Error: could not open video writer" << std::endl;
      return -1;
   }

   if (!cap1.isOpened()) {
      std::cout << "Ошибка загрузки первого видео" << std::endl;
      return -1;
   }
   std::vector<cv::Mat> vect_frame;

   while (true) {
      cv::Mat frame;
      cap1 >> frame;
      cv::Mat contour_frame = frame.clone();
      if (frame.empty()) break;
     
      if (vect_frame.size() > 5) {
         vect_frame.erase(vect_frame.begin());
      }

      for (int i = 0; i < vect_frame.size(); i++) {

         cv::Mat temp = vect_frame[i];
         print_Contours(temp, contour_frame);
      }

      vect_frame.push_back(frame);

      print_Contours_and_Text(frame, contour_frame);

      imshow("result", contour_frame);

      cv::resize(contour_frame, contour_frame, cv::Size(320, 240));
      video << contour_frame;

      char c = (char) cv::waitKey(30);
      if (c == 27) break;

   };

   video.release();

   cap1.release();
   cv::destroyAllWindows();
   return 0;
}