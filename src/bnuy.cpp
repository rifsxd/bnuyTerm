#include <gtk/gtk.h>
#include <vte/vte.h>
#include <stdlib.h>

static GtkWidget *create_terminal() {
    GtkWidget *terminal;
    terminal = vte_terminal_new();
    return terminal;
}

static void update_terminal_colors(GtkWidget *terminal) {
    GdkRGBA bg_color, fg_color;

    // Get the background and foreground colors from the current GTK theme
    gtk_style_context_get_background_color(gtk_widget_get_style_context(terminal), GTK_STATE_FLAG_NORMAL, &bg_color);
    gtk_style_context_get_color(gtk_widget_get_style_context(terminal), GTK_STATE_FLAG_NORMAL, &fg_color);

    // Set the background and foreground colors for the terminal
    vte_terminal_set_color_background(VTE_TERMINAL(terminal), &bg_color);
    vte_terminal_set_color_foreground(VTE_TERMINAL(terminal), &fg_color);
}

static void style_set_callback(GtkWidget *widget, GtkStyleProvider *previous, gpointer user_data) {
    // Callback function to handle style changes
    update_terminal_colors(widget);
}

static void spawn_shell(GtkWidget *terminal) {
    GPid child_pid;
    const char *user_shell = getenv("SHELL");

    if (!user_shell) {
        // Default to /bin/bash if SHELL environment variable is not set
        user_shell = "/bin/bash";
    }

    char *argv[] = {(char *)user_shell, NULL};
    char *envp[] = {NULL};

    vte_terminal_spawn_async(VTE_TERMINAL(terminal),
                             VTE_PTY_DEFAULT,
                             NULL,   // working directory
                             argv,   // command
                             envp,   // environment
                             G_SPAWN_DEFAULT, // spawn flags
                             NULL,   // child setup function
                             NULL,   // child setup data
                             NULL,   // child pid
                             -1,     // timeout (use default)
                             NULL,   // cancellable
                             NULL,   // callback
                             NULL);  // user data
}

static gboolean check_theme_change(gpointer user_data) {
    GtkWidget *terminal = GTK_WIDGET(user_data);
    GdkRGBA bg_color, fg_color;

    gtk_style_context_get_background_color(gtk_widget_get_style_context(terminal), GTK_STATE_FLAG_NORMAL, &bg_color);
    gtk_style_context_get_color(gtk_widget_get_style_context(terminal), GTK_STATE_FLAG_NORMAL, &fg_color);

    vte_terminal_set_color_background(VTE_TERMINAL(terminal), &bg_color);
    vte_terminal_set_color_foreground(VTE_TERMINAL(terminal), &fg_color);

    return G_SOURCE_CONTINUE;
}


static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *terminal;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "bnuyTerm");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    terminal = create_terminal();
    gtk_container_add(GTK_CONTAINER(window), terminal);

    // Connect the style-set signal to the callback function
    g_timeout_add_seconds(1, (GSourceFunc)check_theme_change, terminal);


    // Set initial colors
    update_terminal_colors(terminal);

    spawn_shell(terminal);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.bnuy.Term", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
