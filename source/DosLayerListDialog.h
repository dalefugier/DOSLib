/////////////////////////////////////////////////////////////////////////////
// DosLayerListDialog.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

#include "DosColorListBox.h"

class CDosLayerListDialog : public CAcUiDialog
{
public:
  CDosLayerListDialog(CWnd* pParent);

  //{{AFX_DATA(CDosLayerListDialog)
  enum { IDD = IDD_DOSLAYERLISTBOX };
  CDosColorListBox m_listbox;
  CButton m_ok;

  //}}AFX_DATA
  CString m_layer;
  CStringArray m_layers;

  void SetTitle(LPCTSTR lp);
  void SetPrompt(LPCTSTR lp);

  bool MultipleSelection();
  void SetMultipleSelection(bool b = true);

  void HideOnLayers(bool b = true);
  void HideOffLayers(bool b = true);
  void HideUnlockedLayers(bool b = true);
  void HideLockedLayers(bool b = true);
  void HideThawedLayers(bool b = true);
  void HideFrozenLayers(bool b = true);
  void HideStdLayers(bool b = true);
  void HideXrefLayers(bool b = true);
  void HidePlotLayers(bool b = true);
  void HideNoPlotLayers(bool b = true);
  void HideUsedLayers(bool b = true);
  void HideUnusedLayers(bool b = true);

  //{{AFX_VIRTUAL(CDosLayerListDialog)
protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  //}}AFX_VIRTUAL

protected:
  void FillListBox();
  BOOL RecreateListBox(CListBox* pList, LPVOID lpParam = NULL);

  bool ProcessLayers();
  bool GetLayerName(const AcDbObjectId& id, CString& strName);

  bool IsUsedLayer(const wchar_t* name);
  bool IsUnusedLayer(const wchar_t* name);

  //{{AFX_MSG(CDosLayerListDialog)
  virtual BOOL OnInitDialog();
  afx_msg void OnSelChangeList();
  afx_msg void OnDblClkList();
  virtual void OnOK();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

  CString m_title;
  CString m_prompt;

  bool m_hide_on_layers;
  bool m_hide_off_layers;

  bool m_hide_unlocked_layers;
  bool m_hide_locked_layers;

  bool m_hide_thawed_layers;
  bool m_hide_frozen_layers;

  bool m_hide_std_layers;
  bool m_hide_xref_layers;

  bool m_hide_plot_layers;
  bool m_hide_noplot_layers;

  bool m_multiple_selection;

  bool m_hide_used_layers;
  bool m_hide_unused_layers;

  CStringArray m_used_layer_names;
  CStringArray m_unused_layer_names;
};

//{{AFX_INSERT_LOCATION}}
