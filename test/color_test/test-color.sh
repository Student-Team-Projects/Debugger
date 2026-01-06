#!/bin/bash
RELPATH="$(dirname "$0")"
cd "$RELPATH"
OWNPATH="$(pwd)"
cd "$OWNPATH"

RED='\033[1;31m'
GREEN='\033[1;32m'
BLUE='\033[1;34m'
NC='\033[0m'

echo -e "${RED}normal red${GREEN} normal green"
echo -e "normal green returns${NC}"
>&2 echo -e "${RED}error red"
echo -e "${GREEN}normal green${NC}"
>&2 echo -e "${GREEN}error green${NC}"
echo -e "${BLUE}normal blue${NC}"
>&2 echo -e "${BLUE}error blue${NC}"

exit 0