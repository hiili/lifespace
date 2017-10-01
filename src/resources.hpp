/**
 * @file resources.hpp
 *
 * Definitions for colors, materials, etc.
 */
#ifndef SIM_RESOURCES_HPP
#define SIM_RESOURCES_HPP


#include <lifespace/lifespace.hpp>
#include <boost/shared_ptr.hpp>
#include <GL/gl.h>




/* colors */

const GLfloat none[4]         = { 0.0, 0.0, 0.0, 0.0 };
const GLfloat black[4]        = { 0.0, 0.0, 0.0, 1.0 };
const GLfloat white[4]        = { 1.0, 1.0, 1.0, 1.0 };
const GLfloat superwhite[4]   = { 10.0, 10.0, 10.0, 1.0 };
const GLfloat gray8A[4]       = { 0.8, 0.8, 0.8, 0.7 };
const GLfloat gray6A[4]       = { 0.6, 0.6, 0.6, 0.7 };
const GLfloat gray3A[4]       = { 0.3, 0.3, 0.3, 0.7 };
const GLfloat gray9[4]        = { 0.9, 0.9, 0.9, 1.0 };
const GLfloat gray8[4]        = { 0.8, 0.8, 0.8, 1.0 };
const GLfloat gray7[4]        = { 0.7, 0.7, 0.7, 1.0 };
const GLfloat gray6[4]        = { 0.6, 0.6, 0.6, 1.0 };
const GLfloat gray5[4]        = { 0.5, 0.5, 0.5, 1.0 };
const GLfloat gray4[4]        = { 0.4, 0.4, 0.4, 1.0 };
const GLfloat gray3[4]        = { 0.3, 0.3, 0.3, 1.0 };
const GLfloat gray2[4]        = { 0.2, 0.2, 0.2, 1.0 };
const GLfloat gray1[4]        = { 0.1, 0.1, 0.1, 1.0 };
const GLfloat lightRed[4]     = { 1.0, 0.5, 0.5, 1.0 };
const GLfloat red[4]          = { 1.0, 0.0, 0.0, 1.0 };
const GLfloat red6[4]         = { 0.6, 0.0, 0.0, 1.0 };
const GLfloat red3[4]         = { 0.3, 0.0, 0.0, 1.0 };
const GLfloat red1[4]         = { 0.1, 0.0, 0.0, 1.0 };
const GLfloat lightBlue[4]    = { 0.5, 0.5, 1.0, 1.0 };
const GLfloat blue[4]         = { 0.0, 0.0, 1.0, 1.0 };
const GLfloat blue8[4]        = { 0.0, 0.0, 0.8, 1.0 };
const GLfloat blue6[4]        = { 0.0, 0.0, 0.6, 1.0 };
const GLfloat blue3[4]        = { 0.0, 0.0, 0.3, 1.0 };
const GLfloat lightYellow[4]  = { 1.0, 1.0, 0.5, 1.0 };
const GLfloat yellow[4]       = { 1.0, 1.0, 0.0, 1.0 };
const GLfloat yellow8[4]      = { 0.8, 0.8, 0.0, 1.0 };
const GLfloat yellow6[4]      = { 0.6, 0.6, 0.0, 1.0 };
const GLfloat yellow3[4]      = { 0.3, 0.3, 0.0, 1.0 };
const GLfloat polished[1]     = { 40.0 };
const GLfloat semipolished[1] = { 20.0 };
const GLfloat dull[1]         = { 5.0 };

/* visual materials */

const lifespace::Material white8Mat( gray4, gray8, white,
                                     none, polished, GL_FRONT );
const lifespace::Material whiteMat( gray3, gray6, white,
                                    none, polished, GL_FRONT );
const lifespace::Material grayMat( gray1, gray3, white,
                                   none, polished, GL_FRONT );
const lifespace::Material floorBrightMat( gray3, gray6A, white,
                                          none, semipolished, GL_FRONT );
const lifespace::Material floorDarkMat( gray1, gray3A, white,
                                        none, semipolished, GL_FRONT );
const lifespace::Material redMat( red3, red6, lightRed,
                                  none, polished, GL_FRONT );
const lifespace::Material yellowMat( yellow3, yellow6, lightYellow,
                                     none, dull, GL_FRONT );
const lifespace::Material blueMat( blue3, blue6, white,
                                   none, dull, GL_FRONT );
const lifespace::Material brightWhiteMat( white, white, white,
                                          none, polished, GL_FRONT );
const lifespace::Material brightBlueMat( lightBlue, lightBlue, lightBlue,
                                         none, polished, GL_FRONT );


/* physics materials */

const boost::shared_ptr<lifespace::CollisionMaterial> defaultSurface
( new lifespace::CollisionMaterial( 0.5, 0.85, 0.001 ));
const boost::shared_ptr<lifespace::CollisionMaterial> slipperySurface
( new lifespace::CollisionMaterial( 0.1, 0.9, 0.001 ));




#endif   /* SIM_RESOURCES_HPP */
