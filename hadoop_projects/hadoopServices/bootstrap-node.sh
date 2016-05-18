echo "checking if agent is running..."
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color
GREEN='\033[0;32m'
ISOK=$(sudo ambari-agent status|grep 'Agent PID at')
if [ "" == "$ISOK" ]; then
  	echo -e "${YELLOW}STATUS: ${NC}Agent is ${RED}NOT ${NC}running, now start it"
	sudo ambari-agent start
else
	echo -e "${YELLOW}STATUS: ${NC}Agent is ${GREEN}running${NC}"
fi
