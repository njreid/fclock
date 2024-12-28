#ifndef TEXT_WRITER_H
#define TEXT_WRITER_H

#include "led-matrix.h"
#include "graphics.h"
#include "clock_config.h"

class TextWriter
{
public:
	TextWriter(const rgb_matrix::Font &font, const rgb_matrix::Color &color,
						 int letter_spacing, const std::string &format,
						 TextTransform transform, Alignment alignment,
						 bool outline, int x, int y, int display_width);
	~TextWriter();

	// Copy constructor
	TextWriter(const TextWriter &other);
	// Assignment operator
	TextWriter &operator=(const TextWriter &other);
	void write(rgb_matrix::FrameCanvas *canvas, const struct tm &time);
	void write(rgb_matrix::FrameCanvas *canvas, const struct tm &time, int x, int y);

private:
	std::string transform_text(const char *text) const;
	int calculate_x_position(int text_width) const;
	int text_width(const char *text) const;

	const rgb_matrix::Font *font_;
	const rgb_matrix::Color *color_;
	int letter_spacing_;
	std::string format_;
	TextTransform transform_;
	Alignment alignment_;
	bool outline_;
	int x_;
	int y_;
	int display_width_;
	rgb_matrix::Font *outline_font_;
};

#endif // TEXT_WRITER_H
