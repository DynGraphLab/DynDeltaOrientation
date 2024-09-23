#!/bin/bash

# Use the first tag that points to the current HEAD
# if no tag is found, the latest git commit is used as a fallback

CURRENT_TAG=$(git tag -l --points-at HEAD | head -n1)
CURRENT_COMMIT=$(git rev-parse HEAD)
CURRENT_DESCRIBE=$(git describe --always --dirty)

echo "STABLE_VERSION \"${CURRENT_TAG:-$CURRENT_COMMIT}\""
echo "STABLE_SCM_DESCRIBE \"$CURRENT_DESCRIBE\""
# rules_nodejs expects to read from volatile-status.txt
echo "BUILD_SCM_VERSION \"${CURRENT_TAG:-$CURRENT_COMMIT}\""
echo "BUILD_SCM_DESCRIBE \"$CURRENT_DESCRIBE\""