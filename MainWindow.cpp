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
	: m_depthGraph(500)
	, m_volumeGraph(500)
	, m_flowGraph(500)
	, m_alarming(false)
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
	sigc::connection conn = Glib::signal_timeout().connect(slot, 1000);
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
	m_tabs.override_font(Pango::FontDescription("sans bold 20"));
	string font = "sans bold 14";

	//Set up window hierarchy
	add(m_tabs);
		m_tabs.append_page(m_summaryTab, "Summary");
			m_summaryTab.pack_start(m_depthBox, Gtk::PACK_SHRINK);
				m_depthBox.pack_start(m_depthCaptionLabel, Gtk::PACK_SHRINK);
					m_depthCaptionLabel.override_font(Pango::FontDescription("sans bold 20"));
					m_depthCaptionLabel.set_label("Depth: ");
					m_depthCaptionLabel.set_size_request(125, 1);
				m_depthBox.pack_start(m_depthLabel, Gtk::PACK_SHRINK);
					m_depthLabel.override_font(Pango::FontDescription("sans bold 20"));
			m_summaryTab.pack_start(m_volumeBox, Gtk::PACK_SHRINK);
				m_volumeBox.pack_start(m_volumeCaptionLabel, Gtk::PACK_SHRINK);
					m_volumeCaptionLabel.override_font(Pango::FontDescription("sans bold 20"));
					m_volumeCaptionLabel.set_label("Volume: ");
					m_volumeCaptionLabel.set_size_request(125, 1);
				m_volumeBox.pack_start(m_volumeLabel, Gtk::PACK_SHRINK);
					m_volumeLabel.override_font(Pango::FontDescription("sans bold 20"));
			m_summaryTab.pack_start(m_flowBox, Gtk::PACK_SHRINK);
				m_flowBox.pack_start(m_flowCaptionLabel, Gtk::PACK_SHRINK);
					m_flowCaptionLabel.override_font(Pango::FontDescription("sans bold 20"));
					m_flowCaptionLabel.set_label("Flow: ");
					m_flowCaptionLabel.set_size_request(125, 1);
				m_flowBox.pack_start(m_flowLabel, Gtk::PACK_SHRINK);
					m_flowLabel.override_font(Pango::FontDescription("sans bold 20"));
			m_summaryTab.pack_start(m_silenceAlarmButton, Gtk::PACK_SHRINK);
				m_silenceAlarmButton.set_label("Silence alarm");
				m_silenceAlarmButton.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::SilenceAlarm));

			m_summaryTab.pack_start(m_trendFrame, Gtk::PACK_EXPAND_WIDGET);
				m_trendFrame.set_label("Weekly Flow Trend");
				m_trendFrame.add(m_trendGraph);
					m_trendGraph.m_units = "L/hr";
					m_trendGraph.m_minScale = 0;
					m_trendGraph.m_maxScale = 50;
					m_trendGraph.m_scaleBump = 5;
					m_trendGraph.m_maxRedline = 45;
					m_trendGraph.m_series.push_back(&m_trendData);
					m_trendGraph.m_seriesName = "flow";
					m_trendGraph.m_timeScale = 0.001;
					m_trendGraph.m_timeTick = 86400;
					m_trendGraph.m_lineWidth = 3;
					m_trendGraph.m_drawLegend = false;
					m_trendData.m_color = Gdk::Color("#0000ff");
					m_trendGraph.m_font = Pango::FontDescription(font);

		m_tabs.append_page(m_depthTab, "Depth");
			m_depthTab.add(m_depthGraph);
				m_depthGraph.m_units = "mm";
				m_depthGraph.m_minScale = 100;
				m_depthGraph.m_maxScale = 225;
				m_depthGraph.m_scaleBump = 25;
				m_depthGraph.m_maxRedline = 200;
				m_depthGraph.m_series.push_back(&m_depthData);
				m_depthGraph.m_seriesName = "depth";
				m_depthGraph.m_timeScale = 0.15;
				m_depthGraph.m_timeTick = 600;
				m_depthGraph.m_lineWidth = 3;
				m_depthGraph.m_drawLegend = false;
				m_depthData.m_color = Gdk::Color("#0000ff");
				m_depthGraph.m_font = Pango::FontDescription(font);
		m_tabs.append_page(m_volumeTab, "Volume");
			m_volumeTab.add(m_volumeGraph);
				m_volumeGraph.m_units = "L";
				m_volumeGraph.m_minScale = 14;
				m_volumeGraph.m_maxScale = 30;
				m_volumeGraph.m_scaleBump = 2;
				m_volumeGraph.m_maxRedline = 28;
				m_volumeGraph.m_series.push_back(&m_volumeData);
				m_volumeGraph.m_seriesName = "volume";
				m_volumeGraph.m_timeScale = 0.15;
				m_volumeGraph.m_timeTick = 600;
				m_volumeGraph.m_lineWidth = 3;
				m_volumeGraph.m_drawLegend = false;
				m_volumeData.m_color = Gdk::Color("#0000ff");
				m_volumeGraph.m_font = Pango::FontDescription(font);
		m_tabs.append_page(m_inflowTab, "Flow");
			m_inflowTab.add(m_flowGraph);
				m_flowGraph.m_units = "L/hr";
				m_flowGraph.m_minScale = 0;
				m_flowGraph.m_maxScale = 50;
				m_flowGraph.m_scaleBump = 5;
				m_flowGraph.m_maxRedline = 45;
				m_flowGraph.m_series.push_back(&m_flowData);
				m_flowGraph.m_seriesName = "flow";
				m_flowGraph.m_timeScale = 0.075;
				m_flowGraph.m_timeTick = 1200;
				m_flowGraph.m_lineWidth = 3;
				m_flowGraph.m_drawLegend = false;
				m_flowData.m_color = Gdk::Color("#0000ff");
				m_flowGraph.m_font = Pango::FontDescription(font);
		m_tabs.append_page(m_dutyTab, "Duty %");

	//Done adding widgets
	show_all();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Message handlers

bool MainWindow::OnTimer(int /*timer*/)
{
	//Before we do anything else, check if any of the floor sensors are leaking and ring the alarm.
	if(g_leakReading > 10)
	{
		if(!m_alarming)
			AlarmOn();
	}

	//Clear alarms if no trouble conditions
	else if(m_alarming)
		AlarmOff();

	double t = g_timeOfReading;
	double depth = g_depth;
	double volume = DepthToVolume(depth);

	//If we get called before the first measurement shows up, do nothing.
	//(Negative depth is physically impossible)
	if(depth < 0)
		return true;

	auto dseries = m_depthData.GetSeries("depth");
	auto vseries = m_volumeData.GetSeries("volume");
	auto fseries = m_flowData.GetSeries("flow");

	//Flow is calculated in liters per hour.
	//Use a large Gaussian window to get a more accurate estimate.
	const size_t window = 127;
	const size_t mid = (window-1)/2;
	const size_t delta = 120;
	const size_t dwindow = window + delta;
	double sigma = 30;
	double coeffs[window];
	double frac = 1 / (sqrt(2 * M_PI)*sigma);
	double isq = 1 / (2*sigma*sigma);
	double sum = 0;
	for(size_t i=0; i<window; i++)
	{
		double dx = fabs(i - mid);
		coeffs[i] = frac * exp(-dx*dx*isq);
		sum += coeffs[i];
	}
	for(size_t i=0; i<window; i++)	//normalize kernel
		coeffs[i] /= sum;

	double flow = 0;
	if(vseries->size() > dwindow)
	{
		double samples[dwindow];
		double times[dwindow];
		auto it = vseries->end();
		it --;
		for(size_t i=0; i<dwindow && it != vseries->begin(); i ++)
		{
			samples[i] = it->value;
			times[i] = it->time;
			it --;
		}

		double center1 = times[mid];
		double center2 = times[mid + delta];

		//Smooth the volumetric data with a Gaussian kernel
		double gauss1 = 0;
		double gauss2 = 0;
		for(size_t i=0; i<window; i++)
		{
			gauss1 += samples[i] * coeffs[i];
			gauss2 += samples[i+delta] * coeffs[i];
		}
		double dt = center1 - center2;
		//printf("dt = %.3f\n", dt);
		double dvol = gauss1 - gauss2;		//liters
		flow = (dvol * 3600) / dt;
		//printf("rates: %.3f %.3f / %.3f L, %.3f L/hr\n", gauss1, gauss2, dvol, flow);
	}

	//TODO: determine if the pump is on or not

	dseries->push_back(GraphPoint(t, depth));
	vseries->push_back(GraphPoint(t, volume));
	fseries->push_back(GraphPoint(t, flow));

	//Format text
	char tmp[128];
	snprintf(tmp, sizeof(tmp), "%.1f mm", depth);
	m_depthLabel.set_label(tmp);
	snprintf(tmp, sizeof(tmp), "%.1f L", volume);
	m_volumeLabel.set_label(tmp);
	snprintf(tmp, sizeof(tmp), "%.1f L/hr", flow);
	m_flowLabel.set_label(tmp);

	//If the flow rate is positive (pump not running, water leaking in) add the current flow rate to the history
	if(flow > 0)
	{
		if(m_flowSamples.empty())
			printf("Pump stopped\n");

		m_flowSamples.push_back(flow);
	}

	//Pump is running.
	//Pump must have just started if we have samples in the buffer.
	else if(!m_flowSamples.empty())
	{
		printf("Pump started\n");

		//Figure out total memory depth.
		//Ignore 20 sec at start and end of buffer due to interference from the pump flow
		size_t margin = 20;
		double sum = 0;
		double count = 0;
		for(size_t i=margin; i+margin < m_flowSamples.size(); i++)
		{
			sum += m_flowSamples[i];
			count ++;
		}
		double avg = sum / count;
		m_flowSamples.clear();

		printf("Average flow during this pump cycle: %f\n", avg);

		//Write current flow to a file we can read from munin
		FILE* fp = fopen("avgflow.txt", "w");
		fprintf(fp, "%f", avg);
		fclose(fp);

		auto tseries = m_trendData.GetSeries("flow");
		tseries->push_back(GraphPoint(t, avg));
	}

	//No, pump has been running for a while. No action needed.
	else
	{
	}

	//Clean out old stuff
	size_t max_points = 10000;
	while(dseries->size() > max_points)
		dseries->erase(dseries->begin());
	while(vseries->size() > max_points)
		vseries->erase(vseries->begin());
	while(fseries->size() > max_points)
		fseries->erase(fseries->begin());

	return true;
}

void MainWindow::AlarmOn()
{
	m_alarming = true;
	system("python3 /home/azonenberg/alarm-on.py");
}

void MainWindow::AlarmOff()
{
	m_alarming = false;
	system("python3 /home/azonenberg/alarm-off.py");
}

void MainWindow::SilenceAlarm()
{
	system("python3 /home/azonenberg/alarm-off.py");
}
