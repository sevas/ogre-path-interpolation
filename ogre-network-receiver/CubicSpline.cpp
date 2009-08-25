#include "precompiled.h"

#include "CubicSpline.h"

#include <OgreMath.h>

//------------------------------------------------------------------------------
CubicSpline::CubicSpline()
    :mPoints(4)
    ,mStartPoint(2)
    ,mEndPoint(2)
    ,mHasControlPoints(false)
{

}
//------------------------------------------------------------------------------
Vector3 CubicSpline::evaluate(float _t)
{
    assert(_t>=0 && _t<=1);
    Vector3 res;
    if(mHasControlPoints)
    {       
        res.x = evaluateF(mXVars[0], mXVars[1], mXVars[2], mXVars[3], _t);
        res.y = evaluateF(mYVars[0], mYVars[1], mYVars[2], mYVars[3], _t);
        res.z = evaluateF(mZVars[0], mZVars[1], mZVars[2], mZVars[3], _t);
    }
    else
    {
        res = Vector3::ZERO;
    }
    return res;
}
//------------------------------------------------------------------------------
float CubicSpline::evaluateF(float _A, float _B, float _C, float _D, float _t)
{
    return  _A*Math::Pow(_t, 3)  +  _B*Math::Pow(_t, 2)   +  _C*_t + _D;
}
//------------------------------------------------------------------------------
void CubicSpline::setStartPoint(const Vector3 &_pos, const Vector3 &_speed)
{
    mHasControlPoints = false;
    mStartPoint[0] = _pos;
    mStartPoint[1] = _speed;
}
//------------------------------------------------------------------------------
void CubicSpline::setEndPoint(const Vector3 &_pos, const Vector3 &_speed)
{
    mHasControlPoints = false;
    mEndPoint[0] = _pos;
    mEndPoint[1] = _speed;
}
//------------------------------------------------------------------------------
void CubicSpline::calcControlPoints()
{
    mPoints[0] = mStartPoint[0];
    mPoints[1] = mStartPoint[0] + mStartPoint[1];
    mPoints[2] = mEndPoint[0] - mEndPoint[1];
    mPoints[3] = mEndPoint[0];

    mXVars = _getSplineVariables(mPoints[0].x, mPoints[1].x, mPoints[2].x, mPoints[3].x);
    mYVars = _getSplineVariables(mPoints[0].y, mPoints[1].y, mPoints[2].y, mPoints[3].y);
    mZVars = _getSplineVariables(mPoints[0].z, mPoints[1].z, mPoints[2].z, mPoints[3].z);

    mHasControlPoints = true;
}
//------------------------------------------------------------------------------
Vector4 CubicSpline::_getSplineVariables(float _c0, float _c1, float _c2, float _c3)
{
    Vector4 res;
    res[0] = _c3  -  3*_c2  +  3*_c1  -   _c0;
    res[1] =         3*_c2  -  6*_c1  + 3*_c0;
    res[2] =                   3*_c1  - 3*_c0;
    res[3] =                              _c0;       

    return res;
}
//------------------------------------------------------------------------------