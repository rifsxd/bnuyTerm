#include <gtk/gtk.h>
#include <vte/vte.h>

static void activate(GtkApplication* app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "BnuyTerm");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    VteTerminal *terminal = VTE_TERMINAL(vte_terminal_new());
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(terminal));

    // Set a retro Fallout color scheme
    GdkRGBA fg_color, bg_color;
    gdk_rgba_parse(&fg_color, "#00FF00"); // Bright green for foreground
    gdk_rgba_parse(&bg_color, "#000000"); // Black for background
    vte_terminal_set_color_foreground(terminal, &fg_color);
    vte_terminal_set_color_background(terminal, &bg_color);

    // Set a vintage font
    PangoFontDescription *font_desc = pango_font_description_from_string("Monospace 12");
    vte_terminal_set_font(terminal, font_desc);
    pango_font_description_free(font_desc);

    // Corrected type for spawn_flags
    GSpawnFlags spawn_flags = static_cast<GSpawnFlags>(0);

    // Get the default user shell
    gchar *user_shell = getenv("SHELL");

    // Spawn the user's default shell
    gchar **envp = g_get_environ();
    gchar **command = g_strsplit(user_shell, " ", -1);
    vte_terminal_spawn_async(terminal,
                             VTE_PTY_DEFAULT,
                             NULL,         // working directory
                             command,      // command
                             envp,         // environment variables
                             spawn_flags,  // corrected spawn flags
                             NULL,         // child setup function
                             NULL,         // user data
                             NULL,         // child setup data destroy
                             -1,           // timeout (use default)
                             NULL,         // cancellable
                             NULL,         // callback
                             NULL);        // user data destroy

    g_strfreev(envp);
    g_strfreev(command);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("org.bnuy.term", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
