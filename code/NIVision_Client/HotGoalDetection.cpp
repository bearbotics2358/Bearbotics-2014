#include "stdafx.h"

struct Particle
{
    int id;
    double cx, cy;
    double left, top, right;
    double width, height;
    double area;
    bool horizontal;

    void Print( void )
    {
        std::cout << "=== PARTICLE " << id << std::endl;
        std::cout << "center: (" << cx << ", " << cy << ")" << std::endl;
        std::cout << "left: " << left << ", top: " << top << ", right: " << right << std::endl;
        std::cout << "width/height: (" << width << ", " << height << ")" << std::endl;
        std::cout << "area: " << area << std::endl;
        std::cout << std::endl;
    }
};

void FilterParticles( std::vector<Particle> &particles )
{
    std::vector<Particle>::iterator it;
    for( it = particles.begin( ); it != particles.end( ); )
    {
        Particle p = *it;
        if( p.area < 500.0 || p.area > 2000.0 )
        {
            it = particles.erase( it );
        }
        else if( !p.horizontal && fabs( p.width - p.height ) < 70.0 )
        {
            it = particles.erase( it );
        }
        else if( p.horizontal && fabs( p.width - p.height ) < 40.0 )
        {
            it = particles.erase( it );
        }
        else
        {
            ++it;
        }
    }
}

void PrintParticles( const std::vector<Particle> &particles )
{
   std::vector<Particle>::const_iterator it;
    for( it = particles.begin( ); it != particles.end( ); ++it )
    {
        Particle p = *it;
        p.Print( );
    }
}

void CheckIMAQError( int rval, std::string desc )
{
    if( rval == 0 )
    {
        int error = imaqGetLastError( );

        std::ostringstream errorDesc;
        errorDesc << error << " "
            << desc;

        throw std::runtime_error( errorDesc.str( ) );
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
    const _TCHAR *filename = argv[1];

    int rval;

    Image *inputImage;
    inputImage = imaqCreateImage( IMAQ_IMAGE_U8, 3 );
    if( inputImage == NULL )
    {
        CheckIMAQError( 0, "imaqCreateImage" );
    }

    rval = imaqReadFile( inputImage, filename, NULL, NULL );
    CheckIMAQError( rval, "imaqReadFile" );

    int numParticles;
    rval = imaqCountParticles( inputImage, 1, &numParticles );
    CheckIMAQError( rval, "imaqCountParticles" );

    const int kNumMeasures = 8;
    MeasurementType measures[kNumMeasures] =
    {
        IMAQ_MT_CENTER_OF_MASS_X,
        IMAQ_MT_CENTER_OF_MASS_Y,
        IMAQ_MT_BOUNDING_RECT_LEFT,
        IMAQ_MT_BOUNDING_RECT_TOP,
        IMAQ_MT_BOUNDING_RECT_RIGHT,
        IMAQ_MT_BOUNDING_RECT_WIDTH,
        IMAQ_MT_BOUNDING_RECT_HEIGHT,
        IMAQ_MT_AREA,
    };

    MeasureParticlesReport *particleReport =
        imaqMeasureParticles( inputImage,
        IMAQ_CALIBRATION_MODE_PIXEL,
        measures,
        kNumMeasures );
    if( particleReport == NULL )
    {
        CheckIMAQError( 0, "imaqMeasureParticles" );
    }

    std::vector<Particle> particles;
    for( int i = 0; i < numParticles; i++ )
    {
        Particle p;
        p.id = i;
        p.cx = particleReport->pixelMeasurements[i][0];
        p.cy = particleReport->pixelMeasurements[i][1];
        p.left = particleReport->pixelMeasurements[i][2];
        p.top = particleReport->pixelMeasurements[i][3];
        p.right = particleReport->pixelMeasurements[i][4];
        p.width = particleReport->pixelMeasurements[i][5];
        p.height = particleReport->pixelMeasurements[i][6];
        p.area = particleReport->pixelMeasurements[i][7];
        p.horizontal = ( p.width > p.height );
        particles.push_back( p );
    }

    FilterParticles( particles );
    PrintParticles( particles );

    std::vector<Particle>::const_iterator it;
    for( it = particles.begin( ); it != particles.end( ); ++it )
    {
        Particle p = *it;

        // Only look for vertical (static) targets
        if( p.horizontal )
        {
            continue;
        }

        // The targets should be pretty long and thin
        double vRectPct = p.width / p.height;
        if( vRectPct > 0.25 )
        {
            continue;
        }

        std::vector<Particle>::const_iterator it2;
        for( it2 = particles.begin( ); it2 != particles.end( ); ++it2 )
        {
            Particle p2 = *it2;
            if( it == it2 )
            {
                continue;
            }

            // Only examine horizontal targets
            if( !p2.horizontal )
            {
                continue;
            }

            // The targets should be pretty long and thin
            double hRectPct = p2.height / p2.width;
            if( hRectPct > 0.40 )
            {
                continue;
            }

            // The vision targets are very close in
            // terms of their vertical tops
            double heightDiff = fabs( p2.top - p.top );
            if( heightDiff > 15.0 )
            {
                continue;
            }

            // They should also be pretty close (though
            // not as close) in terms of their leftmost
            // or rightmost pixels
            double leftDiff = p2.left - p.left;
            if( leftDiff > 35.0 )
            {
                continue;
            }
            double rightDiff = p.right - p2.right;
            if( rightDiff > 35.0 )
            {
                continue;
            }

            // Fairly certain of a match at this point
            std::cout << "Found a hot goal!" << std::endl;
        }
    }

    // Cleanup
    rval = imaqDispose( particleReport );
    CheckIMAQError( rval, "imaqDispose(particleReport)" );
    
	return 0;
}
