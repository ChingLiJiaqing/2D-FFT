// Distributed two-dimensional Discrete FFT transform
// Jiaqing Li

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>
#include <thread>

#include "Complex.h"
#include "InputImage.h"

constexpr unsigned int NUMTHREADS = 4;
const double pi = 3.1415926;

using namespace std;

//undergrad students can assume NUMTHREADS will evenly divide the number of rows in tested images
//graduate students should assume NUMTHREADS will not always evenly divide the number of rows in tested images.
// I will test with a different image than the one given

//void Transform1D(Complex* h, int w, Complex* H)
void Transform1D(Complex* h, int w, Complex* H, int threadNum, int dftNum)
{
    // Implement a simple 1-d DFT using the double summation equation
    // given in the assignment handout.  h is the time-domain input
    // data, w is the width (N), and H is the output array.

    //Complex W = Complex(cos(2*pi/w), -sin(2*pi/w));
    //h = w*h/NUMTHREADs
    //H = w/NUMTHREAD
    Complex sum(0,0);
    // rows: number of rows, w*h/w/NUMTHREADS
    int rows = w/NUMTHREADS;
    cout << "rows = " << (threadNum+1)*rows << endl;
    //for each row calculate the W*row(size w) = H with size of w
    for(int r = threadNum * rows; r < (threadNum+1)*rows; r++) {
        //H has size of w
        // a row of h has size of w
        for(int ni = 0; ni < w; ni++){
            for(int c = r*w; c < (r+1)*w; c++){
                Complex W = Complex(cos(2*pi*c*ni/w), -sin(2*pi*c*ni/w)*dftNum);
                sum = sum + W * h[c];
            }
            H[r * w + ni] = sum;
            sum = Complex(0,0);
        }
    }
}

void complexTranspose(Complex* arr, Complex* arr_tran, int w){
    int k = 0;
    for(int r = 0; r < w; r++){
        for(int c = 0; c< w; c++){
            arr_tran[k] = arr[c*w + r];
            k++;
        }
    }
    cout << "k = " << k << endl;
}


void Transform2D(const char* inputFN) 
{ // Do the 2D transform here.
  // 1) Use the InputImage object to read in the Tower.txt file and
  //    find the width/height of the input image.
    InputImage img = InputImage(inputFN);
    int w = img.GetWidth();
    int h = img.GetHeight();
    cout << h << endl;
    cout << w << endl;
  // 2) Create a vector of complex objects of size width * height to hold
  //    values calculated
    Complex* vec1d = new Complex[h*w];
    Complex* vec2d = new Complex[h*w];
//    cout << "size of vec2d is " << vec1d.size() << endl;
    Complex* data = img.GetImageData();
    // 3) Do the individual 1D transforms on the rows assigned to each thread
    int group = h*w/4;
    cout << "each group has size " << group << endl;
    thread t1(Transform1D, data, w, vec1d, 0, 1);
    thread t2(Transform1D, data, w, vec1d, 1, 1);
    thread t3(Transform1D, data, w, vec1d, 2, 1);
    thread t4(Transform1D, data, w, vec1d , 3, 1);

  // 4) Force each thread to wait until all threads have completed their row calculations
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    Complex* vec1dTrans = new Complex[h*w];
    Complex* vec2dTrans = new Complex[h*w];
    complexTranspose(vec1d, vec1dTrans, w);
  //    prior to starting column calculations
  // 5) Perform column calculations
    thread t5(Transform1D, vec1dTrans, w, vec2dTrans, 0, 1);
    thread t6(Transform1D, vec1dTrans, w, vec2dTrans, 1, 1);
    thread t7(Transform1D, vec1dTrans, w, vec2dTrans, 2, 1);
    thread t8(Transform1D, vec1dTrans, w, vec2dTrans, 3, 1);

  // 6) Wait for all column calculations to complete
    t5.join();
    t6.join();
    t7.join();
    t8.join();
    complexTranspose(vec2dTrans, vec2d, w);
//  // 7) Use SaveImageData() to output the final results
    img.SaveImageData("../after1d_ori.txt", vec1d, w, h);
    img.SaveImageData("../after2d_ors.txt", vec2d, w, h);

//    // 8) idft
//    vector<Complex> ivec1d(h*w);
//    vector<Complex> ivec2d(h*w);
//    thread t9(Transform1D, &vec2d[0], w, &ivec2d[0], 0, -1);
//    thread t10(Transform1D, &vec2d[0], w, &ivec2d[0], 1, -1);
//    thread t11(Transform1D, &vec2d[0], w, &ivec2d[0], 2, -1);
//    thread t12(Transform1D, &vec2d[0], w, &ivec2d[0], 3, -1);
//    t9.join();
//    t10.join();
//    t11.join();
//    t12.join();
//
//    thread t13(Transform1D, &ivec2d[0], w, &ivec1d[0], 0, -1);
//    thread t14(Transform1D, &ivec2d[0], w, &ivec1d[0], 1, -1);
//    thread t15(Transform1D, &ivec2d[0], w, &ivec1d[0], 2, -1);
//    thread t16(Transform1D, &ivec2d[0], w, &ivec1d[0], 3, -1);
//    t13.join();
//    t14.join();
//    t15.join();
//    t16.join();

    //img.SaveImageData("../after1d_ori.txt", vec1d, w, h);

    delete[] vec1d;
    delete[] vec1dTrans;
    delete[] vec2d;
    delete[] vec2dTrans;
}



int main(int argc, char** argv)
{
  string fn("../Tower.txt"); // default file name
  if (argc > 1) fn = string(argv[1]);  // if name specified on cmd line
  Transform2D(fn.c_str()); // Perform the transform.

}  
  

  
