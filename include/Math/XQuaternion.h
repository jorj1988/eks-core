#ifndef XQUATERNION_H
#define XQUATERNION_H

#include "Math/XMathHelpers.h"
#include "Eigen/Geometry"

namespace Eks
{
typedef Eigen::Quaternion<Real> Quaternion;
}


inline bool operator==(const Eks::Quaternion &a, const Eks::Quaternion& b)
  {
  return a.coeffs() == b.coeffs();
  }

inline bool operator!=(const Eks::Quaternion &a, const Eks::Quaternion& b)
  {
  return a.coeffs() != b.coeffs();
  }

inline std::ostream &operator<<(std::ostream &str, const Eks::Quaternion &quat)
  {
  return str << quat.x() << " " << quat.y() << " " << quat.z() << " " << quat.w();
  }

inline std::istream &operator>>(std::istream &str, Eks::Quaternion &quat)
  {
  return str >> quat.x() >> quat.y() >> quat.z() >> quat.w();
  }

#if X_QT_INTEROP

#include "QtCore/QTextStream"
#include "QtCore/QDataStream"

inline QTextStream &operator<<(QTextStream &str, const Eks::Quaternion &quat)
  {
  return str << quat.x() << " " << quat.y() << " " << quat.z() << " " << quat.w();
  }

inline QTextStream &operator>>(QTextStream &str, Eks::Quaternion &quat)
  {
  return str >> quat.x() >> quat.y() >> quat.z() >> quat.w();
  }

inline QDataStream &operator<<(QDataStream &str, const Eks::Quaternion &quat)
  {
  return str << quat.x() << quat.y() << quat.z() << quat.w();
  }

inline QDataStream &operator>>(QDataStream &str, Eks::Quaternion &quat)
  {
  return str >> quat.x() >> quat.y() >> quat.z() >> quat.w();
  }


Q_DECLARE_METATYPE(Eks::Quaternion);

#endif

#endif // XQUATERNION_H
