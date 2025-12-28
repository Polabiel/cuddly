#ifndef CONFIG_H
#define CONFIG_H

#include <glib.h>
#include <json-glib/json-glib.h>

typedef struct {
    gchar* name;
    gchar* commandline;
    gchar* colorScheme;
    gint fontSize;
    gchar* fontFamily;
    gchar* cursorShape;
    gchar* cursorColor;
    gint scrollbackLines;
} ProfileConfig;

typedef struct {
    gchar* name;
    gchar* foreground;
    gchar* background;
    gchar* black;
    gchar* red;
    gchar* green;
    gchar* yellow;
    gchar* blue;
    gchar* purple;
    gchar* cyan;
    gchar* white;
    gchar* brightBlack;
    gchar* brightRed;
    gchar* brightGreen;
    gchar* brightYellow;
    gchar* brightBlue;
    gchar* brightPurple;
    gchar* brightCyan;
    gchar* brightWhite;
} ColorScheme;

typedef struct {
    ProfileConfig* default_profile;
    GList* color_schemes;
    gchar* theme;
    gdouble opacity;
    gboolean showTabsInTitlebar;
    gboolean alwaysShowTabs;
} Config;

Config* config_load(const gchar* filename);
void config_free(Config* config);
ColorScheme* config_find_scheme(Config* config, const gchar* name);
void color_scheme_free(ColorScheme* scheme);

#endif // CONFIG_H
