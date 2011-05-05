/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _ReconstructionFunc_H
#define _ReconstructionFunc_H

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include <cstddef>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <fstream>
#include <list>
#include <algorithm>
#include <numeric>

#include <boost/shared_array.hpp>

#include "MXA/MXATypes.h"
#include "MXA/Common/MXASetGetMacros.h"

#include "AIM/ANG/AbstractAngDataLoader.h"

#include "AIM/Common/AIMCommonConfiguration.h"
#include "AIM/Common/AIMArray.hpp"
#include "AIM/Common/Constants.h"
#include "AIM/Common/Grain.h"
#include "AIM/Common/Voxel.h"
#include "AIM/Common/AIMRandomNG.h"
#include "AIM/Common/HDF5/H5ReconStatsWriter.h"
#include "AIM/Common/OrientationMath.h"
#include "AIM/Common/OrientationOps/CubicOps.h"
#include "AIM/Common/OrientationOps/HexagonalOps.h"
#include "AIM/Common/OrientationOps/OrthoRhombicOps.h"

typedef boost::shared_array<double>    DoubleArray;
typedef boost::shared_array<int>       IntArray;


using namespace std;

/**
 * @class ReconstructionFunc ReconstructionFunc.h AIM/Common/ReconstructionFunc.h
 * @brief This class holds all the various functions that are needed to reconstruct
 * a 3D representation from a stack of OIM Data.
 * @author Michael A. Jackson for BlueQuartz Software
 * @author Michael A. Groeber (US Air Force Research Laboratory)
 * @version 2.0
 */
class AIMCOMMON_EXPORT ReconstructionFunc
{

  public:

    MXA_SHARED_POINTERS(ReconstructionFunc)
    MXA_STATIC_NEW_MACRO(ReconstructionFunc)

    virtual ~ReconstructionFunc();

    typedef AIMArray<double> DoubleArrayType;
    typedef AIMArray<int>    IntArrayType;

    double sizex;
    double sizey;
    double sizez;

    double resx;
    double resy;
    double resz;

    double misorientationtolerance;
    double minseedconfidence;
    double minseedimagequality;
    double downsamplefactor;
    double sizebinstepsize;
    int minallowedgrainsize;
    int mergetwinsoption;
    int mergecoloniesoption;
    vector<AIM::Reconstruction::CrystalStructure> crystruct;
    int alignmeth;
    int alreadyformed;


    AIMRandomNG rg;
    boost::shared_array<Voxel> voxels;

    std::vector<Grain::Pointer> m_Grains;

    IntArray graincounts;

    vector<vector<double> > graincenters;
    vector<vector<double> > grainmoments;
    DoubleArrayType::Pointer         m_grainQuats;

    int numseNbins;
    int numorients;
    int numeulers;
 //   int numgrains;
    vector<double> totalsurfacearea;
    vector<double> phasefraction;
    vector<double> totalvol;
    vector<double> totalaxes;
    vector<int> maxdiameter;
    vector<int> mindiameter;
    double unbiasedvol;
    int cutoutxsize;
    int cutoutysize;
    int cmaxx;
    int cminx;
    int cmaxy;
    int cminy;
    int xpoints;
    int ypoints;
    int tempxpoints;
    int tempypoints;
    int zpoints;
    int totalpoints;
    int totaltemppoints;
    int numneighbins;


    void initialize(int nX, int nY, int nZ,
                  double xRes, double yRes, double zRes, bool v_mergetwinsoption, bool v_mergecoloniesoption,
				  int v_minallowedgrainsize, double v_minseedconfidence, double v_downsamplefactor,
				  double v_minseedimagequality, double v_misorientationtolerance, double v_sizebinstepsize,
				  vector<AIM::Reconstruction::CrystalStructure> v_crystruct, int v_alignmeth, bool v_alreadyformed);


	void initializeQuats();
    void find_border();
    void form_grains();
    void form_grains_sections();
    void remove_smallgrains();
    int renumber_grains1();
    int load_data(const std::string &filename);
    void assign_badpoints();
    void find_neighbors();
    void define_neighborhood();
    void merge_containedgrains();
    void reorder_grains(const std::string &reconVisFile);
    int remove_smallgrains(size_t numgrains);
	  void fillin_sample();
    void cleanup_data();
    void find_grain_and_kernel_misorientations();
    void merge_twins();
    void merge_colonies();
    void characterize_twins();
    void characterize_colonies();
    void renumber_grains3();
    void find_euclidean_map();

    void find_vectors(H5ReconStatsWriter::Pointer h5io);
    void find_centroids();
    void find_moments();
    void find_axes();
    void find_colors();
    void find_schmids();

    void find_vectors2D(H5ReconStatsWriter::Pointer h5io);
    void find_centroids2D();
    void find_moments2D();
    void find_axes2D();

    void deformation_stats(const std::string &filename, const std::string &filename2);
    void write_graindata(const std::string &graindataFile);
    void align_sections(const std::string &filename);

    void find_eulerodf(H5ReconStatsWriter::Pointer h5io);
    void measure_misorientations(H5ReconStatsWriter::Pointer h5io);
    int volume_stats(H5ReconStatsWriter::Pointer h5io);
    int volume_stats2D(H5ReconStatsWriter::Pointer h5io);

    double gamma(double x);
    double find_xcoord(size_t index);
    double find_ycoord(size_t index);
    double find_zcoord(size_t index);

  protected:
    ReconstructionFunc();

  private:
    std::vector<OrientationMath*>    m_OrientatioOps;
    OrientationMath::Pointer                m_CubicOps;
    OrientationMath::Pointer            m_HexOps;
    OrientationMath::Pointer         m_OrthoOps;

    ReconstructionFunc(const ReconstructionFunc&);    // Copy Constructor Not Implemented
    void operator=(const ReconstructionFunc&);  // Operator '=' Not Implemented
};


#endif
