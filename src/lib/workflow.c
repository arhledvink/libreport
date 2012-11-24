/*
    Copyright (C) 2011  ABRT team
    Copyright (C) 2010  RedHat Inc

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "event_config.h"
#include "workflow.h"
#include "internal_libreport.h"

GHashTable *g_workflow_list;

workflow_t *new_workflow(void)
{
    workflow_t *w = xzalloc(sizeof(*w));
    w->info = new_config_info();
    return w;
}

void free_workflow(workflow_t *w)
{
    if (!w)
        return;

    free_config_info(w->info);
    g_list_free_full(w->events, (GDestroyNotify)free_event_config);
    free(w);
}

static void free_workflow_cb(const char *name, workflow_t *w, gpointer user_data)
{
    free_workflow(w);
}

void free_workflow_list(GHashTable *wl)
{
    g_hash_table_foreach(wl, (GHFunc)free_workflow_cb, NULL);
    g_hash_table_destroy(wl);
}

workflow_t *get_workflow(const char *name)
{
    if (!g_workflow_list)
        return NULL;
    /* @@ FIXME: SYMLINKS@!!!
    if (g_event_config_symlinks)
    {
        char *link = g_hash_table_lookup(g_event_config_symlinks, name);
        if (link)
            name = link;
    }
    */
    return g_hash_table_lookup(g_workflow_list, name);
}

void load_workflow_config_data(const char* path)
{
    if (g_workflow_list == NULL)
    {
        g_workflow_list = g_hash_table_new_full(
                                        g_str_hash,
                                        g_str_equal,
                                        g_free,
                                        (GDestroyNotify) free_workflow
        );
    }

    GList *workflow_files = get_file_list(path, "xml");
    while(workflow_files)
    {
        file_obj_t *file = (file_obj_t *)workflow_files->data;

        workflow_t *workflow = get_workflow(file->filename);
        bool nw_workflow = (!workflow);
        if (nw_workflow)
            workflow = new_workflow();

        load_workflow_description_from_file(workflow, file->fullpath);

        if (nw_workflow)
            g_hash_table_replace(g_workflow_list, xstrdup(file->filename), workflow);

        workflow_files = g_list_next(workflow_files);
    }
    free_file_list(workflow_files);
}

config_item_info_t *workflow_get_config_info(workflow_t *w)
{
    return w->info;
}

GList *wf_get_event_list(workflow_t *w)
{
    return w->events;
}

const char *wf_get_name(workflow_t *w)
{
    return ci_get_name(workflow_get_config_info(w));
}

const char *wf_get_screen_name(workflow_t *w)
{
    return ci_get_screen_name(workflow_get_config_info(w));
}

const char *wf_get_description(workflow_t *w)
{
    return ci_get_description(workflow_get_config_info(w));
}

const char *wf_get_long_desc(workflow_t *w)
{
    return ci_get_long_desc(workflow_get_config_info(w));
}

void wf_set_name(workflow_t *w, const char* name)
{
    ci_set_name(workflow_get_config_info(w), name);
}

void wf_set_description(workflow_t *w, const char* description)
{
    ci_set_description(workflow_get_config_info(w), description);
}

void wf_set_long_desc(workflow_t *w, const char* long_desc)
{
    ci_set_long_desc(workflow_get_config_info(w), long_desc);
}