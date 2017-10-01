/**
 * @file main.cpp
 *
 */

#include <lifespace/lifespace.hpp>
using namespace lifespace;

#include <ode/ode.h>
#include <ode/odecpp.h>

#include <iostream>
using std::cout;
using std::endl;

#include <cstdio>
using std::printf;

#include <ctime>
using std::clock;
using std::clock_t;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;
using boost::dynamic_pointer_cast;

#include <boost/timer.hpp>
using boost::timer;




static const shared_ptr<CollisionMaterial> defaultSurface
( new CollisionMaterial( 0.9, 0.9, 0.001 ));








void makeBallSpaces( Subspace * subspace, int level,
                     bool odeBodies, bool odeGeoms )
{
  if( level == 0 ) {
    subspace->addObject
      ( new Object
        ( Object::Params( odeBodies ? new ODELocator() : new BasicLocator(),
                          0,
                          ( odeGeoms ?
                            new BasicGeometry
                            ( shapes::Sphere::create( 0.3 ),
                              defaultSurface ) :
                            0 ))));
    cout << ".";
    return;
  }
  
  cout << "SSSS";
  Subspace * ballSpace1 = new Subspace
    ( Object::Params( new BasicLocator
                      ( makeVector3d( -level/2.0, 2.0, -level/2.0 ) )));
  Subspace * ballSpace2 = new Subspace
    ( Object::Params( new BasicLocator
                      ( makeVector3d( -level/2.0, 2.0, level/2.0 ) )));
  Subspace * ballSpace3 = new Subspace
    ( Object::Params( new BasicLocator
                      ( makeVector3d( level/2.0, 2.0, -level/2.0 ) )));
  Subspace * ballSpace4 = new Subspace
    ( Object::Params( new BasicLocator
                      ( makeVector3d( level/2.0, 2.0, level/2.0 ) )));
  
  subspace->addObject( ballSpace1 );
  makeBallSpaces( ballSpace1, level / 2, odeBodies, odeGeoms );
  subspace->addObject( ballSpace2 );
  makeBallSpaces( ballSpace2, level / 2, odeBodies, odeGeoms );
  subspace->addObject( ballSpace3 );
  makeBallSpaces( ballSpace3, level / 2, odeBodies, odeGeoms );
  subspace->addObject( ballSpace4 );
  makeBallSpaces( ballSpace4, level / 2, odeBodies, odeGeoms );
}




void makeBallNospaces( Subspace * subspace, int level,
                       bool odeBodies, bool odeGeoms,
                       float x = 0.0, float y = 2.0, float z = 0.0 )
{
  if( level == 0 ) {
    subspace->addObject
      ( new Object
        ( Object::Params( odeBodies ?
                          new ODELocator(makeVector3d(x,y,z)) :
                          new BasicLocator(makeVector3d(x,y,z)),
                          0,
                          ( odeGeoms ?
                            new BasicGeometry
                            ( shapes::Sphere::create( 0.3 ),
                              defaultSurface ) :
                            0 ))));
    cout << ".";
    return;
  }
  
  makeBallNospaces( subspace, level / 2, odeBodies, odeGeoms,
                    x - level/2.0, y, z - level/2.0 );
  makeBallNospaces( subspace, level / 2, odeBodies, odeGeoms,
                    x - level/2.0, y, z + level/2.0 );
  makeBallNospaces( subspace, level / 2, odeBodies, odeGeoms,
                    x + level/2.0, y, z - level/2.0 );
  makeBallNospaces( subspace, level / 2, odeBodies, odeGeoms,
                    x + level/2.0, y, z + level/2.0 );
}








int main( int argc, char * argv[] )
{
  if( argc != 5 ) {
    cout << "Käyttis: " << argv[0]
         << " <depth> [spaces|no] [odebodies|no] [odegeoms|no]" << endl;
    exit(1);
  }
  int depth      = atoi( argv[1] );
  bool spaces    = 0 == strcmp( argv[2], "spaces" );
  bool odeBodies = 0 == strcmp( argv[3], "odebodies" );
  bool odeGeoms  = 0 == strcmp( argv[4], "odegeoms" );
  
  cout << "depth: " << depth << ", spaces: " << spaces
       << ", odeBodies: " << odeBodies << ", odeGeoms: " << odeGeoms << endl;
  
  
  // world
  ODEWorld world;
  world.setGravityVector( makeVector3d( 0.0, -9.81, 0.0 ));
  
  // collision detection
  ODECollisionRenderer collisionRenderer( &world );




  /* create the world content */
  
  
  // balls
  spaces
    ? makeBallSpaces( &world, depth, odeBodies, odeGeoms )
    : makeBallNospaces( &world, depth, odeBodies, odeGeoms );
  cout << endl;
  
  
  
  
  
  
  
  
  // activate the world
  world.activate( true );
  collisionRenderer.connect();


  
  
  // start the system
  
  //long long iter = 0;
  int iter = 0;
  timer t;
  
  iter = 0; t.restart();
  do {
    // no-op
    iter++;
  } while( iter % 10 || t.elapsed() < 4.0 );
  printf( "no-op:                      %.9f s/iteration (%10.f iterations/s)\n\n",
          4.0 / iter, iter / 4.0 );
  
  iter = 0; t.restart();
  do {
    world.timestep( 0.01 );
    iter++;
  } while( iter % 10 || t.elapsed() < 4.0 );
  printf( "world.timestep():           %.9f s/iteration (%10.f iterations/s)\n",
          4.0 / iter, iter / 4.0 );
  
  iter = 0; t.restart();
  do {
    world.prepare( 0.01 );
    iter++;
  } while( iter % 10 || t.elapsed() < 4.0 );
  printf( "world.prepare():            %.9f s/iteration (%10.f iterations/s)\n",
          4.0 / iter, iter / 4.0 );
  
  iter = 0; t.restart();
  do {
    world.dWorld::step( 0.01 );
    iter++;
  } while( iter % 10 || t.elapsed() < 4.0 );
  printf( "dWorld.step():              %.9f s/iteration (%10.f iterations/s)\n",
          4.0 / iter, iter / 4.0 );
  
  iter = 0; t.restart();
  do {
    world.World::step();
    iter++;
  } while( iter % 10 || t.elapsed() < 4.0 );
  printf( "World.step():               %.9f s/iteration (%10.f iterations/s)\n\n",
          4.0 / iter, iter / 4.0 );
  
  
  
  iter = 0; t.restart();
  do {
    collisionRenderer.render();
    iter++;
  } while( iter % 10 || t.elapsed() < 4.0 );
  printf( "collisionRenderer.render(): %.9f s/iteration (%10.f iterations/s)\n",
          4.0 / iter, iter / 4.0 );
  
  iter = 0; t.restart();
  do {
    world.timestep( 0.01 );
    collisionRenderer.render();
    iter++;
  } while( iter % 10 || t.elapsed() < 4.0 );
  printf( "timestep + collisions:      %.9f s/iteration (%10.f iterations/s)\n",
          4.0 / iter, iter / 4.0 );
  
  collisionRenderer.disconnect();
  world.activate( false );
  world.killEmAll();
  world.grimreaper();
  
  
  // this is never reached
  return 0;
}
