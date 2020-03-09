```c
int audit_user_log_get_time(audit_user_log_h handle,
int audit_user_log_get_pid(audit_user_log_h handle, pid_t *pid);
int audit_user_log_get_type(audit_user_log_h handle, int *type);
int audit_user_log_get_text(audit_user_log_h handle, char **text);

int audit_trail_clear_user_log(audit_trail_h handle);
int audit_trail_foreach_user_log(audit_trail_h handle,
int audit_trail_add_user_log_cb(audit_trail_h handle,
int audit_trail_remove_user_log_cb(audit_trail_h handle,

int audit_system_log_get_time(audit_system_log_h handle,
int audit_system_log_get_subject_name(audit_system_log_h handle,
.
.
.
int audit_system_log_get_object_inode(audit_system_log_h handle, ino_t *inode);
int audit_system_log_get_object_dev(audit_system_log_h handle, dev_t *dev);

int audit_trail_clear_system_log(audit_trail_h handle);
int audit_trail_foreach_system_log(audit_trail_h handle,
int audit_trail_add_system_log_cb(audit_trail_h handle,
int audit_trail_remove_system_log_cb(audit_trail_h handle,
```
