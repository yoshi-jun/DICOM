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
#include <fstream>
#include <iostream>

#include "G4SystemOfUnits.hh"

#include "score_edeps.h"

//----------------------------------------------------------------------------
ScoreEdeps::ScoreEdeps()
  : nx_{1}, ny_{1}, nz_{1}
{
  event_counter_ = 0;
  totall_edeps_ = 0;
}

//----------------------------------------------------------------------------
ScoreEdeps* ScoreEdeps::GetInstance()
{
  static ScoreEdeps the_score_edeps;

  return &the_score_edeps;
}

//----------------------------------------------------------------------------
void ScoreEdeps::InitializeDose()
{
  dose_list_.resize(nx_ * ny_ * nz_ , 0.);
}

//----------------------------------------------------------------------------
void ScoreEdeps::AddDose(int ix, int iy, int iz, double val)
{
  auto idx = ix + iy * nx_ + iz * nx_ * ny_ ;

  dose_list_[idx] += val / MeV;
}

//----------------------------------------------------------------------------
void ScoreEdeps::Print() const
{
  for ( int k = 0; k < nz_; k++ ) {
    for ( int i = 0; i < nx_; i++ ) {
      for ( int j = 0; j < ny_; j++ ) {

        int idx = i + j * nx_ + k * nx_ * ny_;

        std::cout << dose_list_ [idx] / MeV << "," ;

      }

      std::cout << std::endl;
    }

    for (int n = 0; n < nx_; n++) {
      std::cout << "-" ;
    }
    std::cout << std::endl;
    std::cout << "z = " << k << std::endl;
  }
}

//----------------------------------------------------------------------------
void ScoreEdeps::SaveToFile(const std::string &filename) const
{
  std::ofstream dose_file_(filename, std::ios::out);

  dose_file_ << "ix,iy,iz,dose" << std::endl;


  for ( int k = 0; k < nz_; k++ ) {
    for ( int j = 0; j < ny_; j++ ) {
      for ( int i = 0; i < nx_; i++ ) {

        auto idx = i + j * nx_ + k * nx_ * ny_ ;

        dose_file_ << i << "," << j << ","<< k << "," << dose_list_ [idx] / MeV
                   <<std::endl;
      }
    }
  }
  dose_file_.close();
}

//----------------------------------------------------------------------------
void ScoreEdeps::AddPoint(double point_x, double point_y, double point_z)
{
  points_list_x_.push_back(point_x);
  points_list_y_.push_back(point_y);
  points_list_z_.push_back(point_z);
}

//----------------------------------------------------------------------------
void ScoreEdeps::SavePoint(const std::string& filename) const
{
  std::ofstream point_file(filename, std::ios::out);

  point_file << "x,y,z" << std::endl;

  for (int i = 0; i < points_list_x_.size(); i++) {

    point_file << points_list_x_[i] << ","
               << points_list_y_[i] << ","
               << points_list_z_[i] << std::endl;

  }

  point_file.close();
}

//-----------------------------------------------------------------------------
void ScoreEdeps::AddKineE(double val)
{
  kine_e_.push_back(val);
}

//-----------------------------------------------------------------------------
void ScoreEdeps::SaveKineE(const std::string& filename) const
{
  std::ofstream kine_file(filename, std::ios::out);

  for (int i; i < kine_e_.size(); i++) {
    kine_file << kine_e_[i] << std::endl;
  }

  kine_file.close();
}
