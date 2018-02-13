/* This is a managed file. Do not delete this comment. */

#include <driver/mnt/filestore/filestore.h>

int16_t filestore_mount_construct(
    filestore_mount this)
{
    corto_mount_setContentType(this, "text/json");
    return corto_super_construct(this);
}

void filestore_mount_on_notify(
    filestore_mount this,
    corto_subscriber_event *event)
{
    char *dir = corto_asprintf("%s/%s", this->storedir, event->data.parent);
    char *file = corto_asprintf("%s/%s.json", dir, event->data.id);
    if (event->event == CORTO_DELETE) {
        if (corto_rm(file)) corto_error("failed to delete file '%s'", file);
    } else {
        if (corto_mkdir(dir)) {
            corto_error("failed to create directory '%s'", dir);
        } else {
            FILE *f = fopen(file, "w");
            fprintf(f, "{\"id\":\"%s%s%s\",\"type\":\"%s\",\"value\":%s}\n",
                event->data.parent[0] == '.' ? "" : event->data.parent,
                event->data.parent[0] == '.' ? "" : "/",
                event->data.id,
                event->data.type,
                corto_result_getText(&event->data));
            fclose(f);
        }
    }
    free(file); free(dir);
}

corto_resultIter filestore_mount_on_query(
    filestore_mount this,
    corto_query *query)
{
    char *dir = corto_asprintf("%s/%s", this->storedir, query->from);
    corto_ll files = corto_opendir(dir);
    if (files) {
        char *prevCwd = strdup(corto_cwd());
        corto_chdir(dir);
        corto_iter it = corto_ll_iter(files);
        while (corto_iter_hasNext(&it)) {
            char *file = corto_iter_next(&it), *ext = strrchr(file, '.');
            if (ext && !strcmp(ext, ".json")) {
                /* Strip extension for matching */
                ext[0] = '\0';
                if (corto_idmatch(query->select, file)) {
                    /* Test if there is a directory with same name, in which case
                     * this file is not a leaf */
                    bool isDir = corto_isdir(file);
                    ext[0] = '.'; /* Restore extension */
                    char *json = corto_file_load(file);
                    if (json) {
                        corto_result r = {0};
                        if (corto_result_fromcontent(&r, "text/json", json)) {
                            corto_raise();
                            continue;
                        }
                        if (!isDir) {
                            r.flags = CORTO_RESULT_LEAF;
                        }
                        corto_mount_return(this, &r);
                        corto_ptr_deinit(&r, corto_result_o);
                    } else {
                        corto_error("filestore: failed to load file '%s'", file);
                        continue;
                    }
                }
            }
        }
        corto_chdir(prevCwd);
        free(prevCwd);
        corto_closedir(files);
    } else {
        corto_catch();
    }

    return CORTO_ITER_EMPTY; /* Using corto_mount_return */
}
