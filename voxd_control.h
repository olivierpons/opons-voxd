// SPDX-License-Identifier: MIT
/*
 * voxd_control.h - Control socket contract shared by the opons-voxd
 * daemon and the opons-voxd-mcp server.
 *
 * Copyright (C) 2026 Olivier Pons
 *
 * Protocol: one request per connection. The client sends "status",
 * "unload" or "reload" followed by '\n'; the daemon answers one line
 * starting with "ok " or "err " and closes the socket. A reload is
 * answered only once loading has finished.
 */

#ifndef VOXD_CONTROL_H
#define VOXD_CONTROL_H

#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

/* Release version, reported by both binaries. */
#define OPONS_VOXD_VERSION  "1.1.0"

/**
 * control_socket_path - Path of the daemon's control socket.
 * @buf: out, receives the path.
 * @n: size of @buf.
 *
 * Derived from the uid alone (not from XDG_RUNTIME_DIR), so the
 * daemon and an MCP client spawned with a stripped environment
 * always agree on it: /run/user/<uid>/opons-voxd.sock when that
 * directory exists, /tmp/opons-voxd-<uid>.sock otherwise.
 */
static inline void control_socket_path(char *buf, size_t n)
{
    char dir[64];
    struct stat st;

    snprintf(dir, sizeof(dir), "/run/user/%u", (unsigned)getuid());
    if (stat(dir, &st) == 0 && S_ISDIR(st.st_mode))
        snprintf(buf, n, "%s/opons-voxd.sock", dir);
    else
        snprintf(buf, n, "/tmp/opons-voxd-%u.sock",
                 (unsigned)getuid());
}

#endif /* VOXD_CONTROL_H */
