#include <stdio.h>
#include <thread>
#include <algorithm>

#include "CycleTimer.h"

typedef struct
{
    float x0, x1;
    float y0, y1;
    unsigned int width;
    unsigned int height;
    int maxIterations;
    int *output;
    int threadId;
    int numThreads;
} WorkerArgs;

static inline int mandel(float c_re, float c_im, int count)
{
  float z_re = c_re, z_im = c_im;
  int i;
  for (i = 0; i < count; ++i)
  {

    if (z_re * z_re + z_im * z_im > 4.f)
      break;

    float new_re = z_re * z_re - z_im * z_im;
    float new_im = 2.f * z_re * z_im;
    z_re = c_re + new_re;
    z_im = c_im + new_im;
  }

  return i;
}

static inline void mandelbrotSerial(
    float x0, float y0, float x1, float y1,
    int width, int height,
    int startRow, int totalRows,
    int maxIterations,
    int numThreads,
    int output[])
{
  float dx = (x1 - x0) / width;
  float dy = (y1 - y0) / height;

  for (int j = startRow; j < height; j+=numThreads)
  {
    for (int i = 0; i < width; ++i)
    {
      float x = x0 + i * dx;
      float y = y0 + j * dy;

      int index = (j * width + i);
      output[index] = mandel(x, y, maxIterations);
    }
  }
}

//
// workerThreadStart --
//
// Thread entrypoint.
static inline void workerThreadStart(WorkerArgs *const args)
{

    // TODO FOR PP STUDENTS: Implement the body of the worker
    // thread here. Each thread could make a call to mandelbrotSerial()
    // to compute a part of the output image. For example, in a
    // program that uses two threads, thread 0 could compute the top
    // half of the image and thread 1 could compute the bottom half.
    // Of course, you can copy mandelbrotSerial() to this file and 
    // modify it to pursue a better performance.

    // printf("Hello world from thread %d\n", args->threadId);
    
    // =================================== version 1 ===================================
    // int DivideRow = args->height / args->numThreads;
    // int RowPerThread = args->threadId == args->numThreads - 1 ? args->height -  DivideRow * args->threadId : DivideRow;
    // int STARTROW = args->threadId * DivideRow;
    // double startTime = CycleTimer::currentSeconds();
    // mandelbrotSerial(
    //     args->x0, args->y0, args->x1, args->y1,
    //     args->width, args->height,
    //     STARTROW, RowPerThread,
    //     args->maxIterations, args->output
    // );
    // double endTime = CycleTimer::currentSeconds();
    // double minTime = std::min(1e30, endTime - startTime);
    // printf("[ Thread %d ]: cost\t [ %.3f ] ms\n", args->threadId, minTime*1000);
    // =================================================================================

    // =================================== version 2 ===================================
    // int DivideRow = args->height / args->numThreads;
    // int RowPerThread = args->threadId == args->numThreads - 1 ? args->height -  DivideRow * args->threadId : DivideRow;
    // int STARTROW = args->threadId * DivideRow;
    // double startTime = CycleTimer::currentSeconds();
    
    mandelbrotSerial(
        args->x0, args->y0, args->x1, args->y1,
        args->width, args->height,
        args->threadId, 0,
        args->maxIterations, 
        args->numThreads,
        args->output
    );
    
    // double endTime = CycleTimer::currentSeconds();
    // double minTime = std::min(1e30, endTime - startTime);
    // printf("[ Thread %d ]: cost\t [ %.3f ] ms\n", args->threadId, minTime*1000);
}

//
// MandelbrotThread --
//
// Multi-threaded implementation of mandelbrot set image generation.
// Threads of execution are created by spawning std::threads.
void mandelbrotThread(
    int numThreads,
    float x0, float y0, float x1, float y1,
    int width, int height,
    int maxIterations, 
    int output[])
{
    static constexpr int MAX_THREADS = 32;

    if (numThreads > MAX_THREADS)
    {
        fprintf(stderr, "Error: Max allowed threads is %d\n", MAX_THREADS);
        exit(1);
    }

    // Creates thread objects that do not yet represent a thread.
    std::thread workers[MAX_THREADS];
    WorkerArgs args[MAX_THREADS];

    for (int i = 0; i < numThreads; i++)
    {
        // TODO FOR PP STUDENTS: You may or may not wish to modify
        // the per-thread arguments here.  The code below copies the
        // same arguments for each thread

        args[i].x0 = x0;
        args[i].y0 = y0;
        args[i].x1 = x1;
        args[i].y1 = y1;
        args[i].width = width;
        args[i].height = height;
        args[i].maxIterations = maxIterations;
        args[i].numThreads = numThreads;
        args[i].output = output;
        args[i].threadId = i;
    }

    // Spawn the worker threads.  Note that only numThreads-1 std::threads
    // are created and the main application thread is used as a worker
    // as well.
    for (int i = 1; i < numThreads; i++)
    {
        workers[i] = std::thread(workerThreadStart, &args[i]);
    }

    workerThreadStart(&args[0]);

    // join worker threads
    for (int i = 1; i < numThreads; i++)
    {
        workers[i].join();
    }
    
}