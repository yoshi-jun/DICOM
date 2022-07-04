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
#include<iostream>
#include<vector>

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include "g4broad_beam.h"
#include "score_edeps.h"

//-----------------------------------------------------------------------------
G4broadbeamGenerator::G4broadbeamGenerator()
{

}

//-----------------------------------------------------------------------------
G4broadbeamGenerator::~G4broadbeamGenerator()
{

}

//-----------------------------------------------------------------------------
void G4broadbeamGenerator::GeneratePrimaries(G4Event* anEvent)
{
  //Get particle-talbe pointer
  G4ParticleTable* particle_table = G4ParticleTable::GetParticleTable();

  std::string aName = "e-";
  double momentam = 20.0 * MeV;

  while (1) {

    auto rand_cos = G4UniformRand();
    auto rand_phi = G4UniformRand();

    const auto ssd = 100. * cm;
    const auto radius = 5. * std::sqrt(2.) * cm;
    const auto field_half_size = 5. * cm;

    //auto cos_theta_min = ssd / (std::sqrt(ssd * ssd + radius * radius));

    auto cos_theta = 2 * rand_cos -1 ;
                            //(1. - cos_theta_min) * rand_cos + cos_theta_min;
    auto phi = 2. * pi * rand_phi;

    auto sin_theta = std::sqrt(1. - cos_theta * cos_theta);

    auto direc_x = sin_theta * std::cos(phi);
    auto direc_y = sin_theta * std::sin(phi);
    auto direc_z = cos_theta;

    auto time_surface = ssd / direc_z;

    if (time_surface > 0) {
      auto x_surface = direc_x * time_surface;
      auto y_surface = direc_y * time_surface;

      if ( x_surface < field_half_size &&
          x_surface > -field_half_size &&
          y_surface < field_half_size &&
          y_surface > -field_half_size)
      {

        G4ThreeVector direction = {direc_x, direc_y, direc_z};

        // checke direction of broad beam
        // auto souce_score = ScoreEdeps::GetInstance();
        // souce_score-> AddPoint(x_surface, y_surface);

        G4ThreeVector momVec = momentam * direction;

        auto particle_code = particle_table-> FindParticle(aName);

        auto primary_particle = new G4PrimaryParticle{ particle_code,
                                                  momVec.x(),
                                                  momVec.y(),
                                                  momVec.z()};

        double pos_x = 0. * cm;
        double pos_y = 0. * cm;
        double pos_z = 0. * cm;
        double time_zero = 0. * s;

        auto primary_vertex = new G4PrimaryVertex{pos_x, pos_y, pos_z,
                                                  time_zero};

        primary_vertex-> SetPrimary(primary_particle);

        anEvent-> AddPrimaryVertex(primary_vertex);

        break;

      };
    };
  };

}
