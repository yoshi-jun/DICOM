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
#include <iostream>
#include <getopt.h>

#include "FTFP_BERT.hh"
#include "QGSP_BIC.hh"
#include "G4RunManager.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4Version.hh"
#include "G4VisExecutive.hh"
#include "CLHEP/Random/MTwistEngine.h"

#include "beam_chooser.h"
#include "geometry.h"
#include "json.hpp"
#include "read_dicom.h"
#include "score_edeps.h"
#include "user_action_initialization.h"

// #include "voxelgeom.h" //g4bench geometry

namespace {

void show_help()
{
  const char* message =
R"(
usage:
Application_Main [options]

  -h, --help                show this message
  -c, --conf <filename>     set config file
)";
  std::cout << message << std::endl;
}

} // end of namespace

//=============================================================================
int main( int argc, char** argv )
{
  // setting option
  const char* optstring = "hc:";
  const struct option long_options[] = {
  {"help",  no_argument,  0,  'h'},
  {"conf",  required_argument, 0, 'c'},
  {0, 0,  0,  0},
  };

  std::string config_name = "";

  while (1) {

    int option_index = -1;
    int c = getopt_long(argc, argv, optstring, long_options, &option_index);

    if (c == -1) {
      break;
    }

    switch (c) {
      case 'h' :
        ::show_help();
        std::exit(EXIT_SUCCESS);
      case 'c' :
        config_name = static_cast<std::string>(optarg);
        break;
    }
  }

  //---------------------------------------------------------------------------
  //setting json element
  std::ifstream fin(config_name);

  if (!fin) {
    std::cout << "You must include correct json file" <<std::endl;
    std::exit(EXIT_FAILURE);
  }
  nlohmann::json js;

  fin >> js;

  auto beam_flag = js["beam_flag"].get<std::string>();
  auto save_flag = js["save_flag"].get<std::string>();
  auto particle_name = js["particle"].get<std::string>();
  auto particle_enegy = js["kine_energy"];
  auto seed_rand = js["rand_seed"];

  // Set file name from config.json

  auto file_name_1 = js["file_name_1"].get<std::string>();
  auto file_name_2 = js["file_name_2"].get<std::string>();

  std::cout << "################################################" << std::endl
            << "filename1 = " << file_name_1 << std::endl
            << "filename2 = " << file_name_2 << std::endl
            << "################################################" << std::endl;

  //  Set boxcell numeric x,y,z
  auto reader = DicomReader::GetInstance();
  reader-> SetVoxelInfo();

  auto dim = reader-> GetVoxelNum();
  auto score_edeps = ScoreEdeps::GetInstance();
  
  score_edeps-> SetDimensions(dim[0], dim[1], dim[2]);

  // Set file name 1,2
  score_edeps-> SetFile1(file_name_1);
  score_edeps-> SetFile2(file_name_2);

  //  Set beam choser, Broad Beam = 1, Pencil Beam = 0
  auto chooser = BeamChooser::GetInstance();
  chooser-> SetChooser(beam_flag);

  // Set save chooser Save = 1, Dont Save = 0
  chooser-> SetSaveCh(save_flag);

  // Set particle info
  score_edeps-> SetParticle(particle_name);
  score_edeps-> SetParticleEnergy(particle_enegy);

  //---------------------------------------------------------------------------

  // Set random Engine
  auto rand_mtwin = new CLHEP::MTwistEngine();
  G4Random::setTheEngine(rand_mtwin);
  G4Random::setTheSeed(seed_rand);

  // Construct the default run manager
  auto runManager = new G4RunManager{};

  // Set up mandatory user initialization: Geometry
  runManager-> SetUserInitialization( new Geometry{} );
  
  // Set up mandatory user initialization: Physics-List
  runManager-> SetUserInitialization( new QGSP_BIC{} );

  // Set up user initialization: User Actions
  runManager-> SetUserInitialization( new UserActionInitialization{} );

  // Set seed for run
  G4Random::setTheSeed(seed_rand);

  // Initialize G4 kernel
  runManager-> Initialize();


  if (optind < argc) {

    auto event_num = argv[optind];
    runManager-> BeamOn(std::stoi(event_num));

  } else {
    // Create visualization environment
    auto visManager = new G4VisExecutive{};
    visManager-> Initialize();

    // Start interactive session
    auto uiExec = new G4UIExecutive( argc, argv );

    // G4UImanager*  uiManager = G4UImanager::GetUIpointer();

    // controll visual
    // uiManager->ApplyCommand( "/control/execute GlobalSetup.mac" );

    uiExec-> SessionStart();

    delete visManager;
  }

  delete runManager;

  std::exit(EXIT_SUCCESS);
}
