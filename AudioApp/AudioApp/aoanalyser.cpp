#define EXTERN
//define EXTERN only goes in one file, vars.h goes in all files that use it

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

//Global Vars
//std::mutex mtx;
#include "vars.h"

GLFWwindow * init_openGL(){

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return nullptr;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1920, 1080, "Audio Out Analysis", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return nullptr;
    }
}

void init(){

}

int main(int argc, char* argv[]) {

	auto time_start_main = std::chrono::high_resolution_clock::now();

    //fftwf_init_threads();

    // Setup GLFW
    GLFWwindow* window = init_openGL();
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    //return var for main func
    int ret;

    // init Audio Sink
    MyAudioSink* pMySink = new MyAudioSink;
    pMySink->DoneRecording = false;

    // init fftw plan and output array
    fftwf_complex* out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * BUFFSIZE);
    //fftwf_plan_with_nthreads(4);
    fftwf_plan p = fftwf_plan_dft_r2c_1d(BUFFSIZE, pMySink->pCurrentSoundBuffer, out, 0);

    //Start Recording Thread ( Records audio off main loop
    std::thread SoundThread(StartRecord, pMySink);

    //hann function
    /*
    for (int i = 0; i < 4000; i++) {
        double multiplier = 0.5 * (1 - cos(2*3.1415*i/3999));
        buff.push_back(multiplier * buff[i]);
    }
    */

    // Start render Loop
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
		auto time_start_loop = std::chrono::high_resolution_clock::now();
		
		auto time_start_plan = std::chrono::high_resolution_clock::now();

		mtx.lock();
        // hann window
		for (int i = 0; i < BUFFSIZE; i++) {
			float multiplier = 0.5 * (1 - cos(2*3.1415*i/(BUFFSIZE - 1)));
			pMySink->pCurrentSoundBuffer[i] = pMySink->pCurrentSoundBuffer[i] * multiplier;
		}

		fftwf_execute(p);

		mtx.unlock();

		auto time_end_plan = std::chrono::high_resolution_clock::now();

        auto time_start_render = std::chrono::high_resolution_clock::now();

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        float size;
        /*
        auto time_color = std::chrono::high_resolution_clock::now();
		auto duration_color = std::chrono::duration_cast<std::chrono::seconds>( time_color - time_start_main).count();
        float mod1 = ((float)((float)(duration_color % 20) + 1) / 20);
        float mod2 = ((float)((float)((duration_color + 7) % 20) + 1) / 20);
        float mod3 = ((float)((float)((duration_color + 15) % 20) + 1) / 20);
        std::cout << mod1 << std::endl;
        glColor3f(mod1, mod2, mod3);
        */
        glColor3f(0, .2, 1);
        for(int k = 1; k < 400; k++){
            /* Get the amplitude of the freq */
            size = sqrt((out[k][0] * out[k][0]) + (out[k][1] * out[k][1]));

            /* Make a rectangle according to the above size */
            glRectf(((float)2*k/400)-1, -1.0f, (((float)2*k/400)-1+(0.005f)), (size/90) - 1);  //start pos 
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

    return ret;
    
}
