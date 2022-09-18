#ifndef TRACKBALL_H
#define TRACKBALL_H
#include "cs488-framework/OpenGLImport.hpp"
typedef GLdouble Matrix[4][4];

/* Function prototypes */
void vCalcRotVec(float fNewX, float fNewY,
                 float fOldX, float fOldY,
                 float fDiameter,
                 float *fVecX, float *fVecY, float *fVecZ);
void vAxisRotMatrix(float fVecX, float fVecY, float fVecZ, Matrix mNewMat);
void vCopyMatrix(Matrix, Matrix);
#endif
