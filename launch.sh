#!/bin/bash
# launch.sh — Start voice_in with rotating logs.
# Keeps voice_in.log (current) + voice_in.log.old (previous).
# Rotates when the current log exceeds 5 MB.

cd /home/olivier/voice_in_linux

LOG="voice_in.log"
MAX_BYTES=5242880  # 5 MB

if [ -f "$LOG" ] && [ "$(stat -c%s "$LOG" 2>/dev/null)" -gt "$MAX_BYTES" ]; then
    mv -f "$LOG" "${LOG}.old"
fi

echo "=== voice_in started at $(date) ===" >> "$LOG"
exec ./voice_in 2>>"$LOG"
