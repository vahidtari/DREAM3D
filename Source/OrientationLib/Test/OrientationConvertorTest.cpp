

#include <stdio.h>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Utilities/UnitTestSupport.hpp"

#include "OrientationLib/OrientationLib.h"
#include "OrientationLib/Math/OrientationConvertor.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void Print_QU(const T& om, typename QuaternionMath<float>::Order layout = QuaternionMath<float>::QuaternionVectorScalar)
{
  if(layout == QuaternionMath<float>::QuaternionVectorScalar)
  {
    printf("Quaternion (<vector>scalar)      :   <% 3.6f   % 3.6f    % 3.6f>    % 3.6f\n", om[0], om[1], om[2], om[3] );
  }

  else if(layout == QuaternionMath<float>::QuaternionScalarVector)
  {
    printf("Quaternion (scalar<vector>)      :   % 3.6f   <% 3.6f    % 3.6f    % 3.6f>\n", om[0], om[1], om[2], om[3] );
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestEulerConversion()
{
  size_t nTuples = 2;
  int qStride = 4;
  QVector<size_t> cDims(1, 3);
  FloatArrayType::Pointer eulers = FloatArrayType::CreateArray(nTuples, cDims, "Eulers");
  // Initialize the Eulers with some values
  eulers->setComponent(0, 0, DREAM3D::Constants::k_PiOver2);
  eulers->setComponent(0, 1, 0.0f);
  eulers->setComponent(0, 2, 0.0f);
  eulers->setComponent(1, 0, DREAM3D::Constants::k_PiOver4);
  eulers->setComponent(1, 1, 0.0f);
  eulers->setComponent(1, 2, 0.0f);

  OrientationConvertor<float>* ocEulers = new EulerConvertor<float>(eulers);
  ocEulers->convertRepresentationTo(OrientationConvertor<float>::Quaternion);

  FloatArrayType::Pointer output = ocEulers->getOutputData();

  for(size_t i = 0; i < nTuples; i++)
  {
    float* ptr = output->getPointer(i * qStride);
    Print_QU(ptr);
  }

  ocEulers->convertRepresentationTo(OrientationConvertor<float>::AxisAngle);
  output = ocEulers->getOutputData();

  for(size_t i = 0; i < nTuples; i++)
  {
    float* ptr = output->getPointer(i * qStride);
    Print_QU(ptr);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestFilterDesign()
{

  size_t nTuples = 2;
  int qStride = 4;
  QVector<size_t> cDims(1, 3);
  FloatArrayType::Pointer eulers = FloatArrayType::CreateArray(nTuples, cDims, "Eulers");
  // Initialize the Eulers with some values
  eulers->setComponent(0, 0, DREAM3D::Constants::k_PiOver2);
  eulers->setComponent(0, 1, 0.0f);
  eulers->setComponent(0, 2, 0.0f);
  eulers->setComponent(1, 0, DREAM3D::Constants::k_PiOver4);
  eulers->setComponent(1, 1, 0.0f);
  eulers->setComponent(1, 2, 0.0f);




  typedef OrientationConvertor<float> OCType;
  QVector<OCType*> converters(7);

  converters[0] = new EulerConvertor<float>();
  converters[1] = new OrientationMatrixConvertor<float>();
  converters[2] = new QuaternionConvertor<float>();
  converters[3] = new AxisAngleConvertor<float>();
  converters[4] = new RodriguesConvertor<float>();
  converters[5] = new HomochoricConvertor<float>();
  converters[6] = new CubochoricConvertor<float>();

  QVector<OCType::OrientationType> ocTypes(7);
  ocTypes[0] = OCType::Euler;
  ocTypes[1] = OCType::OrientationMatrix;
  ocTypes[2] = OCType::Quaternion;
  ocTypes[3] = OCType::AxisAngle;
  ocTypes[4] = OCType::Rodrigues;
  ocTypes[5] = OCType::Homochoric;
  ocTypes[6] = OCType::Cubochoric;


  int selectionIndex = 0;
  int outputIndex = 3;
  converters[selectionIndex]->setInputData(eulers);
  converters[selectionIndex]->convertRepresentationTo(ocTypes[outputIndex]);

  FloatArrayType::Pointer output = converters[selectionIndex]->getOutputData();

  for(size_t i = 0; i < nTuples; i++)
  {
    float* ptr = output->getPointer(i * qStride);
    Print_QU(ptr);
  }


}

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{

  int err = EXIT_SUCCESS;

  DREAM3D_REGISTER_TEST( TestEulerConversion() );
  DREAM3D_REGISTER_TEST( TestFilterDesign() );



  return err;
}
