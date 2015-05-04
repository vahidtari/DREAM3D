#ifndef _OrientationConvertor_H_
#define _OrientationConvertor_H_


#include <QtCore/QVector>
#include <QtCore/QString>

#include "DREAM3DLib/DataArrays/DataArray.hpp"

#include "OrientationLib/OrientationLib.h"
#include "OrientationLib/Math/OrientationArray.hpp"
#include "OrientationLib/Math/OrientationTransforms.h"

template<typename T>
class OrientationConvertor
{

  public:

    enum OrientationType
    {
      Euler,
      OrientationMatrix,
      Quaternion,
      AxisAngle,
      Rodrigues,
      Homochoric,
      Cubochoric,
      UnknownOrientationType
    };

    OrientationConvertor() {}
    virtual ~OrientationConvertor() {}

    /**
   * @brief getOrientationRepresentation
   * @return
   */
    virtual OrientationType getOrientationRepresentation() { return UnknownOrientationType; }

    /**
   * @brief convert
   * @param src
   * @return
   */
    void convertRepresentationTo(OrientationType repType)
    {
      if(repType == Euler) { toEulers(); }
      else if(repType == OrientationMatrix) { toOrientationMatrix(); }
      else if(repType == Quaternion) { toQuaternion(); }
      else if(repType == AxisAngle) { toAxisAngle(); }
      else if(repType == Rodrigues) { toRodrigues(); }
      else if(repType == Homochoric) { toHomochoric(); }
      else if(repType == Cubochoric) { toCubochoric(); }
    }

    virtual void toEulers() = 0;
    virtual void toOrientationMatrix() = 0;
    virtual void toQuaternion() = 0;
    virtual void toAxisAngle() = 0;
    virtual void toRodrigues() = 0;
    virtual void toHomochoric() = 0;
    virtual void toCubochoric() = 0;

    DREAM3D_INSTANCE_PROPERTY(typename DataArray<T>::Pointer, InputData)
    DREAM3D_INSTANCE_PROPERTY(typename DataArray<T>::Pointer, OutputData)

    static QVector<QString> getOrientationTypes()
    {
      QVector<QString> otypes(7);
      otypes[0] = "Euler";
      otypes[1] = "Orientation Matrix";
      otypes[2] = "Quaternion";
      otypes[3] = "Axis-Angle";
      otypes[4] = "Rodrigues";
      otypes[5] = "Homochoric";
      otypes[6] = "Cubochoric";

      return otypes;
    }

    static

    void setSomething(int i) {}
    protected:

      private:
      OrientationConvertor(const OrientationConvertor&); // Copy Constructor Not Implemented
    void operator=( const OrientationConvertor& ); // Operator '=' Not Implemented

};

#define OC_CONVERT_BODY(OUTSTRIDE, OUT_ARRAY_NAME, CONVERSION_METHOD)\
  typedef OrientationArray<T> OrientationArray_t;\
  typename DataArray<T>::Pointer input = this->getInputData();\
  T* inPtr = input->getPointer(0);\
  size_t nTuples = input->getNumberOfTuples();\
  int inStride = input->getNumberOfComponents();\
  int outStride = OUTSTRIDE;\
  QVector<size_t> cDims(1, outStride); /* Create the 4 component (4x1) based array.*/ \
  typename DataArray<T>::Pointer output = DataArray<T>::CreateArray(nTuples, cDims, #OUT_ARRAY_NAME);\
  output->initializeWithZeros(); /* Intialize the array with Zeros */ \
  T* quatPtr = output->getPointer(0);\
  for (size_t i = 0; i < nTuples; ++i) { \
    OrientationArray_t rot(inPtr, inStride); \
    OrientationArray_t res(quatPtr, outStride); \
    OrientationTransforms<OrientationArray_t, T>::CONVERSION_METHOD(rot, res); \
    inPtr = inPtr + inStride; /* Increment euler pointer */ \
    quatPtr = quatPtr + outStride; /* Increment quaternion pointer*/ \
  }\
  this->setOutputData(output);



template<typename T>
class EulerConvertor : public OrientationConvertor<T>
{
  public:
    EulerConvertor() :
      OrientationConvertor<T>()
    {}

    explicit EulerConvertor(typename DataArray<T>::Pointer data) :
    OrientationConvertor<T>()
    {
      this->setInputData(data);
    }
    virtual ~EulerConvertor() {}

    virtual typename OrientationConvertor<T>::OrientationType getOrientationRepresentation()
    { return OrientationConvertor<T>::Euler; }

    virtual void toEulers()
    {
        ////FIXME: Copy the array
    }

    virtual void toOrientationMatrix()
    {
      OC_CONVERT_BODY(9, OrientationMatrix, eu2om)
    }

    virtual void toQuaternion()
    {
      OC_CONVERT_BODY(4, Quaternions, eu2qu)
    }

    virtual void toAxisAngle()
    {
      OC_CONVERT_BODY(4, AxisAngle, eu2ax)
    }

    virtual void toRodrigues()
    {
      OC_CONVERT_BODY(4, Rodrigues, eu2ro)
    }

    virtual void toHomochoric()
    {
      OC_CONVERT_BODY(3, Homochoric, eu2ho)
    }

    virtual void toCubochoric()
    {
      OC_CONVERT_BODY(4, Cubochoric, eu2cu)
    }

  private:

    EulerConvertor(const EulerConvertor&); // Copy Constructor Not Implemented
    void operator=(const EulerConvertor&); // Operator '=' Not Implemented
};

template<typename T>
class QuaternionConvertor : public OrientationConvertor<T>
{
  public:
    QuaternionConvertor() :
      OrientationConvertor<T>()
    {}
    explicit QuaternionConvertor(typename DataArray<T>::Pointer data) :
      OrientationConvertor<T>()
    {}

    virtual ~QuaternionConvertor() {}

    virtual typename OrientationConvertor<T>::OrientationType getOrientationRepresentation()
    { return OrientationConvertor<T>::Quaternion; }

    virtual void toEulers()
    {
      OC_CONVERT_BODY(3, Eulers, qu2eu)
    }

    virtual void toOrientationMatrix()
    {
      OC_CONVERT_BODY(9, OrientationMatrix, qu2om)
    }

    virtual void toQuaternion()
    {
        ////FIXME: Copy the array
    }

    virtual void toAxisAngle()
    {
      OC_CONVERT_BODY(4, AxisAngle, qu2ax)
    }

    virtual void toRodrigues()
    {
      OC_CONVERT_BODY(4, Rodrigues, qu2ro)
    }

    virtual void toHomochoric()
    {
      OC_CONVERT_BODY(3, Homochoric, qu2ho)
    }

    virtual void toCubochoric()
    {
      OC_CONVERT_BODY(4, Cubochoric, qu2cu)
    }

  private:

    QuaternionConvertor(const QuaternionConvertor&); // Copy Constructor Not Implemented
    void operator=( const QuaternionConvertor& ); // Operator '=' Not Implemented
};


template<typename T>
class OrientationMatrixConvertor : public OrientationConvertor<T>
{
  public:
    OrientationMatrixConvertor() :
      OrientationConvertor<T>()
    {}
    explicit OrientationMatrixConvertor(typename DataArray<T>::Pointer data) :
      OrientationConvertor<T>()
    {}

    virtual ~OrientationMatrixConvertor() {}

    virtual typename OrientationConvertor<T>::OrientationType getOrientationRepresentation()
    { return OrientationConvertor<T>::OrientationMatrix; }

    virtual void toEulers()
    {
      OC_CONVERT_BODY(3, Eulers, qu2eu)
    }

    virtual void toOrientationMatrix()
    {
      ////FIXME: Copy the array
    }

    virtual void toQuaternion()
    {
      OC_CONVERT_BODY(4, Quaternion, qu2om)
    }

    virtual void toAxisAngle()
    {
      OC_CONVERT_BODY(4, AxisAngle, qu2ax)
    }

    virtual void toRodrigues()
    {
      OC_CONVERT_BODY(4, Rodrigues, qu2ro)
    }

    virtual void toHomochoric()
    {
      OC_CONVERT_BODY(3, Homochoric, qu2ho)
    }

    virtual void toCubochoric()
    {
      OC_CONVERT_BODY(4, Cubochoric, qu2cu)
    }

  private:

    OrientationMatrixConvertor(const OrientationMatrixConvertor&); // Copy Constructor Not Implemented
    void operator=( const OrientationMatrixConvertor& ); // Operator '=' Not Implemented
};

template<typename T>
class AxisAngleConvertor : public OrientationConvertor<T>
{
  public:
    AxisAngleConvertor() :
      OrientationConvertor<T>()
    {}

    explicit AxisAngleConvertor(typename DataArray<T>::Pointer data) :
    OrientationConvertor<T>()
    {
      this->setInputData(data);
    }
    virtual ~AxisAngleConvertor() {}

    virtual typename OrientationConvertor<T>::OrientationType getOrientationRepresentation()
    { return OrientationConvertor<T>::AxisAngle; }

    virtual void toEulers()
    {
        OC_CONVERT_BODY(3, Eulers, ax2eu)
    }

    virtual void toOrientationMatrix()
    {
      OC_CONVERT_BODY(9, OrientationMatrix, ax2om)
    }

    virtual void toQuaternion()
    {
      OC_CONVERT_BODY(4, Quaternions, ax2qu)
    }

    virtual void toAxisAngle()
    {
        ////FIXME: Copy the array
    }

    virtual void toRodrigues()
    {
      OC_CONVERT_BODY(4, Rodrigues, ax2ro)
    }

    virtual void toHomochoric()
    {
      OC_CONVERT_BODY(3, Homochoric, ax2ho)
    }

    virtual void toCubochoric()
    {
      OC_CONVERT_BODY(4, Cubochoric, ax2cu)
    }

  private:

    AxisAngleConvertor(const AxisAngleConvertor&); // Copy Constructor Not Implemented
    void operator=(const AxisAngleConvertor&); // Operator '=' Not Implemented
};


template<typename T>
class RodriguesConvertor : public OrientationConvertor<T>
{
  public:
    RodriguesConvertor() :
      OrientationConvertor<T>()
    {}

    explicit RodriguesConvertor(typename DataArray<T>::Pointer data) :
    OrientationConvertor<T>()
    {
      this->setInputData(data);
    }
    virtual ~RodriguesConvertor() {}

    virtual typename OrientationConvertor<T>::OrientationType getOrientationRepresentation()
    { return OrientationConvertor<T>::Rodrigues; }

    virtual void toEulers()
    {
        OC_CONVERT_BODY(3, Eulers, ro2eu)
    }

    virtual void toOrientationMatrix()
    {
      OC_CONVERT_BODY(9, OrientationMatrix, ro2om)
    }

    virtual void toQuaternion()
    {
      OC_CONVERT_BODY(4, Quaternions, ro2qu)
    }

    virtual void toAxisAngle()
    {
        OC_CONVERT_BODY(4, AxisAngle, ro2qu)
    }

    virtual void toRodrigues()
    {
     ////FIXME:
    }

    virtual void toHomochoric()
    {
      OC_CONVERT_BODY(3, Homochoric, ro2ho)
    }

    virtual void toCubochoric()
    {
      OC_CONVERT_BODY(4, Cubochoric, ro2cu)
    }

  private:

    RodriguesConvertor(const RodriguesConvertor&); // Copy Constructor Not Implemented
    void operator=(const RodriguesConvertor&); // Operator '=' Not Implemented
};


template<typename T>
class HomochoricConvertor : public OrientationConvertor<T>
{
  public:
    HomochoricConvertor() :
      OrientationConvertor<T>()
    {}

    explicit HomochoricConvertor(typename DataArray<T>::Pointer data) :
    OrientationConvertor<T>()
    {
      this->setInputData(data);
    }
    virtual ~HomochoricConvertor() {}

    virtual typename OrientationConvertor<T>::OrientationType getOrientationRepresentation()
    { return OrientationConvertor<T>::Homochoric; }

    virtual void toEulers()
    {
        OC_CONVERT_BODY(3, Eulers, ho2eu)
    }

    virtual void toOrientationMatrix()
    {
      OC_CONVERT_BODY(9, OrientationMatrix, ho2om)
    }

    virtual void toQuaternion()
    {
      OC_CONVERT_BODY(4, Quaternions, ho2qu)
    }

    virtual void toAxisAngle()
    {
        OC_CONVERT_BODY(4, AxisAngle, ho2qu)
    }

    virtual void toRodrigues()
    {
      OC_CONVERT_BODY(4, Rodrigues, ho2ro)
    }

    virtual void toHomochoric()
    {
      ////FIXME:
    }

    virtual void toCubochoric()
    {
      OC_CONVERT_BODY(4, Cubochoric, ho2cu)
    }

  private:

    HomochoricConvertor(const HomochoricConvertor&); // Copy Constructor Not Implemented
    void operator=(const HomochoricConvertor&); // Operator '=' Not Implemented
};

template<typename T>
class CubochoricConvertor : public OrientationConvertor<T>
{
  public:
    CubochoricConvertor() :
      OrientationConvertor<T>()
    {}

    explicit CubochoricConvertor(typename DataArray<T>::Pointer data) :
    OrientationConvertor<T>()
    {
      this->setInputData(data);
    }
    virtual ~CubochoricConvertor() {}

    virtual typename OrientationConvertor<T>::OrientationType getOrientationRepresentation()
  { return OrientationConvertor<T>::Cubochoric; }

    virtual void toEulers()
    {
      //  OC_CONVERT_BODY(3, Eulers, cu2eu)
    }

    virtual void toOrientationMatrix()
    {
     // OC_CONVERT_BODY(9, OrientationMatrix, cu2om)
    }

    virtual void toQuaternion()
    {
      OC_CONVERT_BODY(4, Quaternions, cu2qu)
    }

    virtual void toAxisAngle()
    {
        OC_CONVERT_BODY(4, AxisAngle, cu2qu)
    }

    virtual void toRodrigues()
    {
     // OC_CONVERT_BODY(4, Rodrigues, cu2ro)
    }

    virtual void toHomochoric()
    {
      OC_CONVERT_BODY(3, Homochoric, cu2ho)
    }

    virtual void toCubochoric()
    {
      //OC_CONVERT_BODY(4, Cubochoric, cu2cu)
    }

  private:

    CubochoricConvertor(const CubochoricConvertor&); // Copy Constructor Not Implemented
    void operator=(const CubochoricConvertor&); // Operator '=' Not Implemented
};

#endif /* _OrientationConvertor_H_ */
