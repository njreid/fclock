#include "clock_config.h"
#include <confuse.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>

static TextTransform parse_transform(const char *transform)
{
	if (!transform || strcmp(transform, "none") == 0)
		return TextTransform::None;
	if (strcmp(transform, "uppercase") == 0)
		return TextTransform::Uppercase;
	if (strcmp(transform, "lowercase") == 0)
		return TextTransform::Lowercase;
	return TextTransform::None;
}

static Alignment parse_alignment(const char *align)
{
	if (!align || strcmp(align, "left") == 0)
		return Alignment::Left;
	if (strcmp(align, "center") == 0)
		return Alignment::Center;
	if (strcmp(align, "right") == 0)
		return Alignment::Right;
	return Alignment::Left; // Default to left alignment
}

ClockConfig load_config(const char *config_path)
{
	cfg_opt_t color_opts[] = {
			CFG_INT("red", 0, CFGF_NONE),
			CFG_INT("green", 0, CFGF_NONE),
			CFG_INT("blue", 0, CFGF_NONE),
			CFG_END()};

	cfg_opt_t line_opts[] = {
			CFG_STR("format", "", CFGF_NONE),
			CFG_STR("font", NULL, CFGF_NONE),
			CFG_SEC("color", color_opts, CFGF_NONE),
			CFG_STR("transform", "none", CFGF_NONE),
			CFG_STR("align", "left", CFGF_NONE),
			CFG_BOOL("outline", cfg_false, CFGF_NONE),
			CFG_INT("padding", 0, CFGF_NONE),
			CFG_END()};

	cfg_opt_t matrix_opts[] = {
			CFG_STR("hardware-mapping", "regular", CFGF_NONE),
			CFG_INT("rows", 32, CFGF_NONE),
			CFG_INT("cols", 32, CFGF_NONE),
			CFG_INT("chain-length", 1, CFGF_NONE),
			CFG_INT("parallel", 1, CFGF_NONE),
			CFG_INT("pwm-bits", 11, CFGF_NONE),
			CFG_INT("pwm-lsb-nanoseconds", 130, CFGF_NONE),
			CFG_INT("pwm-dither-bits", 0, CFGF_NONE),
			CFG_INT("brightness", 100, CFGF_NONE),
			CFG_INT("scan-mode", 0, CFGF_NONE),
			CFG_INT("row-address-type", 0, CFGF_NONE),
			CFG_INT("multiplexing", 0, CFGF_NONE),
			CFG_BOOL("disable-hardware-pulsing", cfg_false, CFGF_NONE),
			CFG_BOOL("show-refresh-rate", cfg_false, CFGF_NONE),
			CFG_BOOL("inverse-colors", cfg_false, CFGF_NONE),
			CFG_STR("led-rgb-sequence", "RGB", CFGF_NONE),
			CFG_STR("panel-type", "", CFGF_NONE),
			CFG_INT("limit-refresh-rate-hz", 0, CFGF_NONE),
			CFG_BOOL("disable-busy-waiting", cfg_false, CFGF_NONE),
			CFG_END()};

	cfg_opt_t runtime_opts[] = {
			CFG_STR("pixel-mapper", "", CFGF_NONE),
			CFG_END()};

	cfg_opt_t opts[] = {
			CFG_STR("font-path", "fonts", CFGF_NONE),
			CFG_STR("default-font", "fonts/7x13.bdf", CFGF_NONE),
			CFG_SEC("default-color", color_opts, CFGF_NONE),
			CFG_INT("x-origin", 0, CFGF_NONE),
			CFG_INT("y-origin", 0, CFGF_NONE),
			CFG_INT("letter-spacing", 0, CFGF_NONE),
			CFG_INT("line-spacing", 0, CFGF_NONE),
			CFG_SEC("background-color", color_opts, CFGF_NONE),
			CFG_SEC("lines", line_opts, CFGF_MULTI),
			CFG_SEC("matrix", matrix_opts, CFGF_NONE),
			CFG_SEC("runtime", runtime_opts, CFGF_NONE),
			CFG_END()};

	cfg_t *cfg = cfg_init(opts, CFGF_NONE);
	if (cfg_parse(cfg, config_path) == CFG_PARSE_ERROR)
	{
		fprintf(stderr, "Failed to load config file: %s\n", config_path);
		exit(1);
	}

	cfg_t *default_color = cfg_getsec(cfg, "default-color");
	cfg_t *bg_color = cfg_getsec(cfg, "background-color");

	ClockConfig config;

	// Load basic config
	config.font_path = cfg_getstr(cfg, "font-path");
	config.default_font = cfg_getstr(cfg, "default-font");
	config.x_origin = static_cast<int>(cfg_getint(cfg, "x-origin"));
	config.y_origin = static_cast<int>(cfg_getint(cfg, "y-origin"));
	config.letter_spacing = static_cast<int>(cfg_getint(cfg, "letter-spacing"));
	config.line_spacing = static_cast<int>(cfg_getint(cfg, "line-spacing"));

	config.default_color = rgb_matrix::Color(
			cfg_getint(default_color, "red"),
			cfg_getint(default_color, "green"),
			cfg_getint(default_color, "blue"));
	config.bg_color = rgb_matrix::Color(
			cfg_getint(bg_color, "red"),
			cfg_getint(bg_color, "green"),
			cfg_getint(bg_color, "blue"));

	// Load lines
	size_t num_lines = cfg_size(cfg, "lines");
	if (num_lines < 1 || num_lines > 6)
	{
		fprintf(stderr, "Number of lines must be between 1 and 6\n");
		exit(1);
	}

	for (size_t i = 0; i < num_lines; i++)
	{
		cfg_t *line = cfg_getnsec(cfg, "lines", i);
		LineConfig line_config;

		line_config.format = cfg_getstr(line, "format");
		const char *font = cfg_getstr(line, "font");
		line_config.font = font ? font : config.default_font;

		cfg_t *color = cfg_getsec(line, "color");
		if (color)
		{
			line_config.color = rgb_matrix::Color(
					cfg_getint(color, "red"),
					cfg_getint(color, "green"),
					cfg_getint(color, "blue"));
		}
		else
		{
			line_config.color = config.default_color;
		}

		line_config.transform = parse_transform(cfg_getstr(line, "transform"));
		line_config.alignment = parse_alignment(cfg_getstr(line, "align"));
		line_config.outline = cfg_getbool(line, "outline");
		line_config.padding = static_cast<int>(cfg_getint(line, "padding"));
		config.lines.push_back(line_config);
	}

	// Initialize matrix options with defaults
	config.matrix_options = rgb_matrix::RGBMatrix::Options();

	// Load matrix options
	cfg_t *matrix = cfg_getsec(cfg, "matrix");
	config.matrix_options.hardware_mapping = strdup(cfg_getstr(matrix, "hardware-mapping"));
	config.matrix_options.rows = static_cast<int>(cfg_getint(matrix, "rows"));
	config.matrix_options.cols = static_cast<int>(cfg_getint(matrix, "cols"));
	config.matrix_options.chain_length = static_cast<int>(cfg_getint(matrix, "chain-length"));
	config.matrix_options.parallel = static_cast<int>(cfg_getint(matrix, "parallel"));
	config.matrix_options.pwm_bits = static_cast<int>(cfg_getint(matrix, "pwm-bits"));
	config.matrix_options.pwm_lsb_nanoseconds = static_cast<int>(cfg_getint(matrix, "pwm-lsb-nanoseconds"));
	config.matrix_options.pwm_dither_bits = static_cast<int>(cfg_getint(matrix, "pwm-dither-bits"));
	config.matrix_options.brightness = static_cast<int>(cfg_getint(matrix, "brightness"));
	config.matrix_options.scan_mode = static_cast<int>(cfg_getint(matrix, "scan-mode"));
	config.matrix_options.row_address_type = static_cast<int>(cfg_getint(matrix, "row-address-type"));
	config.matrix_options.multiplexing = static_cast<int>(cfg_getint(matrix, "multiplexing"));
	config.matrix_options.disable_hardware_pulsing = cfg_getbool(matrix, "disable-hardware-pulsing");
	config.matrix_options.show_refresh_rate = cfg_getbool(matrix, "show-refresh-rate");
	config.matrix_options.inverse_colors = cfg_getbool(matrix, "inverse-colors");
	config.matrix_options.led_rgb_sequence = strdup(cfg_getstr(matrix, "led-rgb-sequence"));
	config.matrix_options.panel_type = strdup(cfg_getstr(matrix, "panel-type"));
	config.matrix_options.limit_refresh_rate_hz = static_cast<int>(cfg_getint(matrix, "limit-refresh-rate-hz"));
	config.matrix_options.disable_busy_waiting = cfg_getbool(matrix, "disable-busy-waiting");

	// Load runtime options
	cfg_t *runtime = cfg_getsec(cfg, "runtime");
	if (runtime)
	{
		const char *pixel_mapper = cfg_getstr(runtime, "pixel-mapper");
		if (pixel_mapper && strlen(pixel_mapper) > 0)
		{
			config.matrix_options.pixel_mapper_config = strdup(pixel_mapper);
		}
	}

	cfg_free(cfg);
	return config;
}
