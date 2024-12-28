#include "text_writer.h"
#include <cstring>

TextWriter::TextWriter(const rgb_matrix::Font &font, const rgb_matrix::Color &color,
											 int letter_spacing, const std::string &format,
											 TextTransform transform, Alignment alignment,
											 bool outline, int x, int y, int display_width)
		: font_(&font), color_(&color), letter_spacing_(letter_spacing),
			format_(format), transform_(transform), alignment_(alignment),
			outline_(outline), x_(x), y_(y), display_width_(display_width),
			outline_font_(nullptr)
{
	if (outline)
	{
		outline_font_ = font_->CreateOutlineFont();
	}
}

TextWriter::~TextWriter()
{
	if (outline_font_)
	{
		delete outline_font_;
		outline_font_ = nullptr;
	}
}

TextWriter::TextWriter(const TextWriter &other)
		: font_(other.font_), color_(other.color_), letter_spacing_(other.letter_spacing_),
			format_(other.format_), transform_(other.transform_), alignment_(other.alignment_),
			outline_(other.outline_), x_(other.x_), y_(other.y_), display_width_(other.display_width_),
			outline_font_(nullptr)
{
	if (outline_)
	{
		outline_font_ = font_->CreateOutlineFont();
	}
}

TextWriter &TextWriter::operator=(const TextWriter &other)
{
	if (this != &other)
	{
		if (outline_font_)
		{
			delete outline_font_;
			outline_font_ = nullptr;
		}

		font_ = other.font_;
		color_ = other.color_;
		letter_spacing_ = other.letter_spacing_;
		format_ = other.format_;
		transform_ = other.transform_;
		alignment_ = other.alignment_;
		outline_ = other.outline_;
		x_ = other.x_;
		y_ = other.y_;
		display_width_ = other.display_width_;

		if (outline_)
		{
			outline_font_ = font_->CreateOutlineFont();
		}
	}
	return *this;
}

std::string TextWriter::transform_text(const char *text) const
{
	std::string result(text);
	if (transform_ == TextTransform::Uppercase)
	{
		for (char &c : result)
			c = toupper(c);
	}
	else if (transform_ == TextTransform::Lowercase)
	{
		for (char &c : result)
			c = tolower(c);
	}
	return result;
}

int TextWriter::calculate_x_position(int text_width) const
{
	if (alignment_ == Alignment::Center)
	{
		return (display_width_ - text_width) / 2;
	}
	else if (alignment_ == Alignment::Right)
	{
		return display_width_ - text_width;
	}
	return x_; // Left alignment
}

int TextWriter::text_width(const char *text) const
{
	const rgb_matrix::Font *draw_font = outline_ ? outline_font_ : font_;
	int width = 0;
	for (const char *p = text; *p; ++p)
	{
		width += draw_font->CharacterWidth(*p);
		if (*(p + 1))
		{ // If not the last character
			width += letter_spacing_;
		}
	}
	return width;
}

void TextWriter::write(rgb_matrix::FrameCanvas *canvas, const struct tm &time)
{
	write(canvas, time, x_, y_);
}

void TextWriter::write(rgb_matrix::FrameCanvas *canvas, const struct tm &time, int x, int y)
{
	char text_buffer[256];
	strftime(text_buffer, sizeof(text_buffer), format_.c_str(), &time);

	std::string transformed_text = transform_text(text_buffer);
	int width = text_width(transformed_text.c_str());
	int draw_x = x;

	if (alignment_ != Alignment::Left)
	{
		if (alignment_ == Alignment::Center)
		{
			draw_x = (display_width_ - width) / 2;
		}
		else if (alignment_ == Alignment::Right)
		{
			draw_x = display_width_ - width;
		}
	}

	const rgb_matrix::Font *draw_font = outline_ ? outline_font_ : font_;
	rgb_matrix::DrawText(canvas, *draw_font, draw_x, y + draw_font->baseline(),
											 *color_, NULL, transformed_text.c_str(), letter_spacing_);
}
