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
#ifndef READ_DICOM_H_
#define READ_DICOM_H_

#include <iostream>
#include <fstream>
#include <vector>

class DicomReader {

public:
  ~DicomReader() = default;

  DicomReader(const DicomReader&) = delete;
  DicomReader& operator= (const DicomReader) = delete;

  static DicomReader* GetInstance();

  void SetVoxelInfo();

  void MakeMaters();

  std::vector<int> GetVoxelNum() const;
  std::vector<double> GetVoxelLen() const;

  void ReadAllDicom(bool save = false);

  int GetPixelElement(int ix, int iy, int iz) const;
  G4Material* GetMateri(int num) const;

  void SetCarrentBoxel(int ix, int iy, int iz);
  std::vector<int> GetCarrentBoxel();

private:
  DicomReader();

  std::string path_ = "/home/yoshijun/Work/DICOM/data/ex01/";

  std::vector<int> voxel_num_;
  std::vector<double> phantom_size_;

  std::vector<int> all_elements_;

  std::vector<G4Material*> materis_;

  int ix_, iy_, iz_;
};
#endif

//=============================================================================
inline std::vector<int> DicomReader::GetVoxelNum() const
{
  return voxel_num_;
}

//-----------------------------------------------------------------------------
inline std::vector<double> DicomReader::GetVoxelLen() const
{
  return phantom_size_;
}

//-----------------------------------------------------------------------------
inline void DicomReader::SetCarrentBoxel(int ix, int iy, int iz)
{
  ix_ = ix; iy_ = iy; iz_ = iz;
}

//-----------------------------------------------------------------------------
inline std::vector<int> DicomReader::GetCarrentBoxel()
{
  std::vector<int> corrent = {ix_, iy_, iz_};

  return corrent;
}