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

#include "beam_chooser.h"
#include "my_runaction.h"
#include "score_edeps.h"

//-----------------------------------------------------------------------------
MyRunaction::MyRunaction()
{
}

//-----------------------------------------------------------------------------
MyRunaction::~MyRunaction()
{
}

//-----------------------------------------------------------------------------
void MyRunaction::BeginOfRunAction(const G4Run *)
{
  auto dose_score = ScoreEdeps::GetInstance();
  dose_score-> InitializeDose();

  std::cout << "000000000000000000000000000000000000000000000000000000000000000"
            << std::endl
            << "                             begin run" << std::endl
            << "000000000000000000000000000000000000000000000000000000000000000"
            << std::endl;
}

//-----------------------------------------------------------------------------
void MyRunaction::EndOfRunAction(const G4Run *)
{
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Get frag infomation
  auto chooser = BeamChooser::GetInstance();
  auto flag_b = chooser-> GetChoosed();
  auto flag_s = chooser-> GetSaveCh();
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  // Get event number and initialize event number
  auto score_edeps = ScoreEdeps::GetInstance();
  int event_times = score_edeps->GetEventTimes();
  score_edeps-> InitializeEvent();

  // Get file name to save data
  auto file_name_1 = score_edeps-> GetFile1();
  auto file_name_2 = score_edeps-> GetFile2();

  // Save file
  score_edeps-> SaveToFile(file_name_1);
  score_edeps-> SavePoint(file_name_2);

  double tedep = score_edeps-> GetTotallEDeps();

  std::cout << "totall edeps =" << tedep << std::endl;

  std::cout << "000000000000000000000000000000000000000000000000000000000000000"
            << std::endl
            << "                            end run" << std::endl
            << "000000000000000000000000000000000000000000000000000000000000000"
            << std::endl;
}
