// SPDX-License-Identifier: MIT
/*
 * opons_voxd_mcp.c - Model Context Protocol server for opons-voxd.
 *
 * Copyright (C) 2026 Olivier Pons
 *
 * Newline-delimited JSON-RPC 2.0 on stdin/stdout, logs on stderr.
 * Spawned by the AI client for the length of its session, it
 * forwards each tool call to the daemon's control socket
 * (voxd_control.h). Kept apart from the daemon binary and linked
 * against libc only, so each running copy costs about a megabyte of
 * RAM instead of loading GTK, CUDA and Whisper.
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "voxd_control.h"

/* ---- static prototypes ---- */

static int control_request(const char *cmd, char *reply, size_t n);
static const char *json_ws(const char *p);
static const char *json_skip(const char *p);
static bool json_get(const char *obj, const char *key,
                     const char **val, size_t *len);
static bool json_str_eq(const char *val, size_t len, const char *s);
static void json_put_str(const char *s);
static void mcp_send_result(const char *id, size_t id_len,
                            const char *result_json);
static void mcp_send_error(const char *id, size_t id_len,
                           int code, const char *msg);
static void mcp_initialize(const char *id, size_t id_len,
                           const char *params);
static void mcp_tools_call(const char *id, size_t id_len,
                           const char *params);
static void mcp_handle_line(const char *line);
static void usage(FILE *out, const char *prog);

/* ---- MCP server ---- */

#define MCP_SERVER_VERSION  "1.0.0"
#define MCP_LATEST_PROTOCOL "2025-11-25"
#define CONTROL_TIMEOUT_SEC 120

static const char *const mcp_protocols[] = {
    "2025-11-25", "2025-06-18", "2025-03-26", "2024-11-05",
};

#define MCP_TOOL_ANNOTATIONS(read_only) \
    "\"annotations\":{\"readOnlyHint\":" read_only "," \
    "\"destructiveHint\":false,\"idempotentHint\":true," \
    "\"openWorldHint\":false}"

static const char mcp_tools_json[] =
    "{\"tools\":["
    "{\"name\":\"unload_models\","
    "\"title\":\"Unload dictation models (free GPU memory)\","
    "\"description\":\"Free the GPU memory (VRAM) held by the "
    "opons-voxd speech-to-text daemon by unloading its Whisper "
    "models. Call this before a GPU-heavy local workload (local LLM, "
    "image model, training...). Voice dictation stays disabled until "
    "reload_models is called, so always call reload_models once that "
    "work is done. Fails with 'busy' while the user is dictating: "
    "wait a few seconds and retry.\","
    "\"inputSchema\":{\"type\":\"object\",\"properties\":{}},"
    MCP_TOOL_ANNOTATIONS("false") "},"
    "{\"name\":\"reload_models\","
    "\"title\":\"Reload dictation models\","
    "\"description\":\"Load the opons-voxd Whisper models back into "
    "GPU memory and re-enable voice dictation. Blocks until loading "
    "has finished (a few seconds). Call it as soon as the GPU work "
    "that required unload_models is done, even if that work "
    "failed.\","
    "\"inputSchema\":{\"type\":\"object\",\"properties\":{}},"
    MCP_TOOL_ANNOTATIONS("false") "},"
    "{\"name\":\"get_status\","
    "\"title\":\"Dictation models status\","
    "\"description\":\"Report the opons-voxd daemon state: loaded "
    "(idle, recording or transcribing), unloaded (GPU memory freed, "
    "dictation disabled) or loading.\","
    "\"inputSchema\":{\"type\":\"object\",\"properties\":{}},"
    MCP_TOOL_ANNOTATIONS("true") "}"
    "]}";

static const char mcp_instructions[] =
    "opons-voxd is the user's always-on local voice dictation daemon. "
    "Its Whisper models occupy several GB of GPU memory. Call "
    "unload_models before a GPU-heavy local workload and "
    "reload_models right after it, even if that workload failed.";

/**
 * control_request - Send one command to the daemon's control socket.
 * @cmd: "status", "unload" or "reload".
 * @reply: out, the daemon's explanation without its "ok"/"err"
 *         prefix, or why it could not be reached.
 * @n: size of @reply.
 *
 * Return: 0 if the daemon answered "ok", 1 if it answered "err",
 * -1 if it could not be reached or sent no valid answer.
 */
static int control_request(const char *cmd, char *reply, size_t n)
{
    struct sockaddr_un addr;
    struct timeval tv = { .tv_sec = CONTROL_TIMEOUT_SEC, .tv_usec = 0 };
    char line[64];
    size_t used = 0;
    ssize_t r;
    int fd;
    int saved;

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    control_socket_path(addr.sun_path, sizeof(addr.sun_path));
    fd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if (fd < 0 ||
        connect(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        saved = errno;
        if (fd >= 0)
            close(fd);
        snprintf(reply, n,
                 "opons-voxd daemon not reachable on %s (%s): it must "
                 "be running and started with --mcp",
                 addr.sun_path, strerror(saved));
        return -1;
    }
    (void)setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    snprintf(line, sizeof(line), "%s\n", cmd);
    (void)!send(fd, line, strlen(line), MSG_NOSIGNAL);
    while (used < n - 1) {
        r = recv(fd, reply + used, n - 1 - used, 0);
        if (r <= 0)
            break;
        used += (size_t)r;
    }
    close(fd);
    reply[used] = '\0';
    reply[strcspn(reply, "\n")] = '\0';
    if (strncmp(reply, "ok ", 3) == 0) {
        memmove(reply, reply + 3, strlen(reply + 3) + 1);
        return 0;
    }
    if (strncmp(reply, "err ", 4) == 0) {
        memmove(reply, reply + 4, strlen(reply + 4) + 1);
        return 1;
    }
    snprintf(reply, n,
             "no valid answer from the opons-voxd daemon within %d s",
             CONTROL_TIMEOUT_SEC);
    return -1;
}

static const char *json_ws(const char *p)
{
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    return p;
}

/**
 * json_skip - Step over the JSON value starting at @p.
 * @p: start of a value (leading whitespace allowed).
 *
 * Checks only what is needed to find value boundaries: string
 * escapes and bracket nesting. Mismatched bracket kinds go unnoticed.
 *
 * Return: pointer just past the value, NULL if it is truncated or
 * malformed.
 */
static const char *json_skip(const char *p)
{
    int depth = 0;

    p = json_ws(p);
    do {
        if (*p == '"') {
            for (p++; *p != '"'; p++) {
                if (!*p)
                    return NULL;
                if (*p == '\\' && !*++p)
                    return NULL;
            }
            p++;
        } else if (*p == '{' || *p == '[') {
            depth++;
            p++;
        } else if (*p == '}' || *p == ']') {
            if (depth == 0)
                return NULL;
            depth--;
            p++;
        } else if (!*p) {
            return NULL;
        } else if (depth == 0) {
            const char *start = p;

            while (*p && !strchr(",:}] \t\r\n", *p))
                p++;
            return p == start ? NULL : p;
        } else {
            p++;
        }
    } while (depth > 0);
    return p;
}

/* Raw JSON token @val/@len is the string @s (no escapes in @s). */
static bool json_str_eq(const char *val, size_t len, const char *s)
{
    size_t slen = strlen(s);

    return len == slen + 2 && val[0] == '"' &&
           memcmp(val + 1, s, slen) == 0 && val[slen + 1] == '"';
}

/**
 * json_get - Find a top-level member of a JSON object.
 * @obj: start of the object; need not be NUL-terminated after it.
 * @key: member name (compared without unescaping).
 * @val: out, start of the raw value (strings keep their quotes).
 * @len: out, length of the raw value.
 *
 * Return: true if found.
 */
static bool json_get(const char *obj, const char *key,
                     const char **val, size_t *len)
{
    const char *p = json_ws(obj);
    const char *k;
    const char *k_end;
    const char *end;

    if (*p != '{')
        return false;
    p = json_ws(p + 1);
    while (*p == '"') {
        k = p;
        k_end = json_skip(p);
        if (!k_end)
            return false;
        p = json_ws(k_end);
        if (*p != ':')
            return false;
        p = json_ws(p + 1);
        end = json_skip(p);
        if (!end)
            return false;
        if (json_str_eq(k, (size_t)(k_end - k), key)) {
            *val = p;
            *len = (size_t)(end - p);
            return true;
        }
        p = json_ws(end);
        if (*p != ',')
            return false;
        p = json_ws(p + 1);
    }
    return false;
}

/* Write @s to stdout as a quoted, escaped JSON string. */
static void json_put_str(const char *s)
{
    const unsigned char *p;

    putchar('"');
    for (p = (const unsigned char *)s; *p; p++) {
        if (*p == '"' || *p == '\\')
            printf("\\%c", *p);
        else if (*p == '\n')
            fputs("\\n", stdout);
        else if (*p < 0x20)
            printf("\\u%04x", *p);
        else
            putchar(*p);
    }
    putchar('"');
}

static void mcp_send_result(const char *id, size_t id_len,
                            const char *result_json)
{
    printf("{\"jsonrpc\":\"2.0\",\"id\":%.*s,\"result\":%s}\n",
           (int)id_len, id, result_json);
    fflush(stdout);
}

/* @id NULL: the request id is unknown, answered as null. */
static void mcp_send_error(const char *id, size_t id_len,
                           int code, const char *msg)
{
    printf("{\"jsonrpc\":\"2.0\",\"id\":%.*s,"
           "\"error\":{\"code\":%d,\"message\":",
           id ? (int)id_len : 4, id ? id : "null", code);
    json_put_str(msg);
    fputs("}}\n", stdout);
    fflush(stdout);
}

/*
 * Answers with the client's protocol version when it is one we know
 * (the tool surface is identical in all of them), otherwise with the
 * latest one, letting the client decide whether to go on.
 */
static void mcp_initialize(const char *id, size_t id_len,
                           const char *params)
{
    const char *ver = MCP_LATEST_PROTOCOL;
    const char *val;
    size_t len;
    size_t i;

    if (json_get(params, "protocolVersion", &val, &len)) {
        for (i = 0; i < sizeof(mcp_protocols) / sizeof(mcp_protocols[0]); i++) {
            if (json_str_eq(val, len, mcp_protocols[i])) {
                ver = mcp_protocols[i];
                break;
            }
        }
    }
    printf("{\"jsonrpc\":\"2.0\",\"id\":%.*s,\"result\":{"
           "\"protocolVersion\":\"%s\","
           "\"capabilities\":{\"tools\":{}},"
           "\"serverInfo\":{\"name\":\"opons-voxd\","
           "\"version\":\"" MCP_SERVER_VERSION "\"},"
           "\"instructions\":",
           (int)id_len, id, ver);
    json_put_str(mcp_instructions);
    fputs("}}\n", stdout);
    fflush(stdout);
}

/*
 * Daemon refusals ("busy", daemon unreachable) are tool results
 * with isError, not JSON-RPC errors, so the model sees them and can
 * retry; only an unknown tool name is a protocol error.
 */
static void mcp_tools_call(const char *id, size_t id_len,
                           const char *params)
{
    const char *name;
    const char *cmd;
    size_t len;
    char reply[512];
    int rc;

    if (!json_get(params, "name", &name, &len)) {
        mcp_send_error(id, id_len, -32602, "missing tool name");
        return;
    }
    if (json_str_eq(name, len, "unload_models"))
        cmd = "unload";
    else if (json_str_eq(name, len, "reload_models"))
        cmd = "reload";
    else if (json_str_eq(name, len, "get_status"))
        cmd = "status";
    else {
        mcp_send_error(id, id_len, -32602, "unknown tool");
        return;
    }
    rc = control_request(cmd, reply, sizeof(reply));
    fprintf(stderr, "mcp: %s -> %s %s\n", cmd,
            rc == 0 ? "ok" : "err", reply);
    printf("{\"jsonrpc\":\"2.0\",\"id\":%.*s,\"result\":{"
           "\"content\":[{\"type\":\"text\",\"text\":",
           (int)id_len, id);
    json_put_str(reply);
    printf("}],\"isError\":%s}}\n", rc == 0 ? "false" : "true");
    fflush(stdout);
}

/*
 * Messages without an id are notifications and never answered;
 * messages without a method are responses to server requests, which
 * this server never sends, so they are ignored too.
 */
static void mcp_handle_line(const char *line)
{
    const char *p = json_ws(line);
    const char *method;
    const char *id;
    const char *params = "{}";
    size_t mlen;
    size_t id_len;
    size_t plen;
    bool has_id;

    if (!*p)
        return;
    if (*p == '[') {
        mcp_send_error(NULL, 0, -32600,
                       "batch requests are not supported");
        return;
    }
    if (*p != '{' || !json_skip(p)) {
        mcp_send_error(NULL, 0, -32700, "parse error");
        return;
    }
    has_id = json_get(p, "id", &id, &id_len);
    if (!json_get(p, "method", &method, &mlen)) {
        if (!has_id)
            mcp_send_error(NULL, 0, -32600, "invalid request");
        return;
    }
    if (!has_id)
        return;
    (void)json_get(p, "params", &params, &plen);
    if (json_str_eq(method, mlen, "initialize"))
        mcp_initialize(id, id_len, params);
    else if (json_str_eq(method, mlen, "ping"))
        mcp_send_result(id, id_len, "{}");
    else if (json_str_eq(method, mlen, "tools/list"))
        mcp_send_result(id, id_len, mcp_tools_json);
    else if (json_str_eq(method, mlen, "tools/call"))
        mcp_tools_call(id, id_len, params);
    else
        mcp_send_error(id, id_len, -32601, "method not found");
}

static void usage(FILE *out, const char *prog)
{
    fprintf(out,
            "Usage: %s [-h]\n"
            "\n"
            "MCP server on stdin/stdout, spawned by an AI client. Lets\n"
            "it unload/reload the Whisper models of the opons-voxd\n"
            "daemon, which must run with --mcp.\n"
            "\n"
            "  -h, --help   show this help and exit\n",
            prog);
}

int main(int argc, char **argv)
{
    static const struct option long_opts[] = {
        { "help", no_argument, NULL, 'h' },
        { NULL,   0,           NULL, 0   },
    };
    char *line = NULL;
    size_t cap = 0;
    int opt;

    while ((opt = getopt_long(argc, argv, "h", long_opts, NULL)) != -1) {
        if (opt == 'h') {
            usage(stdout, argv[0]);
            return 0;
        }
        usage(stderr, argv[0]);
        return 2;
    }
    fprintf(stderr, "mcp: stdio server ready\n");
    while (getline(&line, &cap, stdin) != -1)
        mcp_handle_line(line);
    free(line);
    return 0;
}
