#include <iostream>
#include <gtk/gtk.h>
#include <vte/vte.h>

int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);

    // Create main window
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Cuddly Terminal");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create terminal widget
    GtkWidget* terminal = vte_terminal_new();
    
    // Configure terminal
    char* shell[] = {"/bin/bash", NULL};
    vte_terminal_spawn_async(
        VTE_TERMINAL(terminal),
        VTE_PTY_DEFAULT,
        NULL,                       // working directory
        shell,                      // argv
        NULL,                       // envv
        G_SPAWN_DEFAULT,            // spawn flags
        NULL, NULL,                 // child setup
        NULL,                       // child pid
        -1,                         // timeout
        NULL,                       // cancellable
        NULL,                       // callback
        NULL                        // user data
    );

    // Add terminal to window
    gtk_container_add(GTK_CONTAINER(window), terminal);
    
    // Show all widgets
    gtk_widget_show_all(window);
    
    // Start main loop
    gtk_main();

    return 0;
}
