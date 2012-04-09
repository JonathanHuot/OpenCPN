/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Console Canvas
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register   *
 *   bdbcat@yahoo.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.             *
 ***************************************************************************
 *
 *
 *
 */


#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "dychart.h"

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "wx/datetime.h"

#include "chart1.h"
#include "concanv.h"
#include "routeman.h"
#include "navutil.h"

#ifdef __WXMAC__
#include "wx/mac/private.h"
#endif

extern Routeman         *g_pRouteMan;
extern FontMgr          *pFontMgr;
extern MyFrame          *gFrame;

extern                  double gCog;
extern                  double gSog;



//------------------------------------------------------------------------------
//    ConsoleCanvas Implementation
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(ConsoleCanvas, wxWindow)
            EVT_PAINT(ConsoleCanvas::OnPaint)
            EVT_BUTTON(ID_LEGROUTE, ConsoleCanvas::OnLegRouteButton)
            EVT_MOUSE_EVENTS(ConsoleCanvas::MouseEvent)
            EVT_MOUSE_CAPTURE_LOST(ConsoleCanvas::MouseLostCaptureEvent)

END_EVENT_TABLE()

// Define a constructor for my canvas
ConsoleCanvas::ConsoleCanvas(wxWindow *frame):
            wxDialog(frame, wxID_ANY, _T(""), wxPoint(-1, -1), wxSize(-1, -1),wxNO_BORDER | wxCLIP_CHILDREN | wxSTAY_ON_TOP )
{

#ifdef __WXMAC__        //This does not work.....
//      ChangeWindowAttributes( (WindowRef)m_macWindow, kWindowNoShadowAttribute, 0 );
//      ChangeWindowAttributes( (WindowRef)m_macWindow, kWindowHideOnSuspendAttribute, 0);
#endif
      m_pParent = frame;

      pThisLegBox = new wxStaticBox(this, -1, _("This Leg"), wxPoint(1,1),
                                    wxSize(170,200), wxCLIP_CHILDREN, _T("staticBox"));

      m_pitemStaticBoxSizerLeg = new wxStaticBoxSizer(pThisLegBox, wxVERTICAL);

 //     pSBoxRgn = new wxRegion(pThisLegBox->GetRect() );

      pThisLegFont = wxTheFontList->FindOrCreateFont(12, wxDEFAULT,wxNORMAL, wxBOLD, FALSE,
              wxString(_T("Eurostile Extended")));

      pThisLegBox->SetFont(*pThisLegFont);


      m_pLegRouteButton = new wxButton( this, ID_LEGROUTE, _("Leg/Route"), wxDefaultPosition, wxSize(-1, -1), 0 );
      m_pLegRouteButton->SetMinSize(wxSize(-1, 25));
      m_pitemStaticBoxSizerLeg->Add(m_pLegRouteButton, 0, wxALIGN_LEFT|wxALL|wxEXPAND, 2);


      pXTE = new AnnunText(this, -1,  _("Console Legend"), _("Console Value"));
      pXTE->SetALabel(_T("XTE"));
      m_pitemStaticBoxSizerLeg->Add(pXTE, 1, wxALIGN_LEFT|wxALL, 2);

      pBRG = new AnnunText(this, -1, _("Console Legend"), _("Console Value"));
      pBRG->SetALabel(_T("BRG"));
      m_pitemStaticBoxSizerLeg->Add(pBRG, 1, wxALIGN_LEFT|wxALL, 2);

      pVMG = new AnnunText(this, -1, _("Console Legend"), _("Console Value"));
      pVMG->SetALabel(_T("VMG"));
      m_pitemStaticBoxSizerLeg->Add(pVMG, 1, wxALIGN_LEFT|wxALL, 2);

      pRNG = new AnnunText(this, -1, _("Console Legend"), _("Console Value"));
      pRNG->SetALabel(_T("RNG"));
      m_pitemStaticBoxSizerLeg->Add(pRNG, 1, wxALIGN_LEFT|wxALL, 2);

      pTTG = new AnnunText(this, -1,  _("Console Legend"), _("Console Value"));
      pTTG->SetALabel(_T("TTG"));
      m_pitemStaticBoxSizerLeg->Add(pTTG, 1, wxALIGN_LEFT|wxALL, 2);


//    Create CDI Display Window


      pCDI = new CDI(this, -1, wxSIMPLE_BORDER, _T("CDI"));
      m_pitemStaticBoxSizerLeg->AddSpacer(10);
      m_pitemStaticBoxSizerLeg->Add(pCDI, 0, wxALIGN_LEFT|wxALL|wxEXPAND, 2);


      m_bShowRouteTotal = false;

      SetSizer( m_pitemStaticBoxSizerLeg );      // use the sizer for layout
      m_pitemStaticBoxSizerLeg->SetSizeHints(this);
      Layout();
      Fit();

      Hide();
}


 ConsoleCanvas::~ConsoleCanvas()
{
      delete pCDI;

}


void ConsoleCanvas::SetColorScheme(ColorScheme cs)
{
    pbackBrush = wxTheBrushList->FindOrCreateBrush(GetGlobalColor(_T("DILG1"/*UIBDR*/)), wxSOLID);
    SetBackgroundColour(GetGlobalColor(_T("DILG1"/*"UIBDR"*/)));

    //  Also apply color scheme to all known children

    pThisLegBox->SetBackgroundColour(GetGlobalColor(_T("DILG1"/*"UIBDR"*/)));

    m_pLegRouteButton->SetBackgroundColour(GetGlobalColor(_T("DILG1"/*"UIBDR"*/)));

    pXTE->SetColorScheme(cs);
    pBRG->SetColorScheme(cs);
    pRNG->SetColorScheme(cs);
    pTTG->SetColorScheme(cs);
    pVMG->SetColorScheme(cs);

    pCDI->SetColorScheme(cs);
}


void ConsoleCanvas::OnPaint(wxPaintEvent& event)
{
      int x,y;
      GetClientSize(&x, &y);
      wxString str_buf;

      wxPaintDC dc(this);

      if(g_pRouteMan->GetpActiveRoute())
      {
            if(m_bNeedClear)
            {
//                  ClearBackground();
                  pThisLegBox->Refresh();
                  m_bNeedClear = false;
            }

            UpdateRouteData();
      }
}


void ConsoleCanvas::UpdateRouteData()
{
      wxString str_buf;

      if(g_pRouteMan->GetpActiveRoute())
      {

            if(g_pRouteMan->m_bDataValid)
            {

//    Range
                  wxString srng;
                  float rng = g_pRouteMan->GetCurrentRngToActivePoint();
                  float nrng = g_pRouteMan->GetCurrentRngToActiveNormalArrival();

//                  if((fabs(rng - nrng) > .01) && (rng < 10.0))
                  double deltarng = fabs(rng - nrng);
                  if((deltarng > .01) && ((deltarng / rng) > .10) && (rng < 10.0))        // show if there is more than 10% difference in ranges, etc...
                  {
                        if(nrng < 10.0)
                              srng.Printf(_T("%5.2f/%5.2f"), rng, nrng );
                        else
                              srng.Printf(_T("%5.1f/%5.1f"), rng, nrng );
                  }
                  else
                  {
                        if(rng < 10.0)
                              srng.Printf(_T("%6.2f"), rng );
                        else
                              srng.Printf(_T("%6.1f"), rng );
                  }


                  if(!m_bShowRouteTotal)
                        pRNG->SetAValue(srng);

//    Brg
                  float dcog = g_pRouteMan->GetCurrentBrgToActivePoint();
                  if(dcog >= 359.5)
                        dcog = 0;
                  str_buf.Printf(_T("%6.0f"), dcog);
                  pBRG->SetAValue(str_buf);

//    XTE
                  str_buf.Printf(_T("%6.2f"), g_pRouteMan->GetCurrentXTEToActivePoint());
                  pXTE->SetAValue(str_buf);
                  if(g_pRouteMan->GetXTEDir() < 0)
                        pXTE->SetALabel(wxString(_("XTE         L")));
                  else
                        pXTE->SetALabel(wxString(_("XTE         R")));

//    VMG
                  // VMG is always to next waypoint, not to end of route
                  // VMG is SOG x cosine (difference between COG and BRG to Waypoint)
                  double VMG = 0.;
                  if(!wxIsNaN(gCog) && !wxIsNaN(gSog))
                  {
                        double BRG;
                        BRG = g_pRouteMan->GetCurrentBrgToActivePoint();
                        VMG = gSog * cos((BRG-gCog) *PI/180.);
                        str_buf.Printf(_T("%6.2f"), VMG);
                  }
                  else
                        str_buf = _T("---");

                  pVMG->SetAValue(str_buf);

//    TTG
                  // In all cases, ttg/eta are declared invalid if VMG <= 0.

                  // If showing only "this leg", use VMG for calculation of ttg
                  wxString ttg_s;
                  if((VMG > 0.) && !wxIsNaN(gCog) && !wxIsNaN(gSog))

                  {
                        float ttg_sec = (rng / VMG) * 3600.;
                        wxTimeSpan ttg_span(0, 0, long(ttg_sec), 0);
                        ttg_s = ttg_span.Format();
                  }
                  else
                        ttg_s = _T("---");

                  if(!m_bShowRouteTotal)
                        pTTG->SetAValue(ttg_s);

      //    Remainder of route
                  float trng = rng;

                  Route *prt = g_pRouteMan->GetpActiveRoute();
                  wxRoutePointListNode *node = (prt->pRoutePointList)->GetFirst();
                  RoutePoint *prp;

                  int n_addflag = 0;
                  while(node)
                  {
                        prp = node->GetData();
                        if(n_addflag)
                              trng += prp->m_seg_len;

                        if(prp == prt->m_pRouteActivePoint)
                              n_addflag ++;

                        node=node->GetNext();
                  }

//                total rng
                  wxString strng;
                  if(trng < 10.0)
                        strng.Printf(_T("%6.2f"), trng );
                  else
                        strng.Printf(_T("%6.1f"), trng );

                  if(m_bShowRouteTotal)
                        pRNG->SetAValue(strng);

//                total ttg
                  // If showing total route ttg/ETA, use gSog for calculation

                  wxString tttg_s;
                  wxTimeSpan tttg_span;
                  if(VMG > 0.)
                  {
                        float tttg_sec = (trng / gSog) * 3600.;
                        tttg_span = wxTimeSpan::Seconds((long)tttg_sec);
                        tttg_s = tttg_span.Format();
                  }
                  else
                  {
                        tttg_span = wxTimeSpan::Seconds(0);
                        tttg_s = _T("---");
                  }

                  if(m_bShowRouteTotal)
                        pTTG->SetAValue(tttg_s);

//                total ETA to be shown on XTE panel
                  if(m_bShowRouteTotal)
                  {
                        wxDateTime dtnow, eta;
                        dtnow.SetToCurrent();
                        eta = dtnow.Add(tttg_span);
                        wxString seta;

                        if(VMG > 0.)
                              seta = eta.Format(_T("%H:%M"));
                        else
                              seta = _T("---");

                        pXTE->SetAValue(seta);
                        pXTE->SetALabel(wxString(_("ETA          ")));
                  }


                  pRNG->Refresh();
                  pBRG->Refresh();
                  pVMG->Refresh();
                  pTTG->Refresh();
                  pXTE->Refresh();
            }
      }
}

void ConsoleCanvas::RefreshConsoleData(void)
{
      UpdateRouteData();

      pRNG->Refresh();
      pBRG->Refresh();
      pVMG->Refresh();
      pTTG->Refresh();
      pXTE->Refresh();
      pCDI->Refresh();
}



void ConsoleCanvas::OnLegRouteButton(wxCommandEvent& event)
{
      m_bShowRouteTotal = !m_bShowRouteTotal;
      if(m_bShowRouteTotal)
            pThisLegBox->SetLabel(_("Route"));
      else
            pThisLegBox->SetLabel(_("This Leg"));

      pThisLegBox->Refresh(true);

      RefreshConsoleData();       // to pick up changes in the annunciator contents
}

void ConsoleCanvas::MouseEvent(wxMouseEvent& event)
{
      m_pParent->SetCursor ( wxCURSOR_ARROW );

      int x,y;
      event.GetPosition(&x, &y);


//    Check the region of the Route/Leg button
#ifdef __WXMSW__
      if(event.LeftDown())
      {
            wxRegion rg(m_pLegRouteButton->GetRect());
            if(rg.Contains(x,y)  == wxInRegion)
            {
                  m_bShowRouteTotal = !m_bShowRouteTotal;
                  if(m_bShowRouteTotal)
                        pThisLegBox->SetLabel(_("Route"));
                  else
                        pThisLegBox->SetLabel(_("This Leg"));

                  pThisLegBox->Refresh(true);
            }
      }

///  Why is this necessary???
//    Because of the CaptureMouse call in chcanv.cpp when mouse enters concanv region

///   Became unnecessary when console became a wxDialog
/*
      wxRect rr = GetRect();
      if(!rr.Contains(x + rr.x, y + rr.y) )
            ReleaseMouse();
*/
#endif


}

void ConsoleCanvas::MouseLostCaptureEvent(wxMouseCaptureLostEvent& event)
{
}

void ConsoleCanvas::ShowWithFreshFonts(void)
{
      Hide();
      Move(0,0);

      UpdateFonts();
      gFrame->PositionConsole();
      Show();

}

void ConsoleCanvas::UpdateFonts(void)
{
      pBRG->RefreshFonts();
      pXTE->RefreshFonts();
      pTTG->RefreshFonts();
      pRNG->RefreshFonts();
      pVMG->RefreshFonts();

      m_pitemStaticBoxSizerLeg->SetSizeHints(this);
      Layout();
      Fit();

      Refresh();
}



//------------------------------------------------------------------------------
//    AnnunText Implementation
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(AnnunText, wxWindow)
      EVT_PAINT(AnnunText::OnPaint)
END_EVENT_TABLE()


AnnunText::AnnunText(wxWindow *parent, wxWindowID id, const wxString& LegendElement, const wxString& ValueElement):
            wxWindow(parent, id, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER/*wxSUNKEN_BORDER*/)
{
      m_label = _T("Label");
      m_value = _T("-----");

      m_plabelFont = wxTheFontList->FindOrCreateFont(14, wxFONTFAMILY_SWISS,wxNORMAL, wxBOLD, FALSE,
              wxString(_T("Arial Bold")));
      m_pvalueFont = wxTheFontList->FindOrCreateFont(24, wxFONTFAMILY_DEFAULT,wxNORMAL, wxBOLD, FALSE,
              wxString(_T("helvetica")), wxFONTENCODING_ISO8859_1);


      m_LegendTextElement = LegendElement;
      m_ValueTextElement =  ValueElement;

      RefreshFonts();
}

AnnunText::~AnnunText()
{
}


void AnnunText::CalculateMinSize(void)
{
            //    Calculate the minimum required size of the window based on text size

      int wl = 50;            // reasonable defaults?
      int hl = 20;
      int wv = 50;
      int hv = 20;

      if(m_plabelFont)
            GetTextExtent(_T("1234"), &wl, &hl, NULL, NULL, m_plabelFont);


      if(m_pvalueFont)
            GetTextExtent(_T("123.456"), &wv, &hv, NULL, NULL, m_pvalueFont);

      wxSize min;
      min.x = wl + wv;
      min.y = (int)((hl + hv) * 1.2);

      SetMinSize(min);
}

void AnnunText::SetColorScheme(ColorScheme cs)
{
      m_pbackBrush = wxTheBrushList->FindOrCreateBrush(GetGlobalColor(_T("UBLCK"/*"UIBCK"*/)), wxSOLID);

      m_text_color = GetGlobalColor(_T("GREEN4"/*"UINFD"*/));
}



void AnnunText::RefreshFonts()
{
      m_plabelFont = pFontMgr->GetFont(m_LegendTextElement);
      m_pvalueFont = pFontMgr->GetFont(m_ValueTextElement);

      CalculateMinSize();

}

void AnnunText::SetLegendElement(const wxString &element)
{
      m_LegendTextElement =  element ;
}

void AnnunText::SetValueElement(const wxString &element)
{
      m_ValueTextElement = element;
}




void AnnunText::SetALabel(const wxString &l)
{
      m_label = l;
}

void AnnunText::SetAValue(const wxString &v)
{
      m_value = v;
}

void AnnunText::OnPaint(wxPaintEvent& event)
{
      int sx,sy;
      GetClientSize(&sx, &sy);

      //    Do the drawing on an off-screen memory DC, and blit into place
      //    to avoid objectionable flashing
      wxMemoryDC mdc;

      wxBitmap m_bitmap(sx, sy, -1);
      mdc.SelectObject(m_bitmap);
      mdc.SetBackground(*m_pbackBrush);
      mdc.Clear();

      mdc.SetTextForeground(m_text_color);

      if(m_plabelFont)
      {
            mdc.SetFont(*m_plabelFont);
            mdc.DrawText(m_label, 5, 2);
      }

      if(m_pvalueFont)
      {
            mdc.SetFont(*m_pvalueFont);

            int w, h;
            mdc.GetTextExtent(m_value, &w, &h);
            int cw, ch;
            mdc.GetSize(&cw, &ch);

            mdc.DrawText(m_value, cw - w - 2, ch - h - 2);
      }

      wxPaintDC dc(this);
      dc.Blit(0, 0, sx, sy, &mdc, 0, 0);

}





//------------------------------------------------------------------------------
//    CDI Implementation
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(CDI, wxWindow)
      EVT_PAINT(CDI::OnPaint)
END_EVENT_TABLE()


CDI::CDI(wxWindow *parent, wxWindowID id, long style, const wxString& name):
            wxWindow(parent, id, wxDefaultPosition, wxDefaultSize, style, name)

{
      SetMinSize(wxSize(10,150));
}

void CDI::SetColorScheme(ColorScheme cs)
{
    m_pbackBrush = wxTheBrushList->FindOrCreateBrush(GetGlobalColor(_T("DILG2")), wxSOLID);
    m_proadBrush = wxTheBrushList->FindOrCreateBrush(GetGlobalColor(_T("DILG1")), wxSOLID);
    m_proadPen = wxThePenList->FindOrCreatePen(GetGlobalColor(_T("CHBLK")), 1, wxSOLID);
}


void CDI::OnPaint(wxPaintEvent& event)
{
      int sx,sy;
      GetClientSize(&sx, &sy);

      //    Do the drawing on an off-screen memory DC, and blit into place
      //    to avoid objectionable flashing
      wxMemoryDC mdc;

      wxBitmap m_bitmap(sx, sy, -1);
      mdc.SelectObject(m_bitmap);
      mdc.SetBackground(*m_pbackBrush);
      mdc.Clear();

      int xp = sx/2;
      int yp = sy*9/10;

      int path_length = sy * 3;
      int pix_per_xte = 120;

      if(g_pRouteMan->GetpActiveRoute())
      {
            double angle = 90 - (g_pRouteMan->GetCurrentSegmentCourse() - gCog);

            double dy = path_length * sin(angle *PI/180.);
            double dx = path_length * cos(angle *PI/180.);

            int xtedir;
            xtedir = g_pRouteMan->GetXTEDir();
            double xte = g_pRouteMan->GetCurrentXTEToActivePoint();

            double ddy = xtedir * pix_per_xte * xte * sin((90 - angle) *PI/180.);
            double ddx = xtedir * pix_per_xte * xte * cos((90 - angle) *PI/180.);

            int ddxi = (int)ddx;
            int ddyi = (int)ddy;

            int xc1 = xp - (int)(dx/2) + ddxi;
            int yc1 = yp + (int)(dy/2) + ddyi;
            int xc2 = xp + (int)(dx/2) + ddxi;
            int yc2 = yp - (int)(dy/2) + ddyi;


            wxPoint road[4];

            int road_top_width = 10;
            int road_bot_width = 40;

            road[0].x = xc1 - (int)(road_bot_width * cos((90 - angle) *PI/180.));
            road[0].y = yc1 - (int)(road_bot_width * sin((90 - angle) *PI/180.));

            road[1].x = xc2 - (int)(road_top_width * cos((90 - angle) *PI/180.));
            road[1].y = yc2 - (int)(road_top_width * sin((90 - angle) *PI/180.));

            road[2].x = xc2 + (int)(road_top_width * cos((90 - angle) *PI/180.));
            road[2].y = yc2 + (int)(road_top_width * sin((90 - angle) *PI/180.));

            road[3].x = xc1 + (int)(road_bot_width * cos((90 - angle) *PI/180.));
            road[3].y = yc1 + (int)(road_bot_width * sin((90 - angle) *PI/180.));

            mdc.SetBrush(*m_proadBrush);
            mdc.SetPen(*m_proadPen);
            mdc.DrawPolygon(4, road,0,0,wxODDEVEN_RULE);

            mdc.DrawLine(xc1, yc1, xc2, yc2);

            mdc.DrawLine(0, yp, sx, yp);
            mdc.DrawCircle(xp,yp,6);
            mdc.DrawLine(xp, yp+5, xp, yp-5);
      }

      wxPaintDC dc(this);
      dc.Blit(0, 0, sx, sy, &mdc, 0, 0);
}



