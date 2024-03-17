/*
 * color.c
 *
 * Copyright (C) 2010 Berislav Kovacki
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Authors: Berislav Kovacki <pantokrator@pantokrator.net>
 */

#include "config.h"

#include <string.h>
#include "zenity.h"
#include "util.h"

static ZenityData *zen_data;

static void zenity_colorselection_dialog_response (GtkWidget *widget, int response, gpointer data);

void zenity_colorselection (ZenityData *data, ZenityColorData *color_data)
{
  GtkWidget *dialog;
  GtkWidget *colorsel;
  GdkColor color;

  zen_data = data;

  dialog = gtk_color_selection_dialog_new (NULL);

  g_signal_connect (G_OBJECT (dialog), "response",
                    G_CALLBACK (zenity_colorselection_dialog_response),
                    color_data);

  if (data->dialog_title)
    gtk_window_set_title (GTK_WINDOW (dialog), data->dialog_title);

  colorsel = gtk_color_selection_dialog_get_color_selection (GTK_COLOR_SELECTION_DIALOG (dialog));

  if (color_data->color) {
    if (gdk_color_parse (color_data->color, &color))
      gtk_color_selection_set_current_color (GTK_COLOR_SELECTION (colorsel),
                                             &color);
  }

  gtk_color_selection_set_has_palette (GTK_COLOR_SELECTION (colorsel),
                                       color_data->show_palette);

  zenity_util_show_dialog (dialog);

  if (data->timeout_delay > 0) {
    g_timeout_add (data->timeout_delay * 1000,
                   (GSourceFunc) zenity_util_timeout_handle,
                   NULL);
  }

  gtk_main();
}

static void
zenity_colorselection_dialog_response (GtkWidget *widget, int response, gpointer data)
{
  GtkWidget *colorsel;
  GdkColor color;

  switch (response) {
    case GTK_RESPONSE_OK:
      zen_data->exit_code = zenity_util_return_exit_code (ZENITY_OK);
      colorsel = gtk_color_selection_dialog_get_color_selection (GTK_COLOR_SELECTION_DIALOG (widget));
      gtk_color_selection_get_current_color (GTK_COLOR_SELECTION (colorsel), &color);
      g_print ("%s\n", gdk_color_to_string (&color));
      break;

    case GTK_RESPONSE_CANCEL:
      zen_data->exit_code = zenity_util_return_exit_code (ZENITY_CANCEL);
      break;

    default:
      zen_data->exit_code = zenity_util_return_exit_code (ZENITY_ESC);
      break;
  }

  gtk_main_quit ();
}
