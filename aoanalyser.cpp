#include <vector>
//fftw and audiofile parser
#include <fftw3.h>
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
//pulse
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pulse/simple.h>
#include <pulse/error.h>
//#define BUFSIZE 512
//#define BUFSIZE 1024
//#define BUFSIZE 2048
#define BUFSIZE 4096
//GLUT - window
#include<GL/glut.h>
#include <GLFW/glfw3.h>
//matplotlib
#include "matplotlibcpp.h"
//time measure
#include <chrono>

//g++ aoanalyser.cpp -I/usr/include/python2.7 -I/home/daviddjh/.local/lib/python2.7/site-packages/numpy/core/include/ -lfftw3 -lm -lpulse-simple -lpulse -g -std=c++11 -lpython2.7 
// g++ aoanalyser.cpp -I/usr/include/python2.7 -I/home/daviddjh/.local/lib/python2.7/site-packages/numpy/core/include/ -lfftw3 -lfftw3f -lm -lpulse-simple -lpulse -g -std=c++11 -lpython2.7

using namespace std;

namespace plt = matplotlibcpp;

static ssize_t loop_write(int fd, const void*data, size_t size);

void setupGlut(int* argc, char** argv);

void Grender(void);

int main (int argc, char *argv[]){

    auto timestart = chrono::high_resolution_clock::now();

    //setupGlut(&argc, argv);
    GLFWwindow* window;
    float r, g, b = 0;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    r = .11;
    b = 0.6;
    g = 0.01;

    //fftw_init_threads();
    //fftwf_plan_with_nthreads(4);



    fftwf_complex *out;
    //double *in;
    float *in;
    fftwf_plan p;
    int ret;
    float buf[BUFSIZE];
    //float *buf;

    //fftw arrays
    //buf = (float*) fftw_malloc(sizeof(float) * BUFSIZE);
    //in = (float*) fftwf_malloc(sizeof(float) * BUFSIZE);
    in = (float*) fftwf_malloc(sizeof(float) * BUFSIZE);
    out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * BUFSIZE);

    //fftw plan - setup for the transform to be executed
    //p = fftw_plan_dft_1d(BUFSIZE, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    p = fftwf_plan_dft_r2c_1d(BUFSIZE, buf, out, FFTW_ESTIMATE);
    cout << fftwf_cost(p) << endl;


    /* The sample type to use */
    static const pa_sample_spec ss = {
        //.format = PA_SAMPLE_S16LE,
        //.format = PA_SAMPLE_U8, // unsigned, 8bit integer
        .format = PA_SAMPLE_FLOAT32, // unsigned, 8bit integer
        .rate = 44100,  //44100 hz
        .channels = 1
    };

    pa_simple *s = NULL;
    int error;


    /* Create the recording stream */
    if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, "alsa_output.pci-0000_00_1f.3.analog-stereo.monitor", "record", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        //goto finish;
    }

    //hann function
    /*
    for (int i = 0; i < 4000; i++) {
        double multiplier = 0.5 * (1 - cos(2*3.1415*i/3999));
        buff.push_back(multiplier * buff[i]);
    }
    */

    float bass, mid, high, midval;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
            auto time_start_loop = chrono::high_resolution_clock::now();

       // for (int j = 0; j < 1000; j++){
            auto time_start_buf = chrono::high_resolution_clock::now();
            cout << "sizeof(buff): " << sizeof(buf) << " BUFSIZE: " << BUFSIZE << 
              " buffsize * floatsize: " << sizeof(float) * BUFSIZE << endl;

            pa_simple_read(s, buf, sizeof(buf), &error);
            if (error)
              cout << error << endl;
            auto time_end_buf = chrono::high_resolution_clock::now();

            //pa_simple_read(s, in, sizeof(in), &error);

            //fill fftw array with audio buffer
            
            /*
            for(int i = 0; i < BUFSIZE; i++){
                //double multiplier = 0.5 * (1 - cos(2*3.1415*i/(BUFSIZE-1)));
                //in[i] = (multiplier * buf[i]);
                in[i] = buf[i];
            }
            */
            
            //in = buf;
            auto time_start_plan = chrono::high_resolution_clock::now();

            fftwf_execute(p);

            auto time_end_plan = chrono::high_resolution_clock::now();

            // Bass - 20 - 300      [1,7]   .1 = 35
            // Mid  - 301 - 1280    [8,29]  .1 = 105
            // High - 1281 - 10200  [30,233].1 = 1015
            /*
            bass = 0;
            mid = 0;
            high = 0;


            for(int i = 1; i < 234; i++){
                if(i <= 7){
                    bass += sqrt((out[i][0] * out[i][0]) + (out[i][1] * out[i][1])); 
                }
                if(i > 7 && i <= 29){
                    mid += sqrt((out[i][0] * out[i][0]) + (out[i][1] * out[i][1])); 
                }
                if(i > 29 && i <= 233){
                    high += sqrt((out[i][0] * out[i][0]) + (out[i][1] * out[i][1])); 
                }
            }   

            system("clear");
            cout << "Bass: " << pow((bass), 2) << endl;
            cout << "Mid:  " << pow((mid), 2) << endl;
            cout << "High: " << pow((high), 2) << endl << endl;
            if(pow(bass, 2) > 21000){
                cout << " +++ " << endl;
            }

        //}

        b = bass/60;
        g = mid/60;
        r = high/60;

        if(b < g < r || r < g < b){
            midval = g;
        } else if(g < b < r || r < b < g){
            midval = b;
        } else {
            midval = r;
        }

        b=4*pow((b-(midval)),3)+(midval);
        g=4*pow((g-(midval)),3)+(midval);
        r=4*pow((r-(midval)),3)+(midval);
        */

        auto time_start_render = chrono::high_resolution_clock::now();

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        //glColor3f(r, 0, b);
        //glColor3f(0, 0, b);
        //glColor3f(0, 0, b);
        float size;
        //glRectf(-1.0f, -1.0f, 1.0, 1.0);  //start pos 
        //glColor3f(r, 0.1, b);
        glColor3f(0, 0, 0.5);
        for(int k = 1; k < 200; k++){
            size = sqrt((out[k][0] * out[k][0]) + (out[k][1] * out[k][1]));
            //glRectf(((float)2*k/200)-1, -1.0f, (((float)2*k/200)-1+(0.005f)), (size/70)-1);  //start pos 
            glRectf(((float)2*k/200)-1, -1.0f, (((float)2*k/200)-1+(0.005f)), (pow(size/200, 2))-1);  //start pos 
            /*
            glRectf((2*k/233)-1, -1.0f,  //start pos 
                    (2*k/233)-1, size);   //end pos
                    *///
        }
        /*
        glBegin(GL_TRIANGLES);
            glVertex2f(-1, -1);
            glVertex2f(-1, 1);
            glVertex2f(1, -1);
            glVertex2f(1, 1);
        glEnd();
        */

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        auto time_end_render = chrono::high_resolution_clock::now();

        auto time_end_loop = chrono::high_resolution_clock::now();

        //Calc and print durations for debugging
        auto duration_loop = chrono::duration_cast<std::chrono::microseconds>( time_end_loop - time_start_loop ).count();
        auto duration_plan = chrono::duration_cast<std::chrono::microseconds>( time_end_plan - time_start_plan ).count();
        auto duration_render = chrono::duration_cast<std::chrono::microseconds>( time_end_render - time_start_render ).count();
        auto duration_buf= chrono::duration_cast<std::chrono::microseconds>( time_end_buf - time_start_buf ).count();

        cout << "Duration of loop: " << duration_loop <<  
          "    Duration of plan: " << duration_plan <<  
          "    Duration of render: " << duration_render <<  
          "    Duration of buff copy: " << duration_buf << endl;
        
    }

    glfwTerminate();

    /*
    
    //Plot data on matlibplot graph
    double hz = 0;
    vector<double> xplot;
    vector<double> yplot;
    cout << "Beginning output: " << endl;
    //for(int i = 1; i < BUFSIZE/2; i++){
    for(int i = 1; i < 234; i++){
        int mag = sqrt((out[i][0] * out[i][0]) + (out[i][1] * out[i][1]));
        hz = (double)(i * 44100) / BUFSIZE;

        cout << i << ": " << hz << ": " << mag << endl;
        yplot.push_back(mag);
        xplot.push_back(hz);

    }
    //plt::plot(xplot, yplot, "b-"); 
    plt::plot( xplot, yplot, "b-"); 
    plt::show();
    */

    ret = 0;
    //free(buf);
    fftwf_destroy_plan(p);
    fftwf_free(out);


finish:
    if (s)
        pa_simple_free(s);
    return ret;
    
}

static ssize_t loop_write(int fd, const void*data, size_t size) {
    ssize_t ret = 0;
    while (size > 0) {
        ssize_t r;
        if ((r = write(fd, data, size)) < 0)
            return r;
        if (r == 0)
            break;
        ret += r;
        data = (const uint8_t*) data + r;
        size -= (size_t) r;
    }
    return ret;
}

void setupGlut(int* argc, char** argv){
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(640,480);
    glutCreateWindow("Simple GLut");

    glutDisplayFunc(Grender);

    glutMainLoop();
}

void Grender(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_TRIANGLES);
        glColor3f(1, 0, 0);
        glVertex2f(-0.5, -0.5);
        glColor3f(0, 1, 0);
        glVertex2f(0.5, -0.5);
        glColor3f(0, 0, 1);
        glVertex2f(0.0, 0.5);
    glEnd();

    glutSwapBuffers();

}
