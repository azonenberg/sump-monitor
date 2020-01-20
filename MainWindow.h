/***********************************************************************************************************************
*                                                                                                                      *
* SUMP MONITOR v0.1                                                                                                    *
*                                                                                                                      *
* Copyright (c) 2020 Andrew D. Zonenberg                                                                               *
* All rights reserved.                                                                                                 *
*                                                                                                                      *
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the     *
* following conditions are met:                                                                                        *
*                                                                                                                      *
*    * Redistributions of source code must retain the above copyright notice, this list of conditions, and the         *
*      following disclaimer.                                                                                           *
*                                                                                                                      *
*    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the       *
*      following disclaimer in the documentation and/or other materials provided with the distribution.                *
*                                                                                                                      *
*    * Neither the name of the author nor the names of any contributors may be used to endorse or promote products     *
*      derived from this software without specific prior written permission.                                           *
*                                                                                                                      *
* THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED   *
* TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL *
* THE AUTHORS BE HELD LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES        *
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR       *
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
* POSSIBILITY OF SUCH DAMAGE.                                                                                          *
*                                                                                                                      *
***********************************************************************************************************************/

/**
	@file
	@author Andrew D. Zonenberg
	@brief Top-level window for the application
 */

#ifndef MainWindow_h
#define MainWindow_h

#include "graphwidget/Graph.h"

/**
	@brief Main application window class for a sump pump
 */
class MainWindow	: public Gtk::Window
{
public:
	MainWindow();
	~MainWindow();

protected:

	//Initialization
	void CreateWidgets();

	//Widgets
	Gtk::Notebook m_tabs;
		Gtk::VBox m_summaryTab;
			Gtk::HBox m_depthBox;
				Gtk::Label m_depthCaptionLabel;
				Gtk::Label m_depthLabel;
			Gtk::HBox m_volumeBox;
				Gtk::Label m_volumeCaptionLabel;
				Gtk::Label m_volumeLabel;
			Gtk::HBox m_flowBox;
				Gtk::Label m_flowCaptionLabel;
				Gtk::Label m_flowLabel;
			Gtk::Button m_silenceAlarmButton;
		Gtk::VBox m_depthTab;
			Graph m_depthGraph;
				Graphable m_depthData;
		Gtk::VBox m_volumeTab;
			Graph m_volumeGraph;
				Graphable m_volumeData;
		Gtk::VBox m_inflowTab;
			Graph m_flowGraph;
				Graphable m_flowData;
		Gtk::VBox m_dutyTab;

	bool OnTimer(int timer);

	bool m_alarming;
	void AlarmOn();
	void AlarmOff();
	void SilenceAlarm();
};

#endif
