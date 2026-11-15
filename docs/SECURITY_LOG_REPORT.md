# SEC-004 — Invalid Configuration Audit Log Report

**Commit tested:** 6611142d24af6c5f02a2dd65f143a3dc60ddf21c  
**Workflow:** https://github.com/rujul-rj/Team11_SE/actions/runs/43694595234  
**Test:** testPersistentInvalidConfigurationLogging

## Procedure

1. Submit an invalid signal configuration with an out-of-range green duration.
2. Submit another invalid signal configuration with an out-of-range yellow duration.
3. Submit a configuration with a newline-injected intersection identifier.
4. Inspect SignalManager::invalid_attempt_log() and the configured log sink.

## Expected and observed result

- All three invalid configurations are rejected.
- Exactly three audit records are created for the three single-error attempts.
- Every record includes a timestamp and the INVALID_CONFIG marker.
- Each record is appended as one line to the configured sink.
- Newline characters in the source field are sanitized, so the injected FORGED text cannot create a forged log entry.
- The sample application appends signal and network configuration errors to logs/invalid-config.log.
- The logs/ directory is excluded from Git by .gitignore.

**Result: PASS in the automated CTest suite and CI.**

## Limitation

The application does not yet implement log rotation, retention policy or a separate audit-log viewer.
