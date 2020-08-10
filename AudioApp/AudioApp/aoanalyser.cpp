//STD
#include <iostream>
#include <thread>
#include <mutex>
#include <cmath>

//fftw and audiofile parser
#include <fftw3.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>

//GLFW - window
#include <GLFW/glfw3.h>

//time measure (std)
#include <chrono>

//Windows Audio Sink
#include "StreamCapEx.h"

//Windows api
#include "audioclient.h"
#include "mmdeviceapi.h"

//#define BUFSIZE 512
//#define BUFSIZE 1024
//#define BUFSIZE 2048
//#define BUFSIZE 4096
#define BUFFSIZE 400

//haha ita me!

std::mutex mtx;                   // mutex for sound and copying buffer

int main(int argc, char* argv[]) {

    auto timestart = std::chrono::high_resolution_clock::now();

    // Setup GLFW
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


    //return var for main func
    int ret;

    // init Audio Sink
    MyAudioSink* pMySink = new MyAudioSink;
    pMySink->DoneRecording = false;

    // init fftw plan and output array
    fftwf_complex* out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * BUFFSIZE);
    fftwf_plan p = fftwf_plan_dft_r2c_1d(400, pMySink->SoundBuffer, out, 0);

    //hann function
    /*
    for (int i = 0; i < 4000; i++) {
        double multiplier = 0.5 * (1 - cos(2*3.1415*i/3999));
        buff.push_back(multiplier * buff[i]);
    }
    */

    //Start Recording Thread ( Records audio off main loop
    std::thread SoundThread(StartRecord, pMySink);

    // Start render Loop
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
            auto time_start_loop = std::chrono::high_resolution_clock::now();
            
            auto time_start_plan = std::chrono::high_resolution_clock::now();

            mtx.lock();

			fftwf_execute(p);

            mtx.unlock();

            auto time_end_plan = std::chrono::high_resolution_clock::now();

        auto time_start_render = std::chrono::high_resolution_clock::now();

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        float size;
        glColor3f(0, 0, 1);
        for(int k = 1; k < 200; k++){
            /* Get the amplitude of the freq */
            size = sqrt((out[k][0] * out[k][0]) + (out[k][1] * out[k][1]));
            /* Make a rectangle according to the above size */
            glRectf(((float)2*k/200)-1, -1.0f, (((float)2*k/200)-1+(0.005f)), (size/50) - 1);  //start pos 
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        auto time_end_render = std::chrono::high_resolution_clock::now();

        auto time_end_loop = std::chrono::high_resolution_clock::now();

        //Calc and print durations for debugging
        auto duration_loop = std::chrono::duration_cast<std::chrono::microseconds>( time_end_loop - time_start_loop ).count();
        auto duration_plan = std::chrono::duration_cast<std::chrono::microseconds>( time_end_plan - time_start_plan ).count();
        auto duration_render = std::chrono::duration_cast<std::chrono::microseconds>( time_end_render - time_start_render ).count();
    }

    glfwTerminate();
    pMySink->DoneRecording = true;

    ret = 0;
    fftwf_destroy_plan(p);
    fftwf_free(out);
    delete pMySink;

    return ret;
    
}
