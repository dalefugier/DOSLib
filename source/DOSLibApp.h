/////////////////////////////////////////////////////////////////////////////
// DOSLibApp.h
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDOSLibApp : public AcRxArxApp
{
public:
  CDOSLibApp();

  // Required overrides
  virtual AcRx::AppRetCode On_kInitAppMsg(void* pkt);
  virtual AcRx::AppRetCode On_kUnloadAppMsg(void* pkt);
  virtual void RegisterServerComponents();

  // Demand loading
  bool IsDemandLoadRegistered() const;
  bool IsDemandLoadOnStartup() const;
  void SetDemandLoadOnStartup() const;
  void SetDemandLoadOnRequest() const;

  bool GetHelpPath(CString& strHelpPath);

public:
  const wchar_t* AppName() const;
  const wchar_t* AppDescription() const;
  const wchar_t* AppVersion() const;
  const wchar_t* AppCopyright() const;
  const wchar_t* AppInternet() const;
  CString AppRegKey();

  int MajorVersion() const;
  int MinorVersion() const;
  int ServiceRelease() const;

private:
  CString m_strHelpPath;
  const int m_nMajorVersion = 9;
  const int m_nMinorVersion = 1;
  const int m_nServiceRelease = 2;
  CString m_strVersion;

public:
  // ARX commands
  static void RMA_DOSLIB_DOSLIB();
  static void RMA_DOSLIB_DOSLIBHELP();

public:
  // ADS functions

  // Drive functions
  static int ads_dos_chkdsk();
  static int ads_dos_drive();
  static int ads_dos_drivep();
  static int ads_dos_drives();
  static int ads_dos_drivetype();
  static int ads_dos_filesys();
  static int ads_dos_format();
  static int ads_dos_label();
  static int ads_dos_serialno();
  static int ads_dos_hdserialno();

  // Path functions
  static int ads_dos_absolutepath();
  static int ads_dos_compactpath();
  static int ads_dos_fullpath();
  static int ads_dos_ispathrelative();
  static int ads_dos_ispathroot();
  static int ads_dos_ispathsameroot();
  static int ads_dos_ispathunc();
  static int ads_dos_ispathurl();
  static int ads_dos_longpath();
  static int ads_dos_makepath();
  static int ads_dos_path();
  static int ads_dos_pathbackslash();
  static int ads_dos_pathextension();
  static int ads_dos_pathquotes();
  static int ads_dos_relativepath();
  static int ads_dos_shortpath();
  static int ads_dos_splitpath();
  static int ads_dos_uncpath();
  static int ads_dos_ispathnetwork();
  static int ads_dos_ispathslow();
  static int ads_dos_localpath();
  static int ads_dos_expandenv();
  static int ads_dos_unexpandenv();

  // Folder functions
  static int ads_dos_chdir();
  static int ads_dos_deltree();
  static int ads_dos_dirattrib();
  static int ads_dos_dirp();
  static int ads_dos_dirtree();
  static int ads_dos_getdir();
  static int ads_dos_mkdir();
  static int ads_dos_pwdir();
  static int ads_dos_rendir();
  static int ads_dos_rmdir();
  static int ads_dos_specialdir();
  static int ads_dos_subdir();
  static int ads_dos_sysdir();
  static int ads_dos_tempdir();
  static int ads_dos_windir();
  static int ads_dos_dirsize();

  // File functions
  static int ads_dos_attrib();
  static int ads_dos_copy();
  static int ads_dos_delete();
  static int ads_dos_dir();
  static int ads_dos_dos2unix();
  static int ads_dos_encrypt();
  static int ads_dos_file();
  static int ads_dos_fileex();
  static int ads_dos_filedate();
  static int ads_dos_filep();
  static int ads_dos_filesize();
  static int ads_dos_find();
  static int ads_dos_getfiled();
  static int ads_dos_getfilem();
  static int ads_dos_getfilenav();
  static int ads_dos_mergefiles();
  static int ads_dos_move();
  static int ads_dos_openp();
  static int ads_dos_recent();
  static int ads_dos_recycle();
  static int ads_dos_rename();
  static int ads_dos_search();
  static int ads_dos_tempfile();
  static int ads_dos_touch();
  static int ads_dos_unix2dos();
  static int ads_dos_fileinfo();
  static int ads_dos_emptyrecycle();
  static int ads_dos_filecrc();
  static int ads_dos_fileowner();
  static int ads_dos_readtextfile();
  static int ads_dos_writetextfile();
  static int ads_dos_readdelimitedfile();
  static int ads_dos_isfilename();

  // Printer functions
  static int ads_dos_getprn();
  static int ads_dos_printers();
  static int ads_dos_setprn();
  static int ads_dos_spool();

  // Configuration functions
  static int ads_dos_getini();
  static int ads_dos_regaddkey();
  static int ads_dos_regdelkey();
  static int ads_dos_regdelval();
  static int ads_dos_regenumkeys();
  static int ads_dos_regenumnames();
  static int ads_dos_reggetint();
  static int ads_dos_reggetstr();
  static int ads_dos_reggettype();
  static int ads_dos_regsetint();
  static int ads_dos_regsetstr();
  static int ads_dos_setini();

  // DEPRECATED REGISTRY FUNCTIONS
  static int ads_dos_regdel();
  static int ads_dos_regget();
  static int ads_dos_regkey();
  static int ads_dos_regset();

  // Process functions
  static int ads_dos_command();
  static int ads_dos_execute();
  static int ads_dos_exewait();
  static int ads_dos_processes();
  static int ads_dos_shellexe();

  // Dialog functions
  static int ads_dos_checklist();
  static int ads_dos_combolist();
  static int ads_dos_editbox();
  static int ads_dos_editlist();
  static int ads_dos_getcancel();
  static int ads_dos_getcolor();
  static int ads_dos_getint();
  static int ads_dos_getpassword();
  static int ads_dos_getprogress();
  static int ads_dos_getreal();
  static int ads_dos_getstring();
  static int ads_dos_htmlbox();
  static int ads_dos_htmlboxex();
  static int ads_dos_listbox();
  static int ads_dos_msgbox();
  static int ads_dos_msgboxex();
  static int ads_dos_multilist();
  static int ads_dos_popupmenu();
  static int ads_dos_proplist();
  static int ads_dos_splash();
  static int ads_dos_waitcursor();
  static int ads_dos_linetypebox();
  static int ads_dos_traywnd();
  static int ads_dos_msgboxchk();
  static int ads_dos_getdate();
  static int ads_dos_orderlist();
  static int ads_dos_duallist();
  static int ads_dos_htmldialog();

  // String functions
  static int ads_dos_strcase();
  static int ads_dos_strcat();
  static int ads_dos_strchar();
  static int ads_dos_strcompare();
  static int ads_dos_strdelete();
  static int ads_dos_strexcluding();
  static int ads_dos_strfind();
  static int ads_dos_strfindoneof();
  static int ads_dos_strincluding();
  static int ads_dos_strinsert();
  static int ads_dos_strischar();
  static int ads_dos_strleft();
  static int ads_dos_strlength();
  static int ads_dos_strmatch();
  static int ads_dos_strmid();
  static int ads_dos_strremove();
  static int ads_dos_strreplace();
  static int ads_dos_strreverse();
  static int ads_dos_strreversefind();
  static int ads_dos_strright();
  static int ads_dos_strtokens();
  static int ads_dos_strtrim();
  static int ads_dos_strtrimleft();
  static int ads_dos_strtrimright();
  static int ads_dos_strformat();
  static int ads_dos_strformatnumber();
  static int ads_dos_strformatcurrency();
  static int ads_dos_strformatdate();
  static int ads_dos_strsort();
  static int ads_dos_strcmplogical();
  static int ads_dos_strcull();
  static int ads_dos_strregexp();

  // AutoCAD functions
  static int ads_dos_acadmem();
  static int ads_dos_acitorgb();
  static int ads_dos_arxlist();
  static int ads_dos_closeall();
  static int ads_dos_cmdline();
  static int ads_dos_curvearea();
  static int ads_dos_dwgpreview();
  static int ads_dos_dwgver();
  static int ads_dos_extractpreview();
  static int ads_dos_isbreak();
  static int ads_dos_istextscr();
  static int ads_dos_isvlide();
  static int ads_dos_layerlistbox();
  static int ads_dos_lisplist();
  static int ads_dos_massprops();
  static int ads_dos_progbar();
  static int ads_dos_rgbtoaci();
  static int ads_dos_saveall();
  static int ads_dos_show();
  static int ads_dos_xreflist();
  static int ads_dos_hideobjects();
  static int ads_dos_showobjects();
  static int ads_dos_rgbtogray(); // new
  static int ads_dos_rgbtohls();
  static int ads_dos_hlstorgb();
  static int ads_dos_isinsidecurve();
  static int ads_dos_plinecentroid();
  static int ads_dos_hatcharea();
  static int ads_dos_cmdargs();
  static int ads_dos_getsecret();
  static int ads_dos_cleardbmod();
  static int ads_dos_summaryinfo();
  static int ads_dos_custominfo();
  static int ads_dos_olelist();
  static int ads_dos_imagelist();
  static int ads_dos_purgexdata();
  static int ads_dos_proxycount();
  static int ads_dos_proxyclean();
  static int ads_dos_plinewinding();
  static int ads_dos_acadname();
  static int ads_dos_slblist();

  // DOSLib functions
  static int ads_dos_about();
  static int ads_dos_demandload();
  static int ads_dos_help();
  static int ads_dos_version();

  // System functions
  static int ads_dos_adminp();
  static int ads_dos_beep();
  static int ads_dos_capslock();
  static int ads_dos_cdate();
  static int ads_dos_clipboard();
  static int ads_dos_computer();
  static int ads_dos_date();
  static int ads_dos_guidgen();
  static int ads_dos_hostname();
  static int ads_dos_htmlhelp();
  static int ads_dos_ispoweruser();
  static int ads_dos_ipaddress();
  static int ads_dos_macaddress();
  static int ads_dos_mem();
  static int ads_dos_numlock();
  static int ads_dos_pause();
  static int ads_dos_random();
  static int ads_dos_scrolllock();
  static int ads_dos_sortlist();
  static int ads_dos_speaker();
  static int ads_dos_time();
  static int ads_dos_username();
  static int ads_dos_ver();
  static int ads_dos_wav();
  static int ads_dos_winhelp();
  static int ads_dos_iswin64();
  static int ads_dos_isacad64();
  static int ads_dos_tickcount();
  static int ads_dos_nicinfo();
  static int ads_dos_printscrn();
  static int ads_dos_asynckeystate();
  static int ads_dos_systemmetrics();

  // Math functions
  static int ads_dos_abs();
  static int ads_dos_acos();
  static int ads_dos_acosh();
  static int ads_dos_asin();
  static int ads_dos_asinh();
  static int ads_dos_atan();
  static int ads_dos_atan2();
  static int ads_dos_atanh();
  static int ads_dos_ceil();
  static int ads_dos_clamp();
  static int ads_dos_cos();
  static int ads_dos_cosh();
  static int ads_dos_div();
  static int ads_dos_dtr();
  static int ads_dos_e();
  static int ads_dos_exp();
  static int ads_dos_floor();
  static int ads_dos_fmod();
  static int ads_dos_hypot();
  static int ads_dos_interp();
  static int ads_dos_log();
  static int ads_dos_log10();
  static int ads_dos_log2();
  static int ads_dos_max();
  static int ads_dos_mean();
  static int ads_dos_median();
  static int ads_dos_min();
  static int ads_dos_mode();
  static int ads_dos_modf();
  static int ads_dos_moment();
  static int ads_dos_normalize();
  static int ads_dos_parameterize();
  static int ads_dos_pi();
  static int ads_dos_pow();
  static int ads_dos_range();
  static int ads_dos_round();
  static int ads_dos_rtd();
  static int ads_dos_sin();
  static int ads_dos_sinh();
  static int ads_dos_sqrt();
  static int ads_dos_sum();
  static int ads_dos_difference();
  static int ads_dos_product();
  static int ads_dos_quotient();
  static int ads_dos_tan();
  static int ads_dos_tanh();
  static int ads_dos_trunc();
  static int ads_dos_gcd();
  static int ads_dos_lcm();
  static int ads_dos_fact();
  static int ads_dos_equal();
  static int ads_dos_sign();
  static int ads_dos_chgsign();
  static int ads_dos_copysign();
  static int ads_dos_sortnumbers();
  static int ads_dos_cullnumbers();
  static int ads_dos_permute();
  static int ads_dos_vector();
  static int ads_dos_unitize();
  static int ads_dos_length();
  static int ads_dos_negate();
  static int ads_dos_crossproduct();
  static int ads_dos_dotproduct();
  static int ads_dos_wedgeproduct();
  static int ads_dos_tripleproduct();
  static int ads_dos_perpendicularto();
  static int ads_dos_zerop();
  static int ads_dos_tinyp();
  static int ads_dos_unitp();
  static int ads_dos_parallelp();
  static int ads_dos_perpendicularp();
  static int ads_dos_orthogonalp();
  static int ads_dos_orthonormalp();
  static int ads_dos_righthandp();
  static int ads_dos_anglebetween();
  static int ads_dos_scale();
  static int ads_dos_quadratic();
  static int ads_dos_phi();
  static int ads_dos_cbrt();
  static int ads_dos_isprime();
};

CDOSLibApp& DOSLibApp();
