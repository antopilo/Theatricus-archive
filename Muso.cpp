#include <iostream>
#include "Window.h"
#include "Core/Timestep.h"

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#include "aquila/aquila.h"
#include "aquila/global.h"
#include "aquila/source/generator/SineGenerator.h"
#include "aquila/transform/FftFactory.h"
#include "aquila/tools/TextPlot.h"

float m_LastFrameTime = 0.0f;
int main()
{
	Window window(800, 800, "Hello world");

	const std::size_t SIZE = 64;
	const Aquila::FrequencyType sampleFreq = 2000;
	const Aquila::FrequencyType f1 = 125, f2 = 700;

	Aquila::SineGenerator sineGenerator1 = Aquila::SineGenerator(sampleFreq);
	sineGenerator1.setAmplitude(32).setFrequency(f1).generate(SIZE);
	Aquila::SineGenerator sineGenerator2 = Aquila::SineGenerator(sampleFreq);
	sineGenerator2.setAmplitude(8).setFrequency(f2).setPhase(0.75).generate(SIZE);
	auto sum = sineGenerator1 + sineGenerator2;

	Aquila::TextPlot plt("Input signal");
	plt.plot(sum);

	// calculate the FFT
	auto fft = Aquila::FftFactory::getFft(SIZE);
	Aquila::SpectrumType spectrum = fft->fft(sum.toArray());

	plt.setTitle("Spectrum");
	plt.plotSpectrum(spectrum);

	

	Aquila::SampleType maxValue = 0, minValue = 0, average = 0;

    while (!glfwWindowShouldClose(window.Get()))
    {
        float time = (float)glfwGetTime();
        Timestep timestep = time - m_LastFrameTime;
        m_LastFrameTime = time;
        window.Draw(timestep);
    }

    glfwTerminate();
}