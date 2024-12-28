// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-

#include "led-matrix.h"
#include "graphics.h"
#include "clock_config.h"
#include "text_writer.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

using namespace rgb_matrix;

volatile bool interrupt_received = false;
static void InterruptHandler(int signo)
{
	interrupt_received = true;
}

static std::string get_default_config_path()
{
	const char *home = getenv("HOME");
	if (!home)
	{
		fprintf(stderr, "HOME environment variable not set\n");
		exit(1);
	}
	return std::string(home) + "/.fclock.conf";
}

int main(int argc, char *argv[])
{
	// Load configuration
	const char *config_path = argc > 1 ? argv[1] : get_default_config_path().c_str();
	ClockConfig config = load_config(config_path);

	/*
	 * Load fonts
	 */
	// Load all unique fonts
	std::map<std::string, rgb_matrix::Font *> fonts;
	for (const auto &line : config.lines)
	{
		if (fonts.find(line.font) == fonts.end())
		{
			auto font = new rgb_matrix::Font();
			std::string font_path = line.font;
			if (line.font[0] != '/')
			{ // If not absolute path
				font_path = config.font_path + "/" + line.font;
			}
			if (!font->LoadFont(font_path.c_str()))
			{
				fprintf(stderr, "Couldn't load font '%s'\n", line.font.c_str());
				return 1;
			}
			fonts[line.font] = font;
		}
	}

	RGBMatrix *matrix = RGBMatrix::CreateFromOptions(config.matrix_options, config.runtime_options);
	if (matrix == NULL)
		return 1;

	const int x = config.x_origin;
	int y = config.y_origin;

	FrameCanvas *offscreen = matrix->CreateFrameCanvas();

	struct timespec next_time;
	next_time.tv_sec = time(NULL);
	next_time.tv_nsec = 0;
	struct tm tm;

	signal(SIGTERM, InterruptHandler);
	signal(SIGINT, InterruptHandler);

	// Check if display is rotated 90 or 270 degrees
	bool is_rotated = false;
	if (config.matrix_options.pixel_mapper_config)
	{
		const char *mapper = config.matrix_options.pixel_mapper_config;
		is_rotated = (strstr(mapper, "Rotate:90") || strstr(mapper, "Rotate:270"));
	}
	int display_width = is_rotated ? config.matrix_options.rows : config.matrix_options.cols;

	// Pre-create TextWriter instances for each line
	std::vector<TextWriter> writers;
	int y_pos = y;
	for (const auto &line : config.lines)
	{
		writers.emplace_back(*fonts[line.font], line.color, config.letter_spacing,
												 line.format, line.transform, line.alignment,
												 line.outline, x, y_pos, display_width);
		y_pos += fonts[line.font]->height() + config.line_spacing + line.padding;
	}

	while (!interrupt_received)
	{
		offscreen->Fill(config.bg_color.r, config.bg_color.g, config.bg_color.b);
		localtime_r(&next_time.tv_sec, &tm);

		// Draw each line using pre-created writers
		for (auto &writer : writers)
		{
			writer.write(offscreen, tm);
		}

		// Wait until we're ready to show it.
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &next_time, NULL);

		// Atomic swap with double buffer
		offscreen = matrix->SwapOnVSync(offscreen);

		next_time.tv_sec += 1;
	}

	// Finished. Shut down the RGB matrix.
	// Clean up fonts
	for (auto &pair : fonts)
	{
		delete pair.second;
	}
	delete matrix;

	if (write(STDOUT_FILENO, "\n", 1) < 0)
	{ // Create a fresh new line after ^C on screen
		perror("write failed");
	}
	return 0;
}
