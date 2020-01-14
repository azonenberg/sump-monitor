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
	@brief Implementation of main application window class
 */

#include "sumpmon.h"
#include "MainWindow.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction / destruction

/**
	@brief Initializes the main window
 */
MainWindow::MainWindow()
{
	set_title("Sump Monitor");

	//Initial setup
	set_reallocate_redraws(true);

	//Add widgets
	CreateWidgets();

	//Run the HMI in fullscreen mode
	fullscreen();

	//Set the update timer
	sigc::slot<bool> slot = sigc::bind(sigc::mem_fun(*this, &MainWindow::OnTimer), 1);
	sigc::connection conn = Glib::signal_timeout().connect(slot, 250);
}

/**
	@brief Application cleanup
 */
MainWindow::~MainWindow()
{
}

/**
	@brief Helper function for creating widgets and setting up signal handlers
 */
void MainWindow::CreateWidgets()
{
	//Set up window hierarchy
	add(m_tabs);
		m_tabs.append_page(m_summaryTab, "Summary");
		m_tabs.append_page(m_depthTab, "Water Depth");
			m_depthTab.add(m_depthGraph);
				m_depthGraph.m_units = "mm";
				m_depthGraph.m_minScale = 0;
				m_depthGraph.m_maxScale = 500;
				m_depthGraph.m_scaleBump = 50;
				m_depthGraph.m_maxRedline = 400;
				m_depthGraph.m_series.push_back(&m_depthData);
				m_depthGraph.m_seriesName = "depth";
				m_depthGraph.m_timeScale = 1.5;
				m_depthGraph.m_timeTick = 30;
				m_depthGraph.m_drawLegend = false;
				m_depthData.m_color = Gdk::Color("#0000ff");
		m_tabs.append_page(m_volumeTab, "Tank Volume");
		m_tabs.append_page(m_inflowTab, "Flow Rate");
		m_tabs.append_page(m_dutyTab, "Pump Duty Cycle");

	m_tabs.set_current_page(1);

	//Done adding widgets
	show_all();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Message handlers

bool MainWindow::OnTimer(int /*timer*/)
{
	double t = GetTime();
	double depth = g_depth;

	auto dseries = m_depthData.GetSeries("depth");
	dseries->push_back(GraphPoint(t, depth));

	//Clean out old stuff
	size_t max_depth = 10000;
	while(dseries->size() > max_depth)
		dseries->erase(dseries->begin());

	return true;
}
