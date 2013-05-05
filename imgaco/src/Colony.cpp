/*
 * File:   Colony.cpp
 * Author: keoma
 *
 * Created on April 26, 2013, 6:29 PM
 */

#include <stdlib.h>

#include "Colony.h"

#define INITIAL_PHEROMONE_ 0.5f
#define EVAPORATION_RATE_ 0.5f
#define MEMORY_ 10

Colony::Colony( Image* image, int nAnts )
{
    _environment = new Environment( INITIAL_PHEROMONE_, EVAPORATION_RATE_, image );

    addAnts( nAnts );
}



Colony::~Colony()
{
    delete _environment;
    _ants.clear();
}



void Colony::addAnt( Point point )
{
    _ants.push_back( Ant( point, MEMORY_ ) );
}



void Colony::addAnt()
{
    int x = rand() % _environment->getWidth();
    int y = rand() % _environment->getHeight();

    addAnt( Point(x,y) );
}



void Colony::addAnts( int n )
{
    for (int i = 0; i < n; ++i)
    {
        addAnt();
    }
}



void Colony::run( int nSteps )
{

}
