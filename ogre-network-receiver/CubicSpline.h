#ifndef CubicSpline_h__
#define CubicSpline_h__

#include <vector>
#include <OgreVector3.h>

using namespace Ogre;

class CubicSpline
{
public:
    CubicSpline();
    void setStartPoint(const Vector3&,const Vector3&);
    void setEndPoint(const Vector3&,const Vector3&);
    void calcControlPoints();
    Vector4 _getSplineVariables(float, float, float, float);
    Vector3 evaluate(float);
    float evaluateF(float, float, float, float, float);

protected:
    Vector4 mXVars, mYVars, mZVars;
    std::vector<Vector3> mPoints;
    std::vector<Vector3> mStartPoint;
    std::vector<Vector3> mEndPoint;
    bool mHasControlPoints;
    
};
#endif // CubicSpline_h__