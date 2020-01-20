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
	@brief Program entry point
 */

#include "sumpmon.h"
#include "MainWindow.h"
#include <thread>

using namespace std;

bool g_terminating = false;

float g_depth = -999;
int g_leakReading = 0;
double g_timeOfReading = 0;

void PollThread();

/**
	@brief The main application class
 */
class SumpApp : public Gtk::Application
{
public:
	SumpApp()
	 : Gtk::Application()
	 , m_window(NULL)
	{}

	virtual ~SumpApp();

	static Glib::RefPtr<SumpApp> create()
	{
		return Glib::RefPtr<SumpApp>(new SumpApp);
	}

	virtual void run();

protected:
	MainWindow* m_window;

	virtual void on_activate();
};

SumpApp::~SumpApp()
{

}

void SumpApp::run()
{
	register_application();
	on_activate();

	thread poller(PollThread);

	while(true)
	{
		//Dispatch events if we have any
		while(Gtk::Main::events_pending())
			Gtk::Main::iteration();

		//Stop if the main window got closed
		if(!m_window->is_visible())
			break;
	}

	g_terminating = true;
	poller.join();

	delete m_window;
	m_window = NULL;
}

/**
	@brief Create the main window
 */
void SumpApp::on_activate()
{
	m_window = new MainWindow();
	add_window(*m_window);
	m_window->present();
}

int main(int /*argc*/, char* /*argv*/[])
{
	auto app = SumpApp::create();
	app->run();
	return 0;
}

double GetTime()
{
#ifdef _WIN32
	uint64_t tm;
	static uint64_t freq = 0;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&tm));
	double ret = tm;
	if(freq == 0)
		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&freq));
	return ret / freq;
#else
	timespec t;
	clock_gettime(CLOCK_REALTIME,&t);
	double d = static_cast<double>(t.tv_nsec) / 1E9f;
	d += t.tv_sec;
	return d;
#endif
}

void PollThread()
{
	while(!g_terminating)
	{
		g_depth = GetWaterDepth();
		g_timeOfReading = GetTime();
		g_leakReading = ReadLeakSensor();

		usleep(250 * 1000);
	}
}

int ReadLeakSensor()
{
	FILE* fp = popen("python3 /home/azonenberg/read-leak1.py", "r");
	int adc_code;
	fscanf(fp, "%d", &adc_code);
	fclose(fp);
	return adc_code;
}

/**
	@brief Returns the water depth, in mm
 */
float GetWaterDepth()
{
	//calibration constants hard coded for now
	const int cal_offset = 741;
	const float cal_mm_per_lsb = 1.525;

	float adc_code_avg = 0;
	int num_avg = 10;
	for(int avg=0; avg<num_avg; avg ++)
	{
		FILE* fp = popen("python3 /home/azonenberg/read-depth.py", "r");
		int adc_code;
		fscanf(fp, "%d", &adc_code);
		fclose(fp);
		adc_code_avg += (adc_code - cal_offset);
	}
	adc_code_avg /= num_avg;

	if(adc_code_avg < 0)
		return 0;

	return adc_code_avg * cal_mm_per_lsb;
}

/**
	@brief Returns the volume of water in the sump, given the depth
 */
float DepthToVolume(float depth)
{
	//calibration constants hard coded for now (7.4 mm per liter)
	const float cal_ml_per_mm = 0.135;

	return depth * cal_ml_per_mm;
}
