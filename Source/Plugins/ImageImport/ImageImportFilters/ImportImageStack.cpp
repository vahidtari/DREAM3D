/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ImportImageStack.h"

#include <string.h>

#include <limits>

#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtGui/QImage>
#include <QtGui/QImageReader>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportImageStack::ImportImageStack() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_ImageDataArrayName(DREAM3D::CellData::ImageData),
  m_ZStartIndex(0),
  m_ZEndIndex(0),
  m_TotalDigits(0),
  m_InputDir(""),
  m_FilePrefix(""),
  m_FileSuffix(""),
  m_FileExt(""),
  m_StackLowToHigh(true),
  m_StackHighToLow(false)
{

  m_Origin.x = 0.0;
  m_Origin.y = 0.0;
  m_Origin.z = 0.0;

  m_Resolution.x = 1.0;
  m_Resolution.y = 1.0;
  m_Resolution.z = 1.0;


  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportImageStack::~ImportImageStack()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportImageStack::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> options;



  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportImageStack::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setImageDataArrayName( reader->readString("ImageDataArrayName", getImageDataArrayName()) );
  setZStartIndex( reader->readValue("ZStartIndex", getZStartIndex()) );
  setZEndIndex( reader->readValue("ZEndIndex", getZEndIndex()) );
  setTotalDigits( reader->readValue("TotalDigits", getTotalDigits()) );
  setStackLowToHigh( reader->readValue("StackLowToHigh", getStackLowToHigh()) );
  setStackHighToLow( reader->readValue("StackHighToLow", getStackHighToLow()) );
  setInputDir( reader->readString("InputDir", getInputDir()) );
  setFilePrefix( reader->readString("FilePrefix", getFilePrefix()) );
  setFileSuffix( reader->readString("FileSuffix", getFileSuffix()) );
  setFileExt( reader->readString("FileExt", getFileExt()) );
  setOrigin( reader->readFloatVec3("Origin", getOrigin()) );
  setResolution( reader->readFloatVec3("Resolution", getResolution()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImportImageStack::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  writer->writeValue("ImageDataArrayName", getImageDataArrayName() );
  writer->writeValue("ZStartIndex", getZStartIndex() );
  writer->writeValue("ZEndIndex", getZEndIndex() );
  writer->writeValue("TotalDigits", getTotalDigits() );
  writer->writeValue("StackLowToHigh", getStackLowToHigh() );
  writer->writeValue("StackHighToLow", getStackHighToLow() );
  writer->writeValue("InputDir", getInputDir() );
  writer->writeValue("FilePrefix", getFilePrefix() );
  writer->writeValue("FileSuffix", getFileSuffix() );
  writer->writeValue("FileExt", getFileExt() );
  writer->writeValue("Origin", getOrigin() );
  writer->writeValue("Resolution", getResolution() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportImageStack::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->createNonPrereqDataContainer<VolumeDataContainer, ImportImageStack>(this, getDataContainerName());
  if(getErrorCondition() < 0) { return; }

  generateFileList();
  if (m_ImageFileList.size() == 0)
  {
    QString ss = QObject::tr("No files have been selected for import. Have you set the input directory?");
    setErrorCondition(-11);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else
  {
    // If we have RGB or RGBA Images then we are going to have to change things a bit.
    // We should read the file and see what we have? Of course Qt is going to read it up into
    // an RGB array by default
    int err = 0;
    QImageReader reader((m_ImageFileList[0]));
    QSize imageDims = reader.size();
    int64_t dims[3] = {imageDims.width(), imageDims.height(), m_ImageFileList.size()};
    /* Sanity check what we are trying to load to make sure it can fit in our address space.
     * Note that this does not guarantee the user has enough left, just that the
     * size of the volume can fit in the address space of the program
     */
#if   (CMP_SIZEOF_SSIZE_T==4)
    int64_t max = std::numeric_limits<size_t>::max();
#else
    int64_t max = std::numeric_limits<int64_t>::max();
#endif
    if(dims[0] * dims[1] * dims[2] > max)
    {
      err = -1;
      QString ss = QObject::tr("The total number of elements '%1' is greater than this program can hold. Try the 64 bit version.").arg((dims[0] * dims[1] * dims[2]));
      setErrorCondition(err);
      notifyErrorMessage(getHumanLabel(), ss, err);
    }

    if(dims[0] > max || dims[1] > max || dims[2] > max)
    {
      err = -1;
      QString ss = QObject::tr("One of the dimensions is greater than the max index for this sysem. Try the 64 bit version."
                               " dim[0]=%1  dim[1]=%2  dim[2]=%3").arg(dims[0]).arg(dims[1]).arg(dims[2]);
      setErrorCondition(err);
      notifyErrorMessage(getHumanLabel(), ss, err);
    }
    /* ************ End Sanity Check *************************** */

    m->setDimensions(static_cast<size_t>(dims[0]), static_cast<size_t>(dims[1]), static_cast<size_t>(dims[2]));
    m->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
    m->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);
    
    QVector<size_t> tDims(3, 0);
    for(int i=0;i<3; i++)
    {
      tDims[i] = dims[i];
    }
    AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
    if(getErrorCondition() < 0) { return; }
    QVector<size_t> arraydims(1, 1);
    // This would be for a gray scale image
    m_ImageDataPtr = cellAttrMat->createNonPrereqArray<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, m_ImageDataArrayName, 0, arraydims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_ImageDataPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_ImageData = m_ImageDataPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportImageStack::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportImageStack::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  m->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  m->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);

  UInt8ArrayType::Pointer data = UInt8ArrayType::NullPointer();

  uint8_t* imagePtr = NULL;

  int pixelBytes = 0;
  int totalPixels = 0;
  int height = 0;
  int width = 0;
  //  int bytesPerLine = 0;

  int64_t z = m_ZStartIndex;
  int64_t zSpot;
  generateFileList();
  for (QVector<QString>::iterator filepath = m_ImageFileList.begin(); filepath != m_ImageFileList.end(); ++filepath)
  {
    QString imageFName = *filepath;
    QString ss = QObject::tr("Importing file %1").arg(imageFName);
    notifyStatusMessage(getHumanLabel(), ss);

    QImage image(imageFName);
    if (image.isNull() == true)
    {
      setErrorCondition(-14000);
      notifyErrorMessage(getHumanLabel(), "Failed to load Image file", getErrorCondition());
    }
    height = image.height();
    width = image.width();
    totalPixels = width * height;
    //  bytesPerLine = image.bytesPerLine();
    // This is the first image so we need to create our block of data to store the data
    if (z == m_ZStartIndex)
    {
      m->setDimensions(width, height, m_ImageFileList.size());
      if (image.format() == QImage::Format_Indexed8)
      {
        pixelBytes = 1;
      }
      else if (image.format() == QImage::Format_RGB32 || image.format() == QImage::Format_ARGB32)
      {
        pixelBytes = 4;
      }
      QVector<size_t> compDims(1, pixelBytes);

      data = UInt8ArrayType::CreateArray(m_ImageFileList.size()*height*width, compDims, m_ImageDataArrayName);

    }

    // Get the current position in the array to copy the image into
    zSpot = (z - m_ZStartIndex);
    for(qint32 i = 0; i < height; ++i)
    {
      imagePtr = data->getPointer( (zSpot) * totalPixels * pixelBytes + i * (width * pixelBytes));
      uint8_t* source = image.scanLine(i);
      ::memcpy(imagePtr, source, width * pixelBytes);
    }

    ++z;
    if(getCancel() == true)
    {
      notifyStatusMessage(getHumanLabel(), "Conversion was Canceled");
      return;
    }
  }

  m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(data->getName(), data);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportImageStack::generateFileList()
{
  int index = 0;
  QString filename;
  m_ImageFileList.clear();
  for (int i = 0; i < (m_ZEndIndex - m_ZStartIndex) + 1; ++i)
  {
    if (m_StackLowToHigh)
    {
      index = m_ZStartIndex + i;
    }
    else
    {
      index = m_ZEndIndex - i;
    }
    filename = QString("%1%2%3.%4").arg(m_FilePrefix)
               .arg(QString::number(index), m_TotalDigits, '0')
               .arg(m_FileSuffix).arg(m_FileExt);
    QString filePath = m_InputDir + QDir::separator() + filename;
    filePath = QDir::toNativeSeparators(filePath);
    m_ImageFileList.push_back(filePath);
  }
}