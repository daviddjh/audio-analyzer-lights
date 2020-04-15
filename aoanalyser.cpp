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
#define BUFSIZE 1024
//matplotlib
#include "matplotlibcpp.h"

//g++ aoanalyser.cpp -I/usr/include/python2.7 -I/home/daviddjh/.local/lib/python2.7/site-packages/numpy/core/include/ -lfftw3 -lm -lpulse-simple -lpulse -g -std=c++11 -lpython2.7 

using namespace std;

namespace plt = matplotlibcpp;

static ssize_t loop_write(int fd, const void*data, size_t size);

int main (int argc, char *argv[]){


    fftw_complex *out;
    double *in;
    fftw_plan p;
    int ret;
    float buf[BUFSIZE];

    //fftw arrays
    //in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * BUFSIZE);
    in = (double*) fftw_malloc(sizeof(double) * BUFSIZE);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * BUFSIZE);

    //fftw plan - setup for the transform to be executed
    //p = fftw_plan_dft_1d(BUFSIZE, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    p = fftw_plan_dft_r2c_1d(BUFSIZE, in, out, FFTW_ESTIMATE);

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

    float bass, mid, high;
    for (int j = 0; j < 1000; j++){
        pa_simple_read(s, buf, sizeof(buf), &error);

        //fill fftw array with audio buffer
        for(int i = 0; i < BUFSIZE; i++){
            double multiplier = 0.5 * (1 - cos(2*3.1415*i/(BUFSIZE-1)));
            in[i] = (multiplier * buf[i]);
        }
        //in = buf;

        fftw_execute(p);

        // Bass - 20 - 300      [1,7]   .1 = 35
        // Mid  - 301 - 1280    [8,29]  .1 = 105
        // High - 1281 - 10200  [30,233].1 = 1015
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
        if(pow(bass, 2) > 20000){
            cout << " +++ " << endl;
        }


    }


    
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

    ret = 0;

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);

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