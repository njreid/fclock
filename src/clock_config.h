#ifndef CLOCK_CONFIG_H
#define CLOCK_CONFIG_H

#include "led-matrix.h"
#include "graphics.h"
#include <string>
#include <vector>

enum class TextTransform
{
	None,
	Uppercase,
	Lowercase
};

enum class Alignment
{
	Left,
	Center,
	Right
};

struct LineConfig
{
	std::string format;
	std::string font;
	rgb_matrix::Color color;
	TextTransform transform;
	Alignment alignment;
	bool outline; // Whether to use outline font
	int padding;	// Additional pixels to skip after this line
};

struct ClockConfig
{
	std::string font_path;
	std::string default_font;
	rgb_matrix::Color default_color;
	int x_origin;
	int y_origin;
	int letter_spacing;
	int line_spacing;
	rgb_matrix::Color bg_color;
	std::vector<LineConfig> lines;
	rgb_matrix::RGBMatrix::Options matrix_options;
	rgb_matrix::RuntimeOptions runtime_options;
};

ClockConfig load_config(const char *config_path);

#endif // CLOCK_CONFIG_H
