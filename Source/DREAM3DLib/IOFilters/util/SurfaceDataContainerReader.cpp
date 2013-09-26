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

#include "SurfaceDataContainerReader.h"


#include <vector>

#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"

#include "DREAM3DLib/HDF5/VTKH5Constants.h"
#include "DREAM3DLib/HDF5/H5DataArrayReader.h"
#include "DREAM3DLib/DataArrays/StatsDataArray.h"

namespace Detail {
class H5GroupAutoCloser
{
public:
  H5GroupAutoCloser(hid_t* groupId) :
  gid(groupId)
  {}

  virtual ~H5GroupAutoCloser()
  {
    if (*gid > 0)
    {
      H5Gclose(*gid);
    }
  }
  private:
   hid_t* gid;
};

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceDataContainerReader::SurfaceDataContainerReader() :
EdgeDataContainerReader(),
  m_ReadFaceData(true),
  m_ReadFaceFieldData(true),
  m_ReadFaceEnsembleData(true),
  m_ReadAllArrays(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceDataContainerReader::~SurfaceDataContainerReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainerReader::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  SurfaceDataContainer* dc = SurfaceDataContainer::SafePointerDownCast(getDataContainer());
  if(NULL == dc)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  if(getHdfGroupId() < 0)
  {
    setErrorCondition(-150);
    addErrorMessage(getHumanLabel(), "The HDF5 file id was < 0. This means this value was not set correctly from the calling object.", getErrorCondition());
  }
  else if (preflight == true)
  {
    int err = gatherData(preflight);
    if (err < 0)
    {

    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainerReader::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceDataContainerReader::execute()
{
  int err = 0;

  // We are NOT going to check for NULL DataContainer because we are this far and the checks
  // have already happened. WHich is why this method is protected or private.
  SurfaceDataContainer* dc = SurfaceDataContainer::SafePointerDownCast(getDataContainer());
  if(NULL == dc)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  err = gatherData(false);
  setErrorCondition(err);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainerReader::gatherData(bool preflight)
{

  if(getHdfGroupId() < 0)
  {
    QString ss = QObject::tr(": Error opening input file");
    setErrorCondition(-150);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }

  hid_t dcGid = H5Gopen(getHdfGroupId(), getDataContainer()->getName().toLatin1().data(), H5P_DEFAULT );
  if(dcGid < 0)
  {
    QString ss = QObject::tr(": Error opening group '%1'. Is the .dream3d file a version 4 data file?").arg(DREAM3D::HDF5::EdgeDataContainerName);
    setErrorCondition(-150);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }
  Detail::H5GroupAutoCloser dcGidAutoCloser(&dcGid);

  int err = 0;

  err = gatherFaceData(dcGid, preflight);
  if (err < 0)
  {
    return err;
  }

  err = gatherFaceFieldData(dcGid, preflight);
  if (err < 0)
  {
    return err;
  }

  err = gatherFaceEnsembleData(dcGid, preflight);
  if (err < 0)
  {
    return err;
  }
  // Now finally close the group
  H5Gclose(dcGid); // Close the Data Container Group

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainerReader::gatherFaceFieldData(hid_t dcGid, bool preflight)
{
    QVector<QString> readNames;
    herr_t err = readGroupsData(dcGid, H5_FACE_FIELD_DATA_GROUP_NAME, preflight, readNames, m_FaceFieldArraysToRead);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      setErrorCondition(err);
      return -1;
    }
    return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainerReader::gatherFaceEnsembleData(hid_t dcGid, bool preflight)
{
    QVector<QString> readNames;
    herr_t err = readGroupsData(dcGid, H5_FACE_ENSEMBLE_DATA_GROUP_NAME, preflight, readNames, m_FaceEnsembleArraysToRead);
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      setErrorCondition(err);
      return -1;
    }
    return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainerReader::gatherFaceData(hid_t dcGid, bool preflight)
{
  // We are NOT going to check for NULL DataContainer because we are this far and the checks
  // have already happened. WHich is why this method is protected or private.
  SurfaceDataContainer* dc = SurfaceDataContainer::SafePointerDownCast(getDataContainer());

  int err = 0;
  QVector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size;
  if (true == preflight)
  {
    err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::FacesName, dims, type_class, type_size);
    if (err >= 0)
    {
      FaceArray::Pointer triangles = FaceArray::CreateArray(1, DREAM3D::FaceData::SurfaceMeshFaces, NULL);
      dc->setFaces(triangles);
    }
  }
  else
  {
    err = readFaces(dcGid);
  }

  // This will conditionally read all the MeshTriangleNeighborLists data if preflight is true
  err = readMeshTriangleNeighborLists(dcGid, preflight);
  if (err < 0)
  {
  }

  // Read all the Face Attribute data
  QVector<QString> readNames;
  err = readGroupsData(dcGid, H5_FACE_DATA_GROUP_NAME, preflight, readNames, m_FaceArraysToRead);
  if(err == -154) // The group was not in the file so just ignore that error
  {
    err = 0;
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainerReader::readMeshLinks(hid_t dcGid, bool preflight)
{
  // We are NOT going to check for NULL DataContainer because we are this far and the checks
  // have already happened. WHich is why this method is protected or private.
  SurfaceDataContainer* dc = SurfaceDataContainer::SafePointerDownCast(getDataContainer());
  VertexArray::Pointer verticesPtr = dc->getVertices();
  if (NULL == verticesPtr.get())
  {
    return -1;
  }

  Int32DynamicListArray::Pointer MeshLinks = Int32DynamicListArray::New();

  size_t nVerts = verticesPtr->getNumberOfTuples();
  herr_t err = 0;
  QVector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size = 0;
  err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::MeshLinksName, dims, type_class, type_size);
  if (err < 0)
  {
    return err;
  }
  else
  {
    dc->setMeshLinks(MeshLinks);
  }

  if (false == preflight && type_size > 0)
  {
    //Read the array into the buffer
    std::vector<uint8_t> buffer;
    err = QH5Lite::readVectorDataset(dcGid, DREAM3D::HDF5::MeshLinksName, buffer);
    if (err < 0)
    {
      setErrorCondition(err);
      notifyErrorMessage("Error Reading Vertex Links from Data file", getErrorCondition());
      return err;
    }
    MeshLinks->deserializeLinks(buffer, nVerts);
    dc->setMeshLinks(MeshLinks);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainerReader::readVertices(hid_t dcGid)
{
  // We are NOT going to check for NULL DataContainer because we are this far and the checks
  // have already happened. WHich is why this method is protected or private.
  SurfaceDataContainer* dc = SurfaceDataContainer::SafePointerDownCast(getDataContainer());
  herr_t err = 0;
  QVector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size;
  err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::VerticesName, dims, type_class, type_size);
  if (err < 0)
  {
    setErrorCondition(err);
    notifyErrorMessage("No Vertices Data in Data file", getErrorCondition());
    return err;
  }
  // Allocate the data
  VertexArray::Pointer verticesPtr = VertexArray::CreateArray(dims[0],  DREAM3D::VertexData::SurfaceMeshNodes);
  // Read the data
  float* data = reinterpret_cast<float*>(verticesPtr->getPointer(0));
  err = QH5Lite::readPointerDataset(dcGid, DREAM3D::HDF5::VerticesName, data);
  if (err < 0) {
    setErrorCondition(err);
    notifyErrorMessage("Error Reading Vertex List to DREAM3D file", getErrorCondition());
  }
  dc->setVertices(verticesPtr);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainerReader::readFaces(hid_t dcGid)
{
  // We are NOT going to check for NULL DataContainer because we are this far and the checks
  // have already happened. WHich is why this method is protected or private.
  SurfaceDataContainer* dc = SurfaceDataContainer::SafePointerDownCast(getDataContainer());

  herr_t err = 0;
  QVector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size;
  // Get the size of the data set so we know what to allocate
  err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::FacesName, dims, type_class, type_size);
  if (err < 0)
  {
    setErrorCondition(err);
    notifyErrorMessage("No Vertex Links in Data file", getErrorCondition());
    return err;
  }
  // Allocate the Face_t structures
  FaceArray::Pointer facesPtr = FaceArray::CreateArray(dims[0], DREAM3D::FaceData::SurfaceMeshFaces, NULL);
  // We need this to properly use QH5Lite because the data is stored as int32_t in 5 columns
  int32_t* data = reinterpret_cast<int32_t*>(facesPtr->getPointer(0));
  // Read the data from the file
  err = QH5Lite::readPointerDataset(dcGid, DREAM3D::HDF5::FacesName, data);
  if (err < 0) {
    setErrorCondition(err);
    notifyErrorMessage("Error Writing Face List to DREAM3D file", getErrorCondition());
    return err;
  }
  dc->setFaces(facesPtr);

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainerReader::readMeshTriangleNeighborLists(hid_t dcGid, bool preflight)
{
  // We are NOT going to check for NULL DataContainer because we are this far and the checks
  // have already happened. WHich is why this method is protected or private.
  SurfaceDataContainer* dc = SurfaceDataContainer::SafePointerDownCast(getDataContainer());
  FaceArray::Pointer facesPtr = dc->getFaces();
  if (NULL == facesPtr.get())
  {
    return -1;
  }

  Int32DynamicListArray::Pointer meshTriangleNeighbors = Int32DynamicListArray::New();

  size_t nFaces= facesPtr->getNumberOfTuples();
  herr_t err = 0;
  QVector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size = 0;
  err = QH5Lite::getDatasetInfo(dcGid, DREAM3D::HDF5::MeshFaceNeighborLists, dims, type_class, type_size);
  if (err < 0)
  {
    return err;
  }
  else
  {
    dc->getFaces()->setFaceNeighbors(meshTriangleNeighbors);
  }

  if(false == preflight && type_size > 0)
  {
    //Read the array into the buffer
    std::vector<uint8_t> buffer;
    err = QH5Lite::readVectorDataset(dcGid, DREAM3D::HDF5::MeshFaceNeighborLists, buffer);
    if (err < 0)
    {
      setErrorCondition(err);
      notifyErrorMessage("Error Reading Face Neighbor Links from Data file", getErrorCondition());
    }
    else
    {
      meshTriangleNeighbors->deserializeLinks(buffer, nFaces);
      dc->getFaces()->setFaceNeighbors(meshTriangleNeighbors);
    }
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceDataContainerReader::readGroupsData(hid_t dcGid, const QString &groupName, bool preflight,
                                                QVector<QString> &namesRead,
                                                QSet<QString> &namesToRead)
{

  // We are NOT going to check for NULL DataContainer because we are this far and the checks
  // have already happened. WHich is why this method is protected or private.
  SurfaceDataContainer* dc = SurfaceDataContainer::SafePointerDownCast(getDataContainer());

  int err = 0;
  //Read the Cell Data
  hid_t gid = H5Gopen(dcGid, groupName.toLatin1().data(), H5P_DEFAULT);
  if(gid < 0)
  {
    return -154;
  }

  NameListType names;
  QH5Utilities::getGroupObjects(gid, H5Utilities::H5Support_DATASET | H5Utilities::H5Support_ANY, names);
  //  qDebug() << "Number of Items in " << groupName << " Group: " << names.size() << "\n";
  QString classType;
  for (NameListType::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    QSet<QString>::iterator contains = namesToRead.find(*iter);
    if (contains == namesToRead.end() && false == preflight && m_ReadAllArrays == false) { continue; } // Do not read this item if it is NOT in the set of arrays to read
    namesRead.push_back(*iter);
    classType.clear();
    QH5Lite::readStringAttribute(gid, *iter, DREAM3D::HDF5::ObjectType, classType);
    //   qDebug() << groupName << " Array: " << *iter << " with C++ ClassType of " << classType << "\n";
    IDataArray::Pointer dPtr = IDataArray::NullPointer();

    if(classType.startsWith("DataArray") == true)
    {
      dPtr = H5DataArrayReader::readIDataArray(gid, *iter, preflight);
    }
    else if(classType.compare("StringDataArray") == 0)
    {
      dPtr = H5DataArrayReader::readStringDataArray(gid, *iter, preflight);
    }
    else if(classType.compare("vector") == 0)
    {

    }
    else if(classType.compare("NeighborList<T>") == 0)
    {
      dPtr = H5DataArrayReader::readNeighborListData(gid, *iter, preflight);
    }
    else if ( (*iter).compare(DREAM3D::EnsembleData::Statistics) == 0)
    {
      StatsDataArray::Pointer statsData = StatsDataArray::New();
      statsData->SetName(DREAM3D::EnsembleData::Statistics);
      statsData->readH5Data(gid);
      dPtr = statsData;
    }

    if (NULL != dPtr.get())
    {
      if(groupName.compare(H5_VERTEX_DATA_GROUP_NAME) == 0)
      {
        dc->addVertexData(dPtr->GetName(), dPtr);
      }
      else if(groupName.compare(H5_FACE_DATA_GROUP_NAME) == 0)
      {
       dc->addFaceData(dPtr->GetName(), dPtr);
      }
      else if(groupName.compare(H5_EDGE_DATA_GROUP_NAME) == 0)
      {
        dc->addEdgeData(dPtr->GetName(), dPtr);
      }
      else if(groupName.compare(H5_FIELD_DATA_GROUP_NAME) == 0)
      {
        dc->addFaceFieldData(dPtr->GetName(), dPtr);
      }
      else if(groupName.compare(H5_ENSEMBLE_DATA_GROUP_NAME) == 0)
      {
        dc->addFaceEnsembleData(dPtr->GetName(), dPtr);
      }
    }

  }
  H5Gclose(gid); // Close the Cell Group
  return err;
}