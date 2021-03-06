#ifndef XVECTORXD_H
#define XVECTORXD_H

#include "XGlobal.h"
#include "Utilities/XAssert.h"
#include "Math/XMathHelpers.h"
#include "Math/Eigen/Core"

namespace Eks
{

typedef Eigen::Matrix<Real, 2, 1> Vector2D;
typedef Eigen::Matrix<Real, 3, 1> Vector3D;
typedef Eigen::Matrix<Real, 4, 1> Vector4D;

typedef Eigen::Matrix<xint32, 2, 1> VectorI2D;
typedef Eigen::Matrix<xint32, 3, 1> VectorI3D;
typedef Eigen::Matrix<xint32, 4, 1> VectorI4D;

typedef Eigen::Matrix<xuint32, 2, 1> VectorUI2D;
typedef Eigen::Matrix<xuint32, 3, 1> VectorUI3D;
typedef Eigen::Matrix<xuint32, 4, 1> VectorUI4D;

// types for integration with d3d gl, tightly packed for uniform blocks etc.
typedef Eigen::Matrix<float, 3, 1, Eigen::DontAlign> Vector3DTight;
typedef Eigen::Matrix<float, 2, 1, Eigen::DontAlign> Vector2DTight;
xCompileTimeAssert(sizeof(Vector3DTight) == sizeof(float) * 3);
xCompileTimeAssert(sizeof(Vector2DTight) == sizeof(float) * 2);

}

namespace Eigen
{

template <typename Derived> std::istream &operator>>(std::istream &str, Eigen::PlainObjectBase<Derived> &vec)
  {
  xint32 rows = Derived::RowsAtCompileTime;
  xint32 cols = Derived::ColsAtCompileTime;

  if(rows == Eigen::Dynamic)
    {
    str >> rows;
    }

  if(cols == Eigen::Dynamic)
    {
    str >> cols;
    }

  for(xint32 i=0; i<rows; ++i)
    {
    for(xint32 j=0; j<cols; ++j)
      {
      str >> vec(i,j);
      }
    }

  return str;
  }

template <typename Derived> std::ostream &operator<<(std::ostream &str, const Eigen::PlainObjectBase<Derived> &vec)
  {
  xint32 rows = Derived::RowsAtCompileTime;
  xint32 cols = Derived::ColsAtCompileTime;

  if(rows == Eigen::Dynamic)
    {
    rows = vec.rows();
    str << rows << " ";
    }

  if(cols == Eigen::Dynamic)
    {
    cols = vec.cols();
    str << cols << " ";
    }

  for(xint32 i=0; i<rows; ++i)
    {
    for(xint32 j=0; j<cols; ++j)
      {
      str << vec(i,j);

      bool iC = i < (rows-1);
      bool jC = j < (cols-1);
      if(iC || jC)
        {
        str << " ";
        }
      }
    }

  return str;
  }
}

#if X_QT_INTEROP

#include "QtCore/QMetaType"
#include "QtCore/QDataStream"

#include "QtGui/QVector2D"
#include "QtGui/QVector3D"
#include "QtGui/QVector4D"

inline QVector2D toQt(const Eks::Vector2D &vec)
  {
  return QVector2D(vec(0), vec(1));
  }

inline QVector3D toQt(const Eks::Vector3D &vec)
  {
  return QVector3D(vec(0), vec(1), vec(2));
  }

inline QVector4D toQt(const Eks::Vector4D &vec)
  {
  return QVector4D(vec(0), vec(1), vec(2), vec(3));
  }

Q_DECLARE_METATYPE(Eks::Vector2D);
Q_DECLARE_METATYPE(Eks::Vector3D);
Q_DECLARE_METATYPE(Eks::Vector4D);

#endif

#endif // XVECTORXD_H
