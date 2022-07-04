/*==============================================================================
  BSD 2-Clause License
  Copyright (c) 2021 Junichi Yoshida
  All rights reserved.
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
  OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
==============================================================================*/
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"

#include "read_dicom.h"
#include "json.hpp"

namespace {
void print(std::string coment)
{
  std::cout << "#################################################" << std::endl
            << coment << std::endl
            << "#################################################" << std::endl;

}
}
//-----------------------------------------------------------------------------
DicomReader::DicomReader()
{
 voxel_num_.resize(3, 0);
 phantom_size_.resize(3, 0.0);
}

//-----------------------------------------------------------------------------
DicomReader* DicomReader::GetInstance()
{
  static DicomReader reader;

  return &reader;
}

//-----------------------------------------------------------------------------
void DicomReader::SetVoxelInfo()
{
  std::string filename;
  filename  = path_ + std::to_string(0) + ".json";

  std::ifstream fin(filename);
  if (!fin) {
    std::cout << "Read error DICOM data" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  nlohmann::json js;

  fin >> js;

  voxel_num_[0] = js["DICOM"]["n_x"];
  voxel_num_[1] = js["DICOM"]["n_y"];
  voxel_num_[2] = js["DICOM"]["n_z"];

  auto pix_size_x = js["DICOM"]["pixel_size_x"].get<double>();
  auto pix_size_y = js["DICOM"]["pixel_size_y"].get<double>();
  auto pix_size_z = js["DICOM"]["pixel_size_z"].get<double>();

  phantom_size_[0] = pix_size_x * voxel_num_[0];
  phantom_size_[1] = pix_size_x * voxel_num_[1];
  phantom_size_[2] = pix_size_x * voxel_num_[2];

  // all_elements_.resize(voxel_num_[0] * voxel_num_[1] * voxel_num_[2], 0);
}

//-----------------------------------------------------------------------------
void DicomReader::MakeMaters()
{
  auto max = *std::max_element(all_elements_.begin(), all_elements_.end());
  auto min = *std::min_element(all_elements_.begin(), all_elements_.end());

  for (int num = 0; num < max; num++) {
    
    auto density = (num + 1) * mg / cm3;
    auto nist_manager = G4NistManager::Instance();

    auto materi = nist_manager-> 
      BuildMaterialWithNewDensity(std::to_string(num), "G4_WATER", density);

    materis_.push_back(materi);
  }

  std::cout << "end of make materis_, size = " << materis_.size() << std::endl;
}

//-----------------------------------------------------------------------------
void DicomReader::ReadAllDicom(bool save)
{
  for (int num = 0; num < voxel_num_[2]; num ++) {
    std::string filename;
    filename  = path_ + std::to_string(num) + ".json";

    std::ifstream fin(filename);
    if (!fin) {
      std::cout << "Read error DICOM data" << std::endl;
      std::exit(EXIT_FAILURE);
    }
    
    nlohmann::json js;
    fin >> js;

    auto elements = js["DICOM"]["pixel_element"].get<std::vector<int>>();
    for (auto element : elements) {
      all_elements_.push_back(element);
    }
    
  }
  std::cout << "size of all_elements_ = " << all_elements_.size() << std::endl;
  MakeMaters();

  std::ofstream element_file("all_dicom_elements.csv", std::ios::out);

  if (save == true) {
    element_file << "ix,iy,iz,element" << std::endl;


    for ( int k = 0; k < voxel_num_[2]; k++ ) {
      for ( int j = 0; j < voxel_num_[1]; j++ ) {
        for ( int i = 0; i < voxel_num_[0]; i++ ) {

          auto idx = i + j * voxel_num_[0] + k * voxel_num_[0] * voxel_num_[1];

          element_file << i << "," << j << ","<< k << "," << all_elements_[idx]
                    <<std::endl;
        }
      }
    }
    element_file.close();
  }
}

//-----------------------------------------------------------------------------
int DicomReader::GetPixelElement(int ix, int iy, int iz) const
{
  // replace iy and ix to rotation 90 degrese
  auto idx = iy + ix * voxel_num_[1] + iz * voxel_num_[0] * voxel_num_[1];

  std::cout << ix << "," << iy << "," << iz << "," 
            << all_elements_[idx] << std::endl;
  return  all_elements_[idx];
}

//-----------------------------------------------------------------------------
G4Material* DicomReader::GetMateri(int num) const
{
  return materis_[num];
}