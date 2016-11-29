echo "checking if server is running..."
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color
GREEN='\033[0;32m'
ISOK=$(sudo ambari-server status|grep 'Ambari Server PID at')
if [ "" == "$ISOK" ]; then
  	echo -e "${YELLOW}STATUS: ${NC}Server is ${RED}NOT ${NC}running, now start it"
	sudo ambari-server start
	sudo ambari-agent start
else
	echo -e "${YELLOW}STATUS: ${NC}Server is ${GREEN}running${NC}"
fi
