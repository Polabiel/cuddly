#include <gtk/gtk.h>
#include <vte/vte.h>
#include <glib.h>
#include <stdlib.h>
#include "config.h"

typedef struct {
    GtkWidget* window;
    GtkWidget* notebook;
    Config* config;
    gint tab_counter;
} AppState;

static void parse_color(const gchar* color_str, GdkRGBA* color) {
    gdk_rgba_parse(color, color_str);
}

static void apply_color_scheme(VteTerminal* terminal, ColorScheme* scheme) {
    if (!scheme) return;
    
    GdkRGBA fg, bg, palette[16];
    
    parse_color(scheme->foreground, &fg);
    parse_color(scheme->background, &bg);
    parse_color(scheme->black, &palette[0]);
    parse_color(scheme->red, &palette[1]);
    parse_color(scheme->green, &palette[2]);
    parse_color(scheme->yellow, &palette[3]);
    parse_color(scheme->blue, &palette[4]);
    parse_color(scheme->purple, &palette[5]);
    parse_color(scheme->cyan, &palette[6]);
    parse_color(scheme->white, &palette[7]);
    parse_color(scheme->brightBlack, &palette[8]);
    parse_color(scheme->brightRed, &palette[9]);
    parse_color(scheme->brightGreen, &palette[10]);
    parse_color(scheme->brightYellow, &palette[11]);
    parse_color(scheme->brightBlue, &palette[12]);
    parse_color(scheme->brightPurple, &palette[13]);
    parse_color(scheme->brightCyan, &palette[14]);
    parse_color(scheme->brightWhite, &palette[15]);
    
    vte_terminal_set_colors(terminal, &fg, &bg, palette, 16);
}

static void apply_profile_settings(VteTerminal* terminal, ProfileConfig* profile, ColorScheme* scheme) {
    // Apply color scheme
    if (scheme) {
        apply_color_scheme(terminal, scheme);
    }
    
    // Apply font
    if (profile->fontFamily && profile->fontSize > 0) {
        gchar* font_desc = g_strdup_printf("%s %d", profile->fontFamily, profile->fontSize);
        PangoFontDescription* font = pango_font_description_from_string(font_desc);
        vte_terminal_set_font(terminal, font);
        pango_font_description_free(font);
        g_free(font_desc);
    }
    
    // Apply cursor
    if (g_strcmp0(profile->cursorShape, "bar") == 0) {
        vte_terminal_set_cursor_shape(terminal, VTE_CURSOR_SHAPE_IBEAM);
    } else if (g_strcmp0(profile->cursorShape, "underline") == 0) {
        vte_terminal_set_cursor_shape(terminal, VTE_CURSOR_SHAPE_UNDERLINE);
    } else {
        vte_terminal_set_cursor_shape(terminal, VTE_CURSOR_SHAPE_BLOCK);
    }
    
    // Apply scrollback
    vte_terminal_set_scrollback_lines(terminal, profile->scrollbackLines);
    
    // Enable cursor blinking for smooth animations
    vte_terminal_set_cursor_blink_mode(terminal, VTE_CURSOR_BLINK_ON);
}

static void spawn_callback(VteTerminal* terminal, GPid pid, GError* error, gpointer user_data) {
    if (error) {
        g_printerr("Failed to spawn shell: %s\n", error->message);
        g_error_free(error);
    }
}

static void on_terminal_child_exited(VteTerminal* terminal, gint status, gpointer user_data) {
    AppState* app = (AppState*)user_data;
    GtkWidget* scrolled_window = gtk_widget_get_parent(GTK_WIDGET(terminal));
    gint page_num = gtk_notebook_page_num(GTK_NOTEBOOK(app->notebook), scrolled_window);
    
    if (page_num >= 0) {
        gtk_notebook_remove_page(GTK_NOTEBOOK(app->notebook), page_num);
    }
    
    // Quit if no more tabs
    if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(app->notebook)) == 0) {
        gtk_main_quit();
    }
}

static void create_new_tab(AppState* app) {
    // Create terminal widget
    GtkWidget* terminal = vte_terminal_new();
    
    // Apply configuration
    if (app->config && app->config->default_profile) {
        ProfileConfig* profile = app->config->default_profile;
        ColorScheme* scheme = config_find_scheme(app->config, profile->colorScheme);
        apply_profile_settings(VTE_TERMINAL(terminal), profile, scheme);
        
        // Get shell from profile or environment
        const gchar* shell_path = profile->commandline;
        if (!shell_path || strlen(shell_path) == 0) {
            const char* shell_env = g_getenv("SHELL");
            shell_path = shell_env ? shell_env : "/bin/bash";
        }
        
        char* argv[] = {g_strdup(shell_path), NULL};
        
        // Spawn terminal
        vte_terminal_spawn_async(
            VTE_TERMINAL(terminal),
            VTE_PTY_DEFAULT,
            NULL,                       // working directory
            argv,                       // argv
            NULL,                       // envv
            G_SPAWN_DEFAULT,            // spawn flags
            NULL, NULL,                 // child setup
            NULL,                       // child pid
            -1,                         // timeout
            NULL,                       // cancellable
            spawn_callback,             // callback
            NULL                        // user data
        );
        
        g_free(argv[0]);
    } else {
        // Fallback if no config
        const char* shell_env = g_getenv("SHELL");
        const char* shell_path = shell_env ? shell_env : "/bin/bash";
        char* argv[] = {g_strdup(shell_path), NULL};
        
        vte_terminal_spawn_async(
            VTE_TERMINAL(terminal),
            VTE_PTY_DEFAULT,
            NULL, argv, NULL,
            G_SPAWN_DEFAULT,
            NULL, NULL, NULL, -1, NULL,
            spawn_callback, NULL
        );
        
        g_free(argv[0]);
    }
    
    // Connect child exited signal
    g_signal_connect(terminal, "child-exited", G_CALLBACK(on_terminal_child_exited), app);
    
    // Create scrolled window
    GtkWidget* scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_ALWAYS);
    gtk_container_add(GTK_CONTAINER(scrolled_window), terminal);
    
    // Create tab label
    gchar* label_text = g_strdup_printf("Terminal %d", ++app->tab_counter);
    GtkWidget* label = gtk_label_new(label_text);
    g_free(label_text);
    
    // Add tab to notebook
    gint page_num = gtk_notebook_append_page(GTK_NOTEBOOK(app->notebook), scrolled_window, label);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(app->notebook), page_num);
    gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(app->notebook), scrolled_window, TRUE);
    
    gtk_widget_show_all(scrolled_window);
    gtk_widget_grab_focus(terminal);
}

static void close_current_tab(AppState* app) {
    gint current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(app->notebook));
    if (current_page >= 0) {
        gtk_notebook_remove_page(GTK_NOTEBOOK(app->notebook), current_page);
        
        // Quit if no more tabs
        if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(app->notebook)) == 0) {
            gtk_main_quit();
        }
    }
}

static void next_tab(AppState* app) {
    gint current = gtk_notebook_get_current_page(GTK_NOTEBOOK(app->notebook));
    gint n_pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(app->notebook));
    gtk_notebook_set_current_page(GTK_NOTEBOOK(app->notebook), (current + 1) % n_pages);
}

static void prev_tab(AppState* app) {
    gint current = gtk_notebook_get_current_page(GTK_NOTEBOOK(app->notebook));
    gint n_pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(app->notebook));
    gtk_notebook_set_current_page(GTK_NOTEBOOK(app->notebook), (current - 1 + n_pages) % n_pages);
}

static gboolean on_key_press(GtkWidget* widget, GdkEventKey* event, gpointer user_data) {
    AppState* app = (AppState*)user_data;
    guint modifiers = event->state & gtk_accelerator_get_default_mod_mask();
    
    // Ctrl+Shift+T - New Tab
    if (modifiers == (GDK_CONTROL_MASK | GDK_SHIFT_MASK) && event->keyval == GDK_KEY_T) {
        create_new_tab(app);
        return TRUE;
    }
    
    // Ctrl+Shift+W - Close Tab
    if (modifiers == (GDK_CONTROL_MASK | GDK_SHIFT_MASK) && event->keyval == GDK_KEY_W) {
        close_current_tab(app);
        return TRUE;
    }
    
    // Ctrl+Tab - Next Tab
    if (modifiers == GDK_CONTROL_MASK && event->keyval == GDK_KEY_Tab) {
        next_tab(app);
        return TRUE;
    }
    
    // Ctrl+Shift+Tab - Previous Tab
    if (modifiers == (GDK_CONTROL_MASK | GDK_SHIFT_MASK) && event->keyval == GDK_KEY_ISO_Left_Tab) {
        prev_tab(app);
        return TRUE;
    }
    
    // Ctrl+Shift+C - Copy
    if (modifiers == (GDK_CONTROL_MASK | GDK_SHIFT_MASK) && event->keyval == GDK_KEY_C) {
        gint current = gtk_notebook_get_current_page(GTK_NOTEBOOK(app->notebook));
        if (current >= 0) {
            GtkWidget* scrolled = gtk_notebook_get_nth_page(GTK_NOTEBOOK(app->notebook), current);
            GtkWidget* terminal = gtk_bin_get_child(GTK_BIN(scrolled));
            if (VTE_IS_TERMINAL(terminal)) {
                vte_terminal_copy_clipboard_format(VTE_TERMINAL(terminal), VTE_FORMAT_TEXT);
            }
        }
        return TRUE;
    }
    
    // Ctrl+Shift+V - Paste
    if (modifiers == (GDK_CONTROL_MASK | GDK_SHIFT_MASK) && event->keyval == GDK_KEY_V) {
        gint current = gtk_notebook_get_current_page(GTK_NOTEBOOK(app->notebook));
        if (current >= 0) {
            GtkWidget* scrolled = gtk_notebook_get_nth_page(GTK_NOTEBOOK(app->notebook), current);
            GtkWidget* terminal = gtk_bin_get_child(GTK_BIN(scrolled));
            if (VTE_IS_TERMINAL(terminal)) {
                vte_terminal_paste_clipboard(VTE_TERMINAL(terminal));
            }
        }
        return TRUE;
    }
    
    return FALSE;
}

static void apply_css_styling(AppState* app) {
    GtkCssProvider* provider = gtk_css_provider_new();
    
    // Enhanced CSS with transparency and gradient effects
    const gchar* css_data = 
        "window {"
        "    background: linear-gradient(135deg, rgba(30, 30, 30, 0.95) 0%, rgba(45, 45, 45, 0.95) 100%);"
        "}"
        "notebook {"
        "    background: linear-gradient(180deg, rgba(35, 35, 38, 0.98) 0%, rgba(45, 45, 48, 0.98) 100%);"
        "    border: none;"
        "    border-radius: 8px;"
        "}"
        "notebook header {"
        "    background: linear-gradient(180deg, rgba(40, 40, 45, 0.95) 0%, rgba(35, 35, 40, 0.95) 100%);"
        "    border: none;"
        "    border-bottom: 1px solid rgba(62, 62, 62, 0.5);"
        "    border-radius: 8px 8px 0 0;"
        "    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.3);"
        "}"
        "notebook header tabs {"
        "    background: transparent;"
        "}"
        "notebook header tabs tab {"
        "    background: linear-gradient(180deg, rgba(45, 45, 48, 0.8) 0%, rgba(40, 40, 43, 0.8) 100%);"
        "    color: rgba(204, 204, 204, 0.9);"
        "    border: none;"
        "    border-radius: 6px 6px 0 0;"
        "    padding: 10px 20px;"
        "    margin: 2px 2px 0 2px;"
        "    box-shadow: inset 0 -2px 4px rgba(0, 0, 0, 0.2);"
        "    transition: all 200ms ease-in-out;"
        "}"
        "notebook header tabs tab:hover {"
        "    background: linear-gradient(180deg, rgba(55, 55, 60, 0.9) 0%, rgba(50, 50, 55, 0.9) 100%);"
        "    color: rgba(255, 255, 255, 0.95);"
        "    box-shadow: 0 2px 6px rgba(0, 120, 212, 0.3);"
        "}"
        "notebook header tabs tab:checked {"
        "    background: linear-gradient(180deg, rgba(30, 30, 33, 0.98) 0%, rgba(25, 25, 28, 0.98) 100%);"
        "    color: #ffffff;"
        "    border-bottom: 3px solid #0078d4;"
        "    box-shadow: 0 4px 12px rgba(0, 120, 212, 0.4), inset 0 1px 3px rgba(255, 255, 255, 0.1);"
        "}"
        "scrolledwindow {"
        "    background: rgba(12, 12, 12, 0.95);"
        "    border-radius: 0 0 8px 8px;"
        "}";
    
    gtk_css_provider_load_from_data(provider, css_data, -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    
    g_object_unref(provider);
}

int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);
    
    // Initialize app state
    AppState* app = g_new0(AppState, 1);
    app->tab_counter = 0;
    
    // Load configuration
    app->config = config_load("resources/defaults.json");
    if (!app->config) {
        g_printerr("Warning: Could not load configuration, using defaults\n");
    }
    
    // Create main window
    app->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(app->window), "Cuddly Terminal");
    gtk_window_set_default_size(GTK_WINDOW(app->window), 1000, 600);
    
    // Enable transparency and visual effects
    GdkScreen* screen = gtk_widget_get_screen(app->window);
    GdkVisual* visual = gdk_screen_get_rgba_visual(screen);
    if (visual != NULL && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(app->window, visual);
        gtk_widget_set_app_paintable(app->window, TRUE);
    }
    
    // Apply opacity from config
    if (app->config && app->config->opacity > 0.0 && app->config->opacity <= 1.0) {
        gtk_widget_set_opacity(app->window, app->config->opacity);
    } else {
        gtk_widget_set_opacity(app->window, 0.95); // Default 95% opacity for glass effect
    }
    
    g_signal_connect(app->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(app->window, "key-press-event", G_CALLBACK(on_key_press), app);
    
    // Create notebook (tabs)
    app->notebook = gtk_notebook_new();
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(app->notebook), TRUE);
    gtk_notebook_set_show_border(GTK_NOTEBOOK(app->notebook), FALSE);
    
    // Show tabs in titlebar if configured
    if (app->config && app->config->showTabsInTitlebar) {
        gtk_notebook_set_show_tabs(GTK_NOTEBOOK(app->notebook), TRUE);
    }
    
    gtk_container_add(GTK_CONTAINER(app->window), app->notebook);
    
    // Apply CSS styling
    apply_css_styling(app);
    
    // Create first tab
    create_new_tab(app);
    
    // Show all widgets
    gtk_widget_show_all(app->window);
    
    // Start main loop
    gtk_main();
    
    // Cleanup
    config_free(app->config);
    g_free(app);
    
    return 0;
}

