#include <gtk/gtk.h>
#include <vte/vte.h>
#include <glib.h>
#include <stdlib.h>

static void spawn_callback(VteTerminal* terminal, GPid pid, GError* error, gpointer user_data) {
    if (error) {
        g_printerr("Failed to spawn shell: %s\n", error->message);
        g_error_free(error);
    }
}

int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);

    // Create main window
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Cuddly Terminal");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create terminal widget
    GtkWidget* terminal = vte_terminal_new();
    
    // Get shell from environment or use fallback
    const char* shell_env = g_getenv("SHELL");
    const char* shell_path = shell_env ? shell_env : "/bin/bash";
    char* shell[] = {(char*)shell_path, NULL};
    
    // Configure and spawn terminal
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
        spawn_callback,             // callback
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
