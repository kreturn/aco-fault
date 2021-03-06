/*
 * File:   Environment.cpp
 * Author: keoma
 *
 * Created on April 26, 2013, 6:29 PM
 */

#include <string.h>
#include <float.h>
#include <iostream>

#include "Environment.h"

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

Environment::Environment( float initialPheromone, float minimumPheromone, float evaporationRate, Image* image ):
_evaporationRate(evaporationRate), _initialPheromone(initialPheromone), _minimumPheromone(minimumPheromone)
{
    construct( imgGetHeight( image ), imgGetWidth( image ) );
    computeImageMatrix( image );
    clearFeromone();
}



Environment::~Environment()
{
    delete[] _imageMatrix;
    delete[] _pheromoneMatrix;
}



void Environment::construct( int height, int width )
{
    _height = height;
    _width = width;

    int size = height * width;

    _imageMatrix = new float[size];
    _pheromoneMatrix = new float[size];

    memset( _imageMatrix,      0, sizeof(float)*size );
    memset( _pheromoneMatrix,  0, sizeof(float)*size );
}



void Environment::computeImageMatrix( Image* image )
{
    Image* gray = imgGrey( image );

#pragma omp parallel for
    for (int x=0; x<_width; ++x)
    {
        for (int y=0; y<_height; ++y)
        {
            float luminance;
            imgGetPixel3f( gray, x, y, &luminance, &luminance, &luminance );

            int i = id( x, y );
            _imageMatrix[i] = luminance;
        }
    }
}



void Environment::clearFeromone()
{
#pragma omp parallel for
    for (int x=0; x<_width; ++x)
    {
        for (int y=0; y<_height; ++y)
        {
            int i = id( x, y );
            _pheromoneMatrix[i] = _initialPheromone;
        }
    }
}



int Environment::id( int x, int y )
{
    if (x < 0 || y < 0) return 0;

    return x + _width * y;
}



float Environment::getPheromone( int x, int y )
{
    return _pheromoneMatrix[id(x,y)];
}



float Environment::getPheromone( Point point )
{
    return getPheromone( point.x, point.y );
}



void Environment::addPheromone( float amount, int x, int y )
{
    _pheromoneMatrix[id(x,y)] += amount;
}



void Environment::addPheromone( float amount, Point point )
{
    addPheromone( amount, point.x, point.y );
}



float Environment::getMaximumPheromone()
{
    float max = 0.0f;

    for (int x=0; x<_width; ++x)
    {
        for (int y=0; y<_height; ++y)
        {
            int i = id( x, y );
            if (_pheromoneMatrix[i] > max) max = _pheromoneMatrix[i];
        }
    }

    return max;
}



float Environment::getVisibility( int xo, int yo, int xd, int yd )
{
    float w  = _imageMatrix[id(xd-1, yd  )] - _imageMatrix[id(xo, yo)];
    float e  = _imageMatrix[id(xd+1, yd  )] - _imageMatrix[id(xo, yo)];
    float n  = _imageMatrix[id(xd  , yd+1)] - _imageMatrix[id(xo, yo)];
    float s  = _imageMatrix[id(xd  , yd-1)] - _imageMatrix[id(xo, yo)];
    float ne = _imageMatrix[id(xd+1, yd+1)] - _imageMatrix[id(xo, yo)];
    float nw = _imageMatrix[id(xd-1, yd+1)] - _imageMatrix[id(xo, yo)];
    float se = _imageMatrix[id(xd+1, yd-1)] - _imageMatrix[id(xo, yo)];
    float sw = _imageMatrix[id(xd-1, yd-1)] - _imageMatrix[id(xo, yo)];

    float max = _minimumPheromone + MAX(w,MAX(e,MAX(n,MAX(s,MAX(ne,MAX(nw,MAX(se,sw)))))));

    return max;
}



float Environment::getVisibility( Point origin, Point destination )
{
    return getVisibility( origin.x, origin.y, destination.x, destination.y );
}



void Environment::evaporatePheromone()
{
#pragma omp parallel for
    for (int x=0; x<_width; ++x)
    {
        for (int y=0; y<_height; ++y)
        {
            int i = id( x, y );
            _pheromoneMatrix[i] *= ( 1 - _evaporationRate );

             if (_pheromoneMatrix[i] < _minimumPheromone)
            {
                _pheromoneMatrix[i] = _minimumPheromone;
            }
        }
    }
}



void Environment::getAdjacent( Point point, std::vector<Point>& adjacent )
{
    for (int x = point.x-1; x <= point.x+1; ++x)
    {
        if ( x < 0 || x >= _width) continue;

        for (int y = point.y-1; y<=point.y+1; ++y)
        {
            if (y < 0 || y >= _height) continue;
            if (x == point.x && y == point.y) continue;

            adjacent.push_back( Point( x, y ) );
        }
    }
}



int Environment::getWidth()
{
    return _width;
}



int Environment::getHeight()
{
    return _height;
}



Image* Environment::getPheromoneImage()
{
    //normalizePheromone();

    Image* output = imgCreate( _width, _height, 3 );

#pragma omp parallel for
    for (int x = 0; x < _width; ++x)
    {
        for (int y = 0; y < _height; ++y)
        {
            float luminance = _pheromoneMatrix[id(x,y)];
            imgSetPixel3f( output, x, y, luminance, luminance, luminance );
//            if (luminance >= 0.1)
//            {
//                imgSetPixel3f( output, x, y, 1.0f, 1.0f, 1.0f );
//            }
        }
    }

    return output;
}



void Environment::normalizePheromone()
{
    float max = 0;

    int nElements = _height * _width;

    for ( int i = 0; i < nElements; ++i )
    {
        if (max < _pheromoneMatrix[i])
        {
            max = _pheromoneMatrix[i];
            std::cerr << "max: "<<max<<"\n";
        }
    }

#pragma omp parallel for
    for ( int i = 0; i < nElements; ++i )
    {
       _pheromoneMatrix[i] /= max;
    }
}
