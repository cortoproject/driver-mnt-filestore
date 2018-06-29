/* This is a managed file. Do not delete this comment. */

#include <driver/mnt/filestore/filestore.h>

int16_t filestore_mount_construct(
    filestore_mount this)
{
    corto_mount_set_format(this, "text/json");
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
            fprintf(f, "{\"id\":\"%s\",\"type\":\"%s\",\"value\":%s}\n",
                event->data.id,
                event->data.type,
                corto_result_get_text(&event->data));
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
                    /* Test if there is a directory with same name, in which
                     * case this file is not a leaf */
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
                        corto_set_str(&r.parent, query->from);

                        corto_mount_return(this, &r);

                        corto_ptr_deinit(&r, corto_result_o);
                    } else {
                        corto_error(
                            "filestore: failed to load file '%s'", file);
                        continue;
                    }
                }
            } else {
                corto_id dir_with_ext;
                sprintf(dir_with_ext, "%s.json", file);

                /* If a directory is found, return a hidden unknown object. Only
                 * return directory as separate object when no matching JSON
                 * file is found */
                if (corto_isdir(file) && !corto_file_test(dir_with_ext)) {
                    corto_result r = {
                        .parent = query->from,
                        .id = file,
                        .type = "unknown",
                        .flags = CORTO_RESULT_HIDDEN
                    };
                    corto_mount_return(this, &r);
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

int16_t filestore_mount_init(
    filestore_mount this)
{
    if (corto_super_init(this)) {
        return -1;
    }

    this->super.ownership = CORTO_LOCAL_SOURCE;
    return 0;
}
