#include "config.h"
#include <stdio.h>

static ProfileConfig* parse_profile(JsonObject* profile_obj) {
    ProfileConfig* profile = g_new0(ProfileConfig, 1);
    
    profile->name = g_strdup(json_object_get_string_member(profile_obj, "name"));
    profile->commandline = g_strdup(json_object_get_string_member(profile_obj, "commandline"));
    profile->colorScheme = g_strdup(json_object_get_string_member(profile_obj, "colorScheme"));
    profile->fontSize = json_object_get_int_member(profile_obj, "fontSize");
    profile->fontFamily = g_strdup(json_object_get_string_member(profile_obj, "fontFamily"));
    profile->cursorShape = g_strdup(json_object_get_string_member(profile_obj, "cursorShape"));
    profile->cursorColor = g_strdup(json_object_get_string_member(profile_obj, "cursorColor"));
    profile->scrollbackLines = json_object_get_int_member(profile_obj, "scrollbackLines");
    
    return profile;
}

static ColorScheme* parse_color_scheme(JsonObject* scheme_obj) {
    ColorScheme* scheme = g_new0(ColorScheme, 1);
    
    scheme->name = g_strdup(json_object_get_string_member(scheme_obj, "name"));
    scheme->foreground = g_strdup(json_object_get_string_member(scheme_obj, "foreground"));
    scheme->background = g_strdup(json_object_get_string_member(scheme_obj, "background"));
    scheme->black = g_strdup(json_object_get_string_member(scheme_obj, "black"));
    scheme->red = g_strdup(json_object_get_string_member(scheme_obj, "red"));
    scheme->green = g_strdup(json_object_get_string_member(scheme_obj, "green"));
    scheme->yellow = g_strdup(json_object_get_string_member(scheme_obj, "yellow"));
    scheme->blue = g_strdup(json_object_get_string_member(scheme_obj, "blue"));
    scheme->purple = g_strdup(json_object_get_string_member(scheme_obj, "purple"));
    scheme->cyan = g_strdup(json_object_get_string_member(scheme_obj, "cyan"));
    scheme->white = g_strdup(json_object_get_string_member(scheme_obj, "white"));
    scheme->brightBlack = g_strdup(json_object_get_string_member(scheme_obj, "brightBlack"));
    scheme->brightRed = g_strdup(json_object_get_string_member(scheme_obj, "brightRed"));
    scheme->brightGreen = g_strdup(json_object_get_string_member(scheme_obj, "brightGreen"));
    scheme->brightYellow = g_strdup(json_object_get_string_member(scheme_obj, "brightYellow"));
    scheme->brightBlue = g_strdup(json_object_get_string_member(scheme_obj, "brightBlue"));
    scheme->brightPurple = g_strdup(json_object_get_string_member(scheme_obj, "brightPurple"));
    scheme->brightCyan = g_strdup(json_object_get_string_member(scheme_obj, "brightCyan"));
    scheme->brightWhite = g_strdup(json_object_get_string_member(scheme_obj, "brightWhite"));
    
    return scheme;
}

Config* config_load(const gchar* filename) {
    GError* error = NULL;
    JsonParser* parser = json_parser_new();
    
    if (!json_parser_load_from_file(parser, filename, &error)) {
        g_printerr("Failed to load config: %s\n", error->message);
        g_error_free(error);
        g_object_unref(parser);
        return NULL;
    }
    
    JsonNode* root = json_parser_get_root(parser);
    JsonObject* root_obj = json_node_get_object(root);
    
    Config* config = g_new0(Config, 1);
    config->color_schemes = NULL;
    
    // Parse profiles
    if (json_object_has_member(root_obj, "profiles")) {
        JsonObject* profiles_obj = json_object_get_object_member(root_obj, "profiles");
        if (json_object_has_member(profiles_obj, "defaults")) {
            JsonObject* default_obj = json_object_get_object_member(profiles_obj, "defaults");
            config->default_profile = parse_profile(default_obj);
        }
    }
    
    // Parse color schemes
    if (json_object_has_member(root_obj, "schemes")) {
        JsonArray* schemes_array = json_object_get_array_member(root_obj, "schemes");
        guint len = json_array_get_length(schemes_array);
        for (guint i = 0; i < len; i++) {
            JsonObject* scheme_obj = json_array_get_object_element(schemes_array, i);
            ColorScheme* scheme = parse_color_scheme(scheme_obj);
            config->color_schemes = g_list_append(config->color_schemes, scheme);
        }
    }
    
    // Parse appearance
    if (json_object_has_member(root_obj, "appearance")) {
        JsonObject* appearance_obj = json_object_get_object_member(root_obj, "appearance");
        config->theme = g_strdup(json_object_get_string_member(appearance_obj, "theme"));
        config->opacity = json_object_get_double_member(appearance_obj, "opacity");
        config->showTabsInTitlebar = json_object_get_boolean_member(appearance_obj, "showTabsInTitlebar");
        config->alwaysShowTabs = json_object_get_boolean_member(appearance_obj, "alwaysShowTabs");
    }
    
    g_object_unref(parser);
    return config;
}

ColorScheme* config_find_scheme(Config* config, const gchar* name) {
    for (GList* l = config->color_schemes; l != NULL; l = l->next) {
        ColorScheme* scheme = (ColorScheme*)l->data;
        if (g_strcmp0(scheme->name, name) == 0) {
            return scheme;
        }
    }
    return NULL;
}

void color_scheme_free(ColorScheme* scheme) {
    if (!scheme) return;
    g_free(scheme->name);
    g_free(scheme->foreground);
    g_free(scheme->background);
    g_free(scheme->black);
    g_free(scheme->red);
    g_free(scheme->green);
    g_free(scheme->yellow);
    g_free(scheme->blue);
    g_free(scheme->purple);
    g_free(scheme->cyan);
    g_free(scheme->white);
    g_free(scheme->brightBlack);
    g_free(scheme->brightRed);
    g_free(scheme->brightGreen);
    g_free(scheme->brightYellow);
    g_free(scheme->brightBlue);
    g_free(scheme->brightPurple);
    g_free(scheme->brightCyan);
    g_free(scheme->brightWhite);
    g_free(scheme);
}

void config_free(Config* config) {
    if (!config) return;
    
    if (config->default_profile) {
        g_free(config->default_profile->name);
        g_free(config->default_profile->commandline);
        g_free(config->default_profile->colorScheme);
        g_free(config->default_profile->fontFamily);
        g_free(config->default_profile->cursorShape);
        g_free(config->default_profile->cursorColor);
        g_free(config->default_profile);
    }
    
    g_list_free_full(config->color_schemes, (GDestroyNotify)color_scheme_free);
    g_free(config->theme);
    g_free(config);
}
