# Static Analysis Report

**Tool:** cppcheck 2.19.0  
**Date:** 15 November 2026  
**Workflow:** https://github.com/rujul-rj/Team11_SE/actions/runs/43694595234  
**Job:** static-analysis

## Command

```bash
cppcheck --enable=warning,performance,portability \
  --inline-suppr --suppress=missingIncludeSystem \
  --error-exitcode=1 --std=c++17 -I include -I . \
  include src tms
```

## Result

**PASS.** Cppcheck completed analysis of the C++ implementation files under include/, src/ and tms/ with no warning, performance or portability findings that caused the job to fail.

The same workflow run also passed the AddressSanitizer/UndefinedBehaviorSanitizer job. This is evidence for SEC-003; retain the workflow URL with the final QA submission.
