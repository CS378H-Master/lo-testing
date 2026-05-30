# WS-4 brace-migration report

Mode: applied

Files scanned: 152
Files migrated (changed): 147
Files already in new form (no-op): 5
Files skipped (parse anomaly): 0

Totals across migrated corpus:
  classes parsed:          111
  methods:                 375
  constructors relocated:  54
  constructors (existing): 4
  body-blocks stripped:    419

## No-op files (already redesigned / regression anchors)
  - LO-3/InvalidPrograms/test_4.lo
  - LO-3/InvalidPrograms/test_8.lo
  - LO-3/ValidPrograms/test_14_string_reverse_codepoint.lo
  - LO-3/ValidPrograms/test_18.lo
  - LO-3/ValidPrograms/test_88.lo

## Flags for SC
  - LO-3/InvalidPrograms/test_6.lo: top-level method outside any class at offset 92 (handled: brace-stripped; intentional invalid test?)
  - LO-3/ValidPrograms/test_88.lo: top-level method outside any class at offset 124 (handled: brace-stripped; intentional invalid test?)
  - LO-3/ValidPrograms/test_88.lo: top-level method outside any class at offset 342 (handled: brace-stripped; intentional invalid test?)
