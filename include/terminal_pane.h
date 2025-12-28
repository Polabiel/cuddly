#ifndef TERMINAL_PANE_H
#define TERMINAL_PANE_H

#include <gtk/gtk.h>
#include <vte/vte.h>
#include "config.h"

typedef struct AppState AppState;

typedef struct {
    GtkWidget* container;  // GtkPaned or scrolled window
    GtkWidget* terminal;   // VteTerminal (NULL if split)
    gboolean is_split;
    GtkOrientation split_orientation;
    struct TerminalPane* child1;
    struct TerminalPane* child2;
    AppState* app;
} TerminalPane;

TerminalPane* terminal_pane_new(AppState* app);
void terminal_pane_free(TerminalPane* pane);
void terminal_pane_split(TerminalPane* pane, GtkOrientation orientation);
GtkWidget* terminal_pane_get_widget(TerminalPane* pane);
void terminal_pane_focus_terminal(TerminalPane* pane);

#endif // TERMINAL_PANE_H
