{
  // ROOT style setting
  gStyle->SetOptStat(1111111);
  gStyle->SetOptFit(1111);

  // Add Dynamic/Include path
  {
    const TString srcRoot = "src";
    void *dirp = gSystem->OpenDirectory(srcRoot);

    if (!dirp) return;

    TString dypath = gSystem->GetDynamicPath();
    TString incpath = gSystem->GetIncludePath();

    const char *entry = nullptr;

    while ((entry = gSystem->GetDirEntry(dirp))) {
      TString name = entry;
      if (name == "." || name == "..") continue;
      TString srcdir = srcRoot + "/" + name;

      // Directory only
      FileStat_t stat;
      if (gSystem->GetPathInfo(srcdir, stat) != 0) continue;
      if (!R_ISDIR(stat.fMode)) continue;

      // Treat directories containing CMakeLists.txt
      TString cmakeFile = srcdir + "/CMakeLists.txt";
      if (gSystem->AccessPathName(cmakeFile)) continue;

      // Include path
      incpath.Append(" -I");
      incpath.Append(srcdir);

      // Dynamic library path
      TString builddir = srcdir + "/build";

      if (!gSystem->AccessPathName(builddir)) {
        dypath.Append(":");
        dypath.Append(builddir);
      }

      Info("AddUserLibraryPaths", "added user library directory: %s",srcdir.Data());
    }

    gSystem->FreeDirectory(dirp);
    gSystem->SetDynamicPath(dypath);
    gSystem->SetIncludePath(incpath);
  }

  // Load user defined libraries
  gSystem->Load("libCAT");
  gSystem->Load("libtutorial");

  // THttp server setting
  {
    if (!gSystem->AccessPathName(".webdisplay")) {

      std::ifstream fin(".webdisplay");
      int offset = 0;

      if (fin >> offset) {
        THttpServer *gHttpServer = new THttpServer(Form("http:%d", int(8090 + offset)));
      }
    }
  }

  // Resitor artemi(pow-like) command
  TCatCmdFactory *cf = TCatCmdFactory::Instance();

  cf->SetOptExactName(kFALSE);

  cf->Register(TCatCmdHelp::Instance());
  cf->Register(TCatCmdHt::Instance());
  cf->Register(TCatCmdHtp::Instance());
  cf->Register(TCatCmdHb::Instance());
  cf->Register(TCatCmdHn::Instance());
  cf->Register(TCatCmdZone::Instance());
  cf->Register(TCatCmdLs::Instance());
  cf->Register(TCatCmdCd::Instance());
  cf->Register(TCatCmdPrx::Instance());
  cf->Register(TCatCmdPry::Instance());
  cf->Register(TCatCmdAvx::Instance());
  cf->Register(TCatCmdAvy::Instance());
  cf->Register(TCatCmdBnx::Instance());
  cf->Register(TCatCmdBny::Instance());

  cf->Register(new TCatCmdLg(TCatCmdLg::kX,0));
  cf->Register(new TCatCmdLg(TCatCmdLg::kX,1));
  cf->Register(new TCatCmdLg(TCatCmdLg::kY,0));
  cf->Register(new TCatCmdLg(TCatCmdLg::kY,1));
  cf->Register(new TCatCmdLg(TCatCmdLg::kZ,0));
  cf->Register(new TCatCmdLg(TCatCmdLg::kZ,1));

  cf->Register(TCatCmdSly::Instance());
  cf->Register(TCatCmdLoopAdd::Instance());
  cf->Register(TCatCmdLoopResume::Instance());
  cf->Register(TCatCmdLoopSuspend::Instance());
  cf->Register(TCatCmdLoopTerminate::Instance());

  cf->Register(new TCatCmdHstore);

  cf->Register(TCatCmdXval::Instance());
  cf->Register(art::TCatCmdListg::Instance());

  cf->Register(new art::TCmdBranchInfo);
  cf->Register(new art::TCmdClassInfo);
  cf->Register(new art::TCmdHdel);
  cf->Register(new art::TCmdFileCd);
  cf->Register(new art::TCmdFileLs);

  cf->Register(art::TCmdPn::Instance());
  cf->Register(art::TCmdPb::Instance());
  cf->Register(art::TCmdPcd::Instance());

  cf->Register(new art::TCmdRg(art::TCmdRg::kX));
  cf->Register(new art::TCmdRg(art::TCmdRg::kY));
  cf->Register(new art::TCmdRg(art::TCmdRg::kZ));
  cf->Register(new art::TCmdSlope);

  cf->Register(art::TCmdPn::Instance());
  cf->Register(art::TCmdPb::Instance());
  cf->Register(art::TCmdPcd::Instance());
  cf->Register(art::TCmdPadZoom::Instance());

  cf->Register(new art::TCmdProcessorDescription);
  cf->Register(new art::TCmdUnZoom);
  cf->Register(new art::TCmdComment);
  cf->Register(new art::TCmdGlobalComment);

  // Define artemis canvas
  art::TCmdSave * cmdsave = art::TCmdSave::Instance();

  cmdsave->SetDefaultDirectory("figs");
  cmdsave->SetAddDateDir(kTRUE);
  cmdsave->SetAutoName(kTRUE);
  cmdsave->AddFormat("png");
  cmdsave->AddFormat("root");
  cmdsave->AddFormat("pdf",1);

  cf->Register(cmdsave);

  // Set printer command
  art::TCmdPrint *pri = new art::TCmdPrint;
  pri->SetCommand("pdf2ps %s tmp.ps && lp -o fit-to-page tmp.ps");
  //    pri->SetOption("-o fit-to-page -o number-up=2");
  cf->Register(pri);

  // Add pdg data
  TDatabasePDG* gPDGMassTable = new TDatabasePDG();
}
