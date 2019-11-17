cd $( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )

mkdir -p log
currentTime="`date +%Y%m%d_%H%M%S`"
kill $(ps aux | grep 'server.cpp.o 20001' | awk '{print $2}')
kill $(ps aux | grep '7777 avalon' | awk '{print $2}')
nohup ./server.cpp.o 20001 &> log/cpp_$currentTime.txt &
sleep 3
source ~/venv37/bin/activate
nohup gunicorn --bind 0.0.0.0:7777 avalon:server &> log/dash_$currentTime.txt &
