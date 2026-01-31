#!/bin/bash
RELPATH="$(dirname "$0")"
cd "$RELPATH"
OWNPATH="$(pwd)"
cd "$OWNPATH"

BLUE='\033[1;34m'

echo "START (30s)"
>&2 echo "INFO: open index.html !!!!"

for i in {1..3}
do
   sleep 10
   >&2 echo -e "${BLUE}$((i * 10))s passed..${NC}"
done

echo "END"
exit 0