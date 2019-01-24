/* This is a managed file. Do not delete this comment. */

#include <driver.mnt.filestore>

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
    char *dir = ut_asprintf("%s/%s", this->storedir, event->data.parent);
    char *file = ut_asprintf("%s/%s.json", dir, event->data.id);

    if (event->event == CORTO_DELETE) {
        if (ut_rm(file)) ut_error("failed to delete file '%s'", file);
    } else {
        if (ut_mkdir(dir)) {
            ut_error("failed to create directory '%s'", dir);
        } else {
            FILE *f = fopen(file, "w");
            fprintf(f, "{\"id\":\"%s\",\"type\":\"%s\",\"value\":%s}\n",
                event->data.id,
                event->data.type,
                corto_record_get_text(&event->data));
            fclose(f);
        }
    }

    free(file);
    free(dir);
}

corto_recordIter filestore_mount_on_query(
    filestore_mount this,
    corto_query *query)
{

    char *dir = ut_asprintf("%s/%s", this->storedir, query->from);
    ut_ll files = ut_opendir(dir);
    if (files) {
        char *prevCwd = strdup(ut_cwd());
        ut_chdir(dir);
        ut_iter it = ut_ll_iter(files);
        while (ut_iter_hasNext(&it)) {
            char *file = ut_iter_next(&it), *ext = strrchr(file, '.');

            if (ext && !strcmp(ext, ".json")) {
                /* Strip extension for matching */
                ext[0] = '\0';
                if (ut_expr(query->select, file)) {
                    /* Test if there is a directory with same name, in which
                     * case this file is not a leaf */
                    bool isDir = ut_isdir(file);
                    ext[0] = '.'; /* Restore extension */
                    char *json = ut_file_load(file);
                    if (json) {
                        corto_record r = {0};
                        if (corto_record_fromcontent(&r, "text/json", json)) {
                            ut_raise();
                            continue;
                        }
                        if (!isDir) {
                            r.flags = CORTO_RECORD_LEAF;
                        }
                        corto_set_str(&r.parent, query->from);

                        corto_mount_return(this, &r);

                        corto_ptr_deinit(&r, corto_record_o);
                    } else {
                        ut_error(
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
                if (ut_isdir(file) && !ut_file_test(dir_with_ext)) {
                    corto_record r = {
                        .parent = query->from,
                        .id = file,
                        .type = "unknown",
                        .flags = CORTO_RECORD_HIDDEN | CORTO_RECORD_UNKNOWN
                    };
                    corto_mount_return(this, &r);
                }
            }
        }
        ut_chdir(prevCwd);
        free(prevCwd);
        ut_closedir(files);
    } else {
        ut_catch();
    }

    return UT_ITER_EMPTY; /* Using corto_mount_return */
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
